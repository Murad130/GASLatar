// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "EnhancedInputComponent.h"
#include "BorshInputConfig.h"
#include "BorshInputComponent.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshInputComponent : public UEnhancedInputComponent
{
	GENERATED_BODY()

public:

	template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
	void BindAbilityActions(const UBorshInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc);
	
};

template<class UserClass, typename PressedFuncType, typename ReleasedFuncType, typename HeldFuncType>
void UBorshInputComponent::BindAbilityActions(const UBorshInputConfig* InputConfig, UserClass* Object, PressedFuncType PressedFunc, ReleasedFuncType ReleasedFunc, HeldFuncType HeldFunc)
{
	check(InputConfig);

	for (const FBorshInputAction& Action : InputConfig->AbilityInputActions)
	{
		// binding the pressed, released and held functions for all of them.
		// So why are we binding the same functions to all of our inputs? Well, because we have the ability to pass along a gameplay tag.

		// So if we can bind the same three functions pressed, held, and released to all of our input actions and we can identify those inputs by tag, well then we can pass those along to the ability system component
		// which can then decide which abilities to activate.
		if (Action.InputAction && Action.InputTag.IsValid())
		{
			if (PressedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Started, Object, PressedFunc, Action.InputTag);
			}

			if (ReleasedFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Completed, Object, ReleasedFunc, Action.InputTag);
			}

			if (HeldFunc)
			{
				BindAction(Action.InputAction, ETriggerEvent::Triggered, Object, HeldFunc, Action.InputTag);
			}
		}

	}
}
