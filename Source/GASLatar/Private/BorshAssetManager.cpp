// Copyright Latar


#include "BorshAssetManager.h"

#include "BorshGameplayTags.h"

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
}
