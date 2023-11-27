#include "Player/BorshPlayerController.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "BorshGameplayTags.h"
#include "EnhancedInputSubsystems.h"
#include "NavigationPath.h"
#include "NavigationSystem.h"
#include "AbilitySystem/BorshAbilitySystemComponent.h"
#include "Components/SplineComponent.h"
#include "Input/BorshInputComponent.h"
#include "Interaction/EnemyInterface.h"
#include "GameFramework/Character.h"
#include "UI/Widget/DamageTextComponent.h"

ABorshPlayerController::ABorshPlayerController()
{
	bReplicates = true;
	// Constructing spline component
	Spline = CreateDefaultSubobject<USplineComponent>("Spline");
}

void ABorshPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
	CursorTrace();
	AutoRun();
}

void ABorshPlayerController::ShowDamageNumber_Implementation(float DamageAmount, ACharacter* TargetCharacter, bool bBlockedHit, bool bCriticalHit)
{
	if (IsValid(TargetCharacter) && DamageTextComponentClass && IsLocalController())
	{
		UDamageTextComponent* DamageText = NewObject<UDamageTextComponent>(TargetCharacter, DamageTextComponentClass);
		DamageText->RegisterComponent();
		DamageText->AttachToComponent(TargetCharacter->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
		DamageText->DetachFromComponent(FDetachmentTransformRules::KeepWorldTransform);
		DamageText->SetDamageText(DamageAmount, bBlockedHit, bCriticalHit);
	}
}
void ABorshPlayerController::AutoRun()
{
	if (!bAutoRunning) return;
	// Get our Controlled Pawn
	if (APawn* ControlledPawn = GetPawn())
	{
		// if we have a valid pawn, I would like the location on the spline that is closest to the pawn because our pawn may not always be exactly on the spline.
		// Our character will be slightly off from the spline.
		const FVector LocationOnSpline = Spline->FindLocationClosestToWorldLocation(ControlledPawn->GetActorLocation(), ESplineCoordinateSpace::World);
		// So now that we have that location, we can find the direction on the spline that corresponds to this location.
		const FVector Direction = Spline->FindDirectionClosestToWorldLocation(LocationOnSpline, ESplineCoordinateSpace::World);
		// And as soon as we have that direction, we now know which direction to move the controlled pawn. So we're going to take ou
		ControlledPawn->AddMovementInput(Direction);
		// We want to check our distance to the destination. We want to see if it's within auto run acceptance radius.
		const float DistanceToDestination = (LocationOnSpline - CachedDestination).Length();
		// And then we can check this distance.
		if (DistanceToDestination <= AutoRunAcceptanceRadius)
		{
			// if distance is less than or equal to our auto run acceptance we should no longer be auto running
			bAutoRunning = false;
		}
	}
}

void ABorshPlayerController::CursorTrace()
{
	GetHitResultUnderCursor(ECC_Visibility, false, CursorHit);
	if (!CursorHit.bBlockingHit) return;
	LastActor = ThisActor;
	ThisActor = Cast<IEnemyInterface>(CursorHit.GetActor());
	if (LastActor != ThisActor)
	{
		if (LastActor) LastActor->UnHighlightActor();
		if (ThisActor) ThisActor->HighlightActor();
	}
}

void ABorshPlayerController::AbilityInputTagPressed(FGameplayTag InputTag)
{
	// First We need to check if we are pressing the left mouse button
	if (InputTag.MatchesTagExact(FBorshGameplayTags::Get().InputTag_LMB))
	{
		bTargeting = ThisActor ? true : false;
		// When we pressed left mouse button and if it's not an enemy we need to call bAutoRunning variable so our character moves to this location but we don't know if it's a short press or not yet
		// so we give to it false
		bAutoRunning = false;
	}
}

void ABorshPlayerController::AbilityInputTagReleased(FGameplayTag InputTag)
{
	// First We need to check if we are pressing the left mouse button
	if (!InputTag.MatchesTagExact(FBorshGameplayTags::Get().InputTag_LMB))
	{
		// if it's not left mouse button do this
		// Now I don't want to have to cast every single time as ability input tag held may be called every frame and that can be expensive. (Thanks to our GetASC)
		if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);
		return;
	}
	if (GetASC()) GetASC()->AbilityInputTagReleased(InputTag);

	// if this is left mouse button we are concerned about running 
	// Case 1 : if we are pressing the left mouse button down and we're targeting (we're hovering over an enemy) we are just activating the ability
	if (!bTargeting && !bShiftKeyDown)
	{
		// We need to get pawn location to pass it to our Navigation library as argument
		const APawn* ControlledPawn = GetPawn();
		if (FollowTime <= ShortPressThreshold && ControlledPawn)
		{
			// If that's the case, well then we want to do something different. We want to find a path. We want to create a navigation path, a set of points to follow. For this we can use Navigation Library
			if (UNavigationPath* NavPath = UNavigationSystemV1::FindPathToLocationSynchronously(this, ControlledPawn->GetActorLocation(), CachedDestination))
			{
				// So first we're going to take our spline and each time we set the spline points, we should clear all the points out that were there before.
				Spline->ClearSplinePoints();
				// Then we're going to loop through our path.
				for (const FVector& PointLoc : NavPath->PathPoints)
				{
					// So this is the path that we're concerned with and it starts at the controlled pawns location and it ends at the destination. So we have a set of points that we can add to our spline.
					Spline->AddSplinePoint(PointLoc, ESplineCoordinateSpace::World);
				}
				// So in the case where we would run off into the distance 
				// actually a case where we had no path points in the array.  
				// just check for that and only start running if we get at least one path 
				if(NavPath->PathPoints.Num() > 0)
				{
					CachedDestination = NavPath->PathPoints.Last();
					bAutoRunning = true;
				}
				// Our spline will now have points in our path, which means our be auto running boolean should be set to true.
				bAutoRunning = true;
			}
		}
		// And we're also no longer holding the mouse button down. That means we should reset our Follow time
		FollowTime = 0.f;
		// We should also set our bTargeting Boolean to false.
		bTargeting = false;
		// By far all we're doing is adding points to our spine (We are not going to be moving)
		// We need to set that in Tick Event
	}
}
void ABorshPlayerController::AbilityInputTagHeld(FGameplayTag InputTag)
{
	if (!InputTag.MatchesTagExact(FBorshGameplayTags::Get().InputTag_LMB))
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
		return;
	}
	if (bTargeting || bShiftKeyDown)
	{
		if (GetASC()) GetASC()->AbilityInputTagHeld(InputTag);
	}
	else
	{
		FollowTime += GetWorld()->GetDeltaSeconds();
		if (CursorHit.bBlockingHit) CachedDestination = CursorHit.ImpactPoint;


		if (APawn* ControlledPawn = GetPawn())
		{
			const FVector WorldDirection = (CachedDestination - ControlledPawn->GetActorLocation()).GetSafeNormal();
			ControlledPawn->AddMovementInput(WorldDirection);
		}
	}
}
UBorshAbilitySystemComponent* ABorshPlayerController::GetASC()
{
	if (BorshAbilitySystemComponent == nullptr)
	{
		BorshAbilitySystemComponent = Cast<UBorshAbilitySystemComponent>(UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(GetPawn<APawn>()));
	}
	return BorshAbilitySystemComponent;
}
void ABorshPlayerController::BeginPlay()
{
	Super::BeginPlay();
	check(BorshContext);
	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer());
	if (Subsystem)
	{
		Subsystem->AddMappingContext(BorshContext, 0);
	}
	bShowMouseCursor = true;
	DefaultMouseCursor = EMouseCursor::Default;
	FInputModeGameAndUI InputModeData;
	InputModeData.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	InputModeData.SetHideCursorDuringCapture(false);
	SetInputMode(InputModeData);
}
void ABorshPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	UBorshInputComponent* BorshInputComponent = CastChecked<UBorshInputComponent>(InputComponent);
	BorshInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ABorshPlayerController::Move);
	BorshInputComponent->BindAction(ShiftAction, ETriggerEvent::Started, this, &ABorshPlayerController::ShiftPressed);
	BorshInputComponent->BindAction(ShiftAction, ETriggerEvent::Completed, this, &ABorshPlayerController::ShiftReleased);
	BorshInputComponent->BindAbilityActions(InputConfig, this, &ThisClass::AbilityInputTagPressed, &ThisClass::AbilityInputTagReleased, &ThisClass::AbilityInputTagHeld);
}
void ABorshPlayerController::Move(const FInputActionValue& InputActionValue)
{
	const FVector2D InputAxisVector = InputActionValue.Get<FVector2D>();
	const FRotator Rotation = GetControlRotation();
	const FRotator YawRotation(0.f, Rotation.Yaw, 0.f);
	const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
	const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
	if (APawn* ControlledPawn = GetPawn<APawn>())
	{
		ControlledPawn->AddMovementInput(ForwardDirection, InputAxisVector.Y);
		ControlledPawn->AddMovementInput(RightDirection, InputAxisVector.X);
	}
}
