// YanWei.


#include "AbilitySystem/MMC/MMC_MaxHealth.h"

#include "AbilitySystem/AuraAttributeSet.h"
#include "Interaction/CombatInterface.h"

UMMC_MaxHealth::UMMC_MaxHealth()
{
	//GE--修饰符-修饰符幅度-计算类型(属性基础)-基于属性的幅度-支持属性的三个选项
	//1要捕获的Attribute
	VigorDef.AttributeToCapture = UAuraAttributeSet::GetVigorAttribute();
	//2属性源-- Source/Target
	//VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Source;
	VigorDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	//3快照--指的是在某个时间点捕获和存储属性（例如健康、护甲等）的值，这样可以在稍后的时间点使用这些捕获的值进行计算，而不需要依赖实时状态变化。
	VigorDef.bSnapshot = false;

	//一个TArray<FGameplayEffectAttributeCaptureDefinition>
	RelevantAttributesToCapture.Add(VigorDef);
}

float UMMC_MaxHealth::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	//Gather tags from source and target 搜集tags  聚合标签(AggregatedTags) 是指包括 CapturedSourceTags 中的所有标签
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags(); 
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	//FAggregatorEvaluateParameters 是一个用来传递和存储属性评估时所需参数的结构体。
	FAggregatorEvaluateParameters EvaluateParameters;
	EvaluateParameters.SourceTags = SourceTags;
	EvaluateParameters.TargetTags = TargetTags;
	//EvaluateParameters.IgnoreHandles GE
	
	float Vigor = 0.f;
	GetCapturedAttributeMagnitude(VigorDef,Spec,EvaluateParameters,Vigor);
	//此时Vigor将具有Target上的属性
	Vigor = FMath::Max<float>(Vigor,0.f); //让Vigor值非负用于计算

	//TODO::原因暂不确定 ??为什么上面获得的可以是Target 而下面是Source??  因为到目前为止Apply有关Character属性GEtoSelf的源目标都是同一个对象
    ICombatInterface* CombatInterface = Cast<ICombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	//游戏的属性参数设计要平衡  这里是根据活力和玩家等级设置最大生命值
	const float MaxHealthValue = 2.5f*(Vigor + 0.f) + 80.f + 20.f * PlayerLevel;
	return MaxHealthValue;
}
