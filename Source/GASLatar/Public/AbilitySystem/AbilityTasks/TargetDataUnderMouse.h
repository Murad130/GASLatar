// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "TargetDataUnderMouse.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FMouseTargetDataSignature, const FGameplayAbilityTargetDataHandle&, DataHandle);


/**
 * 
 */
UCLASS()
class GASLATAR_API UTargetDataUnderMouse : public UAbilityTask
{
	GENERATED_BODY()

	// Asynchronous blueprint node which means it can have multiple output execution pins. And the way that we create those multiple output execution pins is by broadcasting delegates.

public:
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (DisplayName = "TargetDataUnderMouse",HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "true"))
	static UTargetDataUnderMouse* CreateTargetDataUnderMouse(UGameplayAbility* OwningAbility);

	UPROPERTY(BlueprintAssignable)
	FMouseTargetDataSignature ValidData; // And this will now become an output execution pin on the node.

	// So now we need ValidData to be broadcast nad for Data to give the location under the mouse
private:

	virtual void Activate() override;
	void SendMouseCursorData();

	void OnTargetDataReplicatedCallBack(const FGameplayAbilityTargetDataHandle& DataHandle, FGameplayTag ActivationTag);
	
};
