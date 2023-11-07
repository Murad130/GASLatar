// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "AttributeInfo.generated.h"

// Struct to store all the information associated with a given attribute. So once an attribute changes, then we can broadcast this struct object up to the widget blueprints
// who will receive it and update themselves with that information.

USTRUCT(BlueprintType)
struct FBorshAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag = FGameplayTag();

	// the information that we need to pass up to the widget side is going to require some text for the attribute name in the attribute menu.
	// And whenever we have text that's user facing in a widget, we use Ftext.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText();

	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;
};


/**
 * 
 */
UCLASS()
class GASLATAR_API UAttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	// We need a function here that will take a gameplay tag and return the correct attribute info from AttributeInformation array based on the gameplay tag.
	FBorshAttributeInfo FindAttributeInfoForTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FBorshAttributeInfo> AttributeInformation;
};
