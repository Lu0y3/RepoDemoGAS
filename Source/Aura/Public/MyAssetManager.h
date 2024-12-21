// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "MyAssetManager.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMyAssetManager : public UAssetManager
{
	GENERATED_BODY()
public:
	static UMyAssetManager& Get();

protected:
	//最初初始化
	virtual void StartInitialLoading() override;
};
