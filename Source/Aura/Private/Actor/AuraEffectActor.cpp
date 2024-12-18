// YanWei.


#include "Actor/AuraEffectActor.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

/*#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "Components/SphereComponent.h"
#include "Interaction/EnemyInterface.h"*/


AAuraEffectActor::AAuraEffectActor()
{
	PrimaryActorTick.bCanEverTick = false;

	//TODO::在蓝图中添加一些组件
	SetRootComponent(CreateDefaultSubobject<USceneComponent>(FName("SceneComponent")));
	
	/*Mesh = CreateDefaultSubobject<UStaticMeshComponent>("Mesh");
	SetRootComponent(Mesh);
	
	Sphere = CreateDefaultSubobject<USphereComponent>("Sphere");
	Sphere->SetupAttachment(GetRootComponent());*/
	
}

/*void AAuraEffectActor::OnOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	//TODO:改变AttributeSet 的某些属性值
	
	//暂时使用const_cast 做权宜之计  后面将其更改成GameplayEffect
	//TODO:访问任意角色的能力系统并更改它的Effect属性
	if(IAbilitySystemInterface* ASCInterface = Cast<IAbilitySystemInterface>(OtherActor))
	{
		//从OtherActor的AbilitySystemComponent中获取UAttributeSet的子类实例 UAuraAttributeSet
		const UAuraAttributeSet* AuraAttributeSet = Cast<UAuraAttributeSet>(ASCInterface->GetAbilitySystemComponent()->GetAttributeSet(UAuraAttributeSet::StaticClass()));
		
		UAuraAttributeSet* MutableAuraAttributeSet= const_cast<UAuraAttributeSet*>(AuraAttributeSet); //常量强制转换
		MutableAuraAttributeSet->SetHealth(AuraAttributeSet->GetHealth() + 25.f);
		MutableAuraAttributeSet->SetMana(AuraAttributeSet->GetMana() - 25.f);

		Destroy();
	}
}*/

/*void AAuraEffectActor::EndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	
}*/

void AAuraEffectActor::BeginPlay()
{
	Super::BeginPlay();

	/*
	//动态多播委托绑定 overlap时调用函数
	Sphere->OnComponentBeginOverlap.AddDynamic(this,&AAuraEffectActor::OnOverlap);
	Sphere->OnComponentEndOverlap.AddDynamic(this,&AAuraEffectActor::EndOverlap);
	*/
	
}

void AAuraEffectActor::ApplyEffectToTarget(AActor* TargetActor,TSubclassOf<UGameplayEffect> GameplayEffectClass)
{
	//在蓝图中调用当Overlay时，用于ApplyEffect给Target
	//UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor) 首先会返回实现了ASI的Actor的ASC，否则则返回 Actor->FindComponentByClass<UAbilitySystemComponent>()
	UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
	if (TargetASC == nullptr) return;
	check(GameplayEffectClass);
	
	FGameplayEffectContextHandle EffectContextHandle = TargetASC->MakeEffectContext();
	EffectContextHandle.AddSourceObject(this);
	
	const FGameplayEffectSpecHandle EffectSpecHandle = TargetASC->MakeOutgoingSpec(GameplayEffectClass,ActorLevel, EffectContextHandle);
	const FActiveGameplayEffectHandle ActiveEffectHandle = TargetASC->ApplyGameplayEffectSpecToSelf(*EffectSpecHandle.Data.Get()); //.Data得到封装的TSPtr.Get()得到原始指针，然后解引用

	//EffectSpecHandle.Data.Get()->Def.Get() 可以得到到Effect*然后访问GE蓝图类默认值CD
	const bool bIsInfinite = EffectSpecHandle.Data.Get()->Def.Get()->DurationPolicy == EGameplayEffectDurationType::Infinite;
	if (bIsInfinite && InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		ActiveEffectHandles.Add(ActiveEffectHandle, TargetASC);
	}

	//TODO::未来有时间处理一下Instant和Duration被动移除而不是等时限结束的情况  如被打断之类的
}

void AAuraEffectActor::OnBeginOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnBeginOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
}

void AAuraEffectActor::OnEndOverlap(AActor* TargetActor)
{
	if (InstantEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InstantGameplayEffectClass);
	}
	if (DurationEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, DurationGameplayEffectClass);
	}
	if (InfiniteEffectApplicationPolicy == EEffectApplicationPolicy::ApplyOnEndOverlap)
	{
		ApplyEffectToTarget(TargetActor, InfiniteGameplayEffectClass);
	}
	
	if (InfiniteEffectRemovalPolicy == EEffectRemovalPolicy::RemoveOnEndOverlap)
	{
		//TODO::移除TargetASC中包含在TMap的Handle 并移除TMap中的Handle
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor);
		if (!IsValid(TargetASC)) return;
		
		TArray<FActiveGameplayEffectHandle> HandlesToRemove;
		for (const TTuple<FActiveGameplayEffectHandle, UAbilitySystemComponent*>& HandlePair : ActiveEffectHandles)
		{
			if (TargetASC == HandlePair.Value)
			{
				TargetASC->RemoveActiveGameplayEffect(HandlePair.Key,1);//堆叠移除单一堆栈，不至于发生还在N火里但不掉血的情况
				HandlesToRemove.Add(HandlePair.Key);
			}
		}
		for (FActiveGameplayEffectHandle& Handle : HandlesToRemove)
		{
			ActiveEffectHandles.FindAndRemoveChecked(Handle);
		}
	}
	
}


