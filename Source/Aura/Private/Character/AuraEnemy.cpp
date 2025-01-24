// YanWei.


#include "Character/AuraEnemy.h"

#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAbilitySystemComponent.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/MyASBlueprintFunctionLibrary.h"
#include "Aura/Aura.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "UI/Widget/AuraUserWidget.h"

AAuraEnemy::AAuraEnemy()
{
	//设置碰撞响应  --用于高亮
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block); //设置可视为阻挡

	AbilitySystemComponent = CreateDefaultSubobject<UAuraAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	
	AttributeSet = CreateDefaultSubobject<UAuraAttributeSet>("AttributeSet");

	HealthBar = CreateDefaultSubobject<UWidgetComponent>("HealthBar");
	HealthBar->SetupAttachment(GetRootComponent());
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

void AAuraEnemy::Die()
{
	SetLifeSpan(LifeSpan); //在Enemy死亡后尸体会存在五秒 对象不再需要时会自动销毁
	Super::Die();
}

void AAuraEnemy::HitReactTagChanged(const FGameplayTag CallbackTag, int32 NewCount)
{
	//利用监听的标签和添加标签的个数 设置受击状态和基础速度
	bHitReacting = NewCount > 0;
	GetCharacterMovement()->MaxWalkSpeed = bHitReacting ? 0.f : BaseWalkSpeed;
}

void AAuraEnemy::BeginPlay()
{
	Super::BeginPlay();
	
	InitAbilityActorInfo();
	
	//初始化角色的技能
	UMyASBlueprintFunctionLibrary::GiveStartupAbilities(this, AbilitySystemComponent);
	
	//将敌人的基类作为控制器设置给用户控件，可以在用户控件绑定对应监听
	if(UAuraUserWidget* UserWidget = Cast<UAuraUserWidget>(HealthBar->GetUserWidgetObject()))
	{
		UserWidget->SetWidgetController(this);  //可以回溯F12研究 这里是为了在设置控制器后执行event
	}

	if(UAuraAttributeSet* AS = Cast<UAuraAttributeSet>(AttributeSet))
	{
		//监听血量变化
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnHealthChanged.Broadcast(Data.NewValue);
			}
			);
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(AS->GetMaxHealthAttribute()).AddLambda(
			[this](const FOnAttributeChangeData& Data)
			{
				OnMaxHealthChanged.Broadcast(Data.NewValue);
			}
			);
		//实现对受击标签的监听，在源码中，受击标签会返回监听的标签和添加标签的个数  Callback func会接收这两个参
		AbilitySystemComponent->RegisterGameplayTagEvent(FMyGameplayTags::Get().Effects_HitReact,
			EGameplayTagEventType::NewOrRemoved).AddUObject(this,&AAuraEnemy::HitReactTagChanged);
		
		
		//初始化血量
		OnHealthChanged.Broadcast(AS->GetHealth());
		OnMaxHealthChanged.Broadcast(AS->GetMaxHealth());
	}

	
}

void AAuraEnemy::InitAbilityActorInfo()
{
	//初始化ActorInfo
	AbilitySystemComponent->InitAbilityActorInfo(this,this);
	//单向了解
	Cast<UAuraAbilitySystemComponent>(AbilitySystemComponent)->AbilityActorInfoSet();
	//通过GE初始角色的属性
	InitializeDefaultAttributes();


}

void AAuraEnemy::InitializeDefaultAttributes() const
{
	//重写覆盖父类初始化函数，但删去Super
	UMyASBlueprintFunctionLibrary::InitializeDefaultAttributes(this,CharacterClass,Level,AbilitySystemComponent);
}
