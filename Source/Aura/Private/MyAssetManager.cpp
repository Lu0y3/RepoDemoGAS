// YanWei.


#include "MyAssetManager.h"

#include "AbilitySystemGlobals.h"
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
	
	UAbilitySystemGlobals::Get().InitGlobalData();  //用于TargetData UnderMouse.h 5.4以后已自动调用
}
