// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BorshHUD.generated.h"

class UAttributeMenuWidgetController;
class UAttributeSet;
class UAbilitySystemComponent;
class UBorshUserWidget;
class UOverlayWidgetController;
class USpellMenuWidgetController;
struct FWidgetControllerParams;

/**
 * 
 */
UCLASS()
class GASLATAR_API ABorshHUD : public AHUD
{
	GENERATED_BODY()

public:

	UOverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UAttributeMenuWidgetController* GetAttributeMenuWidgetController(const FWidgetControllerParams& WCParams);
	USpellMenuWidgetController* GetSpellMenuWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);

protected:


private:

	UPROPERTY()
	TObjectPtr<UBorshUserWidget>  OverlayWidget;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UBorshUserWidget> OverlayWidgetClass;

	UPROPERTY()
	TObjectPtr<UOverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UOverlayWidgetController> OverlayWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<UAttributeMenuWidgetController> AttributeMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UAttributeMenuWidgetController> AttributeMenuWidgetControllerClass;

	UPROPERTY()
	TObjectPtr<USpellMenuWidgetController> SpellMenuWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<USpellMenuWidgetController> SpellMenuWidgetControllerClass;


};
