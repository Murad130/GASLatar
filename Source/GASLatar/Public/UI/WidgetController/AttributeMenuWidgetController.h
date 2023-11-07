// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "UI/WidgetController/BorshWidgetController.h"
#include "AttributeMenuWidgetController.generated.h"

class UAttributeInfo;
struct FBorshAttributeInfo;
// Declaring delegate to broadcast our Attribute Info Struct
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FAttributeInfoSignature, const FBorshAttributeInfo&, Info);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class GASLATAR_API UAttributeMenuWidgetController : public UBorshWidgetController
{
	GENERATED_BODY()

public:

	// Overriding BindCallbacksToDependencies and BroadcastInitialValues
	virtual void BindCallbacksToDependencies() override;
	virtual void BroadcastInitialValues() override;

	UPROPERTY(BlueprintAssignable, Category = "GAS|Attributes")
	FAttributeInfoSignature AttributeInfoDelegate;
	// So we have this delegate. We can broadcast so we can broadcast the value of strength.
	// We don't need to fill those attributes (With name and description) here because we already have our data asset in Blueprint 

protected:

	UPROPERTY(EditDefaultsOnly)
	TObjectPtr<UAttributeInfo> AttributeInfo;

};