// YanWei.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraDebugHelper.h"
#include "AbilitySystem/Abilities/AuraGameplayAbility.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//（首先从CharacterInitAAInfo那里调用初始绑定）  绑定委托当GE被应用时调用EffectApplied(a,b,c)并传入三个参数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
	
}

void UAuraAbilitySystemComponent::AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities)
{
	for (const TSubclassOf<UGameplayAbility> AbilityClass : StartupAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass,1);
		
		//为了访问AuraGameplayAbility的 StartupInputTag  然后让该能力开始拥有这个标签
		if (const UAuraGameplayAbility* AuraAbility = Cast<UAuraGameplayAbility>(AbilitySpec.Ability))
		{
			//TODO::动态增加Tag给AbilityTag
			AbilitySpec.DynamicAbilityTags.AddTag(AuraAbility->StartupInputTag); //TODO::增加Tag
			GiveAbility(AbilitySpec);
		}
		
		//GiveAbilityAndActivateOnce(AbilitySpec); //赋予GAS游戏能力后立即执行一次，同时不能为const，不是持续的
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagHeld(const FGameplayTag& InputTag)
{
	if (!InputTag.IsValid()) return;
	//FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities()) //访问ASC中可激活的能力并返回它们的组
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag)) //TODO::匹配Tag
		{
			AbilitySpecInputPressed(AbilitySpec); //Spec.InputPressed = true; 按住 knowTo 按下
			if (!AbilitySpec.IsActive())
			{
				TryActivateAbility(AbilitySpec.Handle);
			}
		}
	}
}

void UAuraAbilitySystemComponent::AbilityInputTagReleased(const FGameplayTag& InputTag)
{
	//不是想取消能力而是告诉它你不能再被按了
	if (!InputTag.IsValid()) return;
	//FScopedAbilityListLock ActiveScopeLoc(*this);
	for (FGameplayAbilitySpec& AbilitySpec : GetActivatableAbilities())
	{
		if (AbilitySpec.DynamicAbilityTags.HasTagExact(InputTag) && AbilitySpec.IsActive())
		{
			AbilitySpecInputReleased(AbilitySpec); //Spec.InputPressed = false; 告知不能再被按了
			//InvokeReplicatedEvent(EAbilityGenericReplicatedEvent::InputReleased, AbilitySpec.Handle, AbilitySpec.ActivationInfo.GetActivationPredictionKey());
		}
	}
}

void UAuraAbilitySystemComponent::EffectApplied(UAbilitySystemComponent* AbilitySystemComponent,
                                                const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveGameplayEffectHandle)
{
	//TODO:: 当GE应用到自身时，可以执行的一些东西（Broadcast）  如：显示Info到HUD上
	//首先与WidgetController建立联系，然后让WC广播给Widget去显示到HUD上
	//现在让我们思考需要广播什么info给--HUD？？
	//应用GameplayTag作 Broadcast？WhatInfo？WhoWidget 的分类依据
	FGameplayTagContainer TagContainer;
	EffectSpec.GetAllAssetTags(TagContainer);   //自身带有Tag--health\Attack?
	/*EffectSpec.GetAllGrantedTags(TagContainer);*/ //被赋予的Tag--Buff/deBuff
	
	EffectAssetTagsCD.Broadcast(TagContainer); 
}
