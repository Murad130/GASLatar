// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BorshWidgetController.h"
#include "SpellMenuWidgetController.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API USpellMenuWidgetController : public UBorshWidgetController
{
	GENERATED_BODY()
	
public:

	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependencies() override;
};
