// YanWei.


#include "Character/AuraEnemy.h"

#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Aura/Aura.h"

AAuraEnemy::AAuraEnemy()
{
	//设置碰撞响应  --用于高亮
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");
	
}

void AAuraEnemy::HighlightActor()
{
	//bIsHighlighted = true; 用于Debug
	
	//渲染深度自定义渠道 模板值
	GetMesh()->SetRenderCustomDepth(true); 
	GetMesh()->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); //材质PP_Highlight
	Weapon->SetRenderCustomDepth(true);
	Weapon->SetCustomDepthStencilValue(CUSTOM_DEPTH_RED); //CUSTOM_DEPTH_RED在Aura自定义变量
	
}

void AAuraEnemy::UnHighlightActor()
{
	//bIsHighlighted = false;
	GetMesh()->SetRenderCustomDepth(false);
	Weapon->SetRenderCustomDepth(false);
}

int32 AAuraEnemy::GetPlayerLevel()
{
	return Level;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
}

void AAuraEnemy::InitAbilityActorInfo()
{
	//初始化ActorInfo
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	//单向了解
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
}
