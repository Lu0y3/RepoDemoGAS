// YanWei.


#include "AbilitySystem/ExecCalc/ExecCalc_Damage.h"

#include "AbilitySystemComponent.h"
#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"

struct MyDamageStatics
{
	DECLARE_ATTRIBUTE_CAPTUREDEF(Armor);
	
	MyDamageStatics()
	{
		DEFINE_ATTRIBUTE_CAPTUREDEF(UAuraAttributeSet, Armor, Target, true);
	}
};

static const MyDamageStatics& DamageStatics()
{
	static MyDamageStatics DStatics; 
	return DStatics; //每次call it时都会得到相同的静态变量
}

UExecCalc_Damage::UExecCalc_Damage()
{
	RelevantAttributesToCapture.Add(MyDamageStatics().ArmorDef);
}

void UExecCalc_Damage::Execute_Implementation(const FGameplayEffectCustomExecutionParameters& ExecutionParams,
	FGameplayEffectCustomExecutionOutput& OutExecutionOutput) const
{
	UAbilitySystemComponent* SourceASC = ExecutionParams.GetSourceAbilitySystemComponent();
	UAbilitySystemComponent* TargetASC = ExecutionParams.GetTargetAbilitySystemComponent();

	AActor* SourceActor = SourceASC ? SourceASC->GetAvatarActor() : nullptr;
	AActor* TargetActor = TargetASC ? TargetASC->GetAvatarActor() : nullptr;

	const FGameplayEffectSpec& Spec = ExecutionParams.GetOwningSpec();
	
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); 
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluateParams;
	EvaluateParams.SourceTags = SourceTags;
	EvaluateParams.TargetTags = TargetTags;

	//从Set by Caller 获取Damage的伤害值
	float Damage = Spec.GetSetByCallerMagnitude(FMyGameplayTags::Get().Damage);
		
	//输出计算结果
	const FGameplayModifierEvaluatedData EvaluatedData(UAuraAttributeSet::GetIncomingDamageAttribute(), EGameplayModOp::Additive,Damage);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);
	
	/*//获取护甲
	float Armor = 0.f;
	ExecutionParams.AttemptCalculateCapturedAttributeMagnitude(MyDamageStatics().ArmorDef,EvaluateParams,Armor);
	Armor = FMath::Max<float>(0.f,Armor);
	++Armor;*/
	
	/*const FGameplayModifierEvaluatedData EvaluatedData(MyDamageStatics().ArmorProperty, EGameplayModOp::Additive,Armor);
	OutExecutionOutput.AddOutputModifier(EvaluatedData);*/
}
