// YanWei.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "AuraGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UAuraGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
public:
	//TODO::增加一个GameplayTag的配置属性-修改按键
	UPROPERTY(EditDefaultsOnly, Category="Input")
	FGameplayTag StartupInputTag;

	//TODO::去给元属性Damage分配SetbyCaller要使用的相关数据
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly ,Category="Damage")
	FScalableFloat Damage;
};
