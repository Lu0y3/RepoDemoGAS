// YanWei.


#include "MyAssetManager.h"
#include "MyGameplayTags.h"

UMyAssetManager& UMyAssetManager::Get()
{
	check(GEngine);
	UMyAssetManager* MyAssetManager = Cast<UMyAssetManager>(GEngine->AssetManager);
	return *MyAssetManager;
}

void UMyAssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();
	
	FMyGameplayTags::InitializeNativeGameplayTag();	//让MyGameplayTags中的Tags有效初始化
	
	
}
