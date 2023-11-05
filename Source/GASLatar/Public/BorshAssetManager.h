// Copyright Latar

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "BorshAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class GASLATAR_API UBorshAssetManager : public UAssetManager
{
	GENERATED_BODY()

public:

	static UBorshAssetManager& Get();

protected:

	virtual void StartInitialLoading() override;
};
