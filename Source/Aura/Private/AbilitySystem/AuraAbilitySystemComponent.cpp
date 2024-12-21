// YanWei.


#include "AbilitySystem/AuraAbilitySystemComponent.h"

#include "AuraDebugHelper.h"

void UAuraAbilitySystemComponent::AbilityActorInfoSet()
{
	//（首先从CharacterInitAAInfo那里调用初始绑定）  绑定委托当GE被应用时调用EffectApplied(a,b,c)并传入三个参数
	OnGameplayEffectAppliedDelegateToSelf.AddUObject(this,&UAuraAbilitySystemComponent::EffectApplied);
	
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
