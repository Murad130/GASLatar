// Copyright Latar


#include "AbilitySystem/AbilityTasks/TargetDataUnderMouse.h"
#include "AbilitySystemComponent.h"

UTargetDataUnderMouse* UTargetDataUnderMouse::CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility)
{
	UTargetDataUnderMouse* MyObj = NewAbilityTask<UTargetDataUnderMouse>(OwningAbility);
	return MyObj;
}

void UTargetDataUnderMouse::Activate()
{

	const bool bIsLocallyControlled = Ability->GetCurrentActorInfo()->IsLocallyControlled();
	if (bIsLocallyControlled)
	{
		SendMouseCursorData();
	}
	else
	{
		// Spec handle for CallReplicatedEventDelegateIfSet
		const FGameplayAbilitySpecHandle SpecHandle = GetAbilitySpecHandle();
		// Prediction key from CallReplicatedEventDelegateIfSet
		const FPredictionKey ActivationPredictonKey = GetActivationPredictionKey();
		// TODO: We are on the server, so we need to listen for target data
		AbilitySystemComponent.Get()->AbilityTargetDataSetDelegate(SpecHandle, ActivationPredictonKey).AddUObject(this, &UTargetDataUnderMouse::OnTargetDataReplicatedCallBack);
		// But what happens if we're too late and the target data has already been sent and the delegate has already been broadcast? Well, in that case we should call that callback anyway.
		// So there's a way to check, or at least a way to call that target data delegate if it's already received target data
		const bool bCalledDelegate = AbilitySystemComponent.Get()->CallReplicatedTargetDataDelegatesIfSet(SpecHandle, ActivationPredictonKey);
		if (!bCalledDelegate)
		{
			SetWaitingOnRemotePlayerData();
		}
	}
}

void UTargetDataUnderMouse::SendMouseCursorData()
{
	// we're creating a scoped prediction window, which is a window during which everything we do here locally is going to be predicted.
	FScopedPredictionWindow ScopedPrediction(AbilitySystemComponent.Get());

	// We'll call this function if we are locally controlled.
	// if we're locally controlled, we can call this function and broadcast our delegate.
	APlayerController* PC = Ability->GetCurrentActorInfo()->PlayerController.Get();
	FHitResult CursorHit;
	PC->GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	// Now we can just get this data and set its hit result

	// Now we have target data, but we don't have a data handle. We're going to need to create one and package up this target data into that handle.
	FGameplayAbilityTargetDataHandle DataHandle;
	FGameplayAbilityTargetData_SingleTargetHit* Data = new FGameplayAbilityTargetData_SingleTargetHit();
	Data->HitResult = CursorHit;
	DataHandle.Add(Data);

	// We have a target data object, but we're going to have to send this up to the server, right? And that means we need to get the ability system component.
	AbilitySystemComponent->ServerSetReplicatedTargetData(
		GetAbilitySpecHandle(), 
		GetActivationPredictionKey(), 
		DataHandle, 
		FGameplayTag(), 
		AbilitySystemComponent->ScopedPredictionKey); // anything that's predicted is associated with a prediction key

	// Now SendMouseCursorData is sending target data up to the server.

	// Now that doesn't mean we shouldn't still broadcast the valid data locally so that we can see that cursor
	// But there's a function we should check. Because there are certain circumstances where we should not broadcast our ability task delegates. This makes sure the ability is still active.
	// So if the ability is not still active, this will prevent us from broadcasting when we shouldn't.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
	// So we'll send that data up to the server and on the server we need to receive it.
}

void UTargetDataUnderMouse::OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag)
{
	// This is the function taht we need to bind to the delegate when we are on the server in Activate function

	// This function will be called in response to receiving replicated target data.

	// So this tells the ability system. Okay, target data has been received, don't keep it stored. We don't need to keep that cached.
	AbilitySystemComponent->ConsumeClientReplicatedTargetData(GetAbilitySpecHandle(), GetActivationPredictionKey());
	// So now that we've done that, we can broadcast the delegate, but of course we should check should broadcast ability task delegates.
	if (ShouldBroadcastAbilityTaskDelegates())
	{
		ValidData.Broadcast(DataHandle);
	}
}
