// YanWei.


#include "Player/AuraPlayerState.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Net/UnrealNetwork.h"

AAuraPlayerState::AAuraPlayerState()
{
	//TODO::GAS网络复制模式
	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	//网络更新频率  //每秒和服务器更新客户端频率 同步  任何需要复制的变量都会更新，使用GAS后可以设置的高一些
	NetUpdateFrequency = 144.f;
}

UAbilitySystemComponent* AAuraPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AAuraPlayerState::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	//TODO::进行网络复制的步骤: 3、变量Level  (#include "Net/UnrealNetwork.h")
	//将Level属性注册到服务器，在服务器有改动时，会自动复制到本地
	//DOREPLIFETIME_CONDITION_NOTIFY 三个是递进的
	//Ctrl+T 搜索"ELifetimeCondition"
	DOREPLIFETIME(AAuraPlayerState,Level);
	
}

void AAuraPlayerState::OnRep_Level(int32 OldLevel) const
{
	//使用委托通知Level 
}
