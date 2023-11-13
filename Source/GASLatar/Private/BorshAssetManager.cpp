// Copyright Latar


#include "BorshAssetManager.h"

#include "BorshGameplayTags.h"
#include "AbilitySystemGlobals.h"

UBorshAssetManager& UBorshAssetManager::Get()
{
	check(GEngine);
	// Get the asset manager
	UBorshAssetManager* BorshAssetManager = Cast<UBorshAssetManager>(GEngine->AssetManager);
	return *BorshAssetManager;
}

void UBorshAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FBorshGameplayTags::InitializeNativeGameplayTags();

	// There's something we have to do that's very, very important if we want to use target dataand that is to use you ability system globals and call the function.
	// This is required to use Target Data!
	UAbilitySystemGlobals::Get().InitGlobalData();
}
