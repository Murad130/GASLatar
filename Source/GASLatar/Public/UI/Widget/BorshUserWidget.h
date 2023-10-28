// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BorshUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshUserWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;

protected:
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();


	
};
