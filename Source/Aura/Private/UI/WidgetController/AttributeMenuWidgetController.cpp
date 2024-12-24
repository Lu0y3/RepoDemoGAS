// YanWei.


#include "UI/WidgetController/AttributeMenuWidgetController.h"

#include "MyGameplayTags.h"
#include "AbilitySystem/AuraAttributeSet.h"
#include "AbilitySystem/Data/AttributeInfo.h"


void UAttributeMenuWidgetController::BroadcastInitialValues()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);
	//TODO::将Info的属性值与AS对应绑定
	/*FAuraAttributeInfo Infotest01 = AttributeInfo->FindAttributeInfoForTag(FMyGameplayTags::Get().Attributes_Primary_Strength);
	Infotest01.AttributeValue = AS->GetStrength();
	//这里直接传是因为在蓝图中已经设置好了Info的其他部分
	AttributeInfoDelegate.Broadcast(Infotest01);

	FAuraAttributeInfo Infotest02 = AttributeInfo->FindAttributeInfoForTag(FMyGameplayTags::Get().Attributes_Secondary_Armor);
	Infotest02.AttributeValue = AS->GetArmor();
	AttributeInfoDelegate.Broadcast(Infotest02);*/

	//TODO::制作属性TMap 4、初始化
	for (auto& Pair : AS->TagsToAttributes)
	{
		FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
		//Execute()表示运行委托中绑定的逻辑，并返回结果是函数或者委托。GetNumericValue(AS)访问指定属性的当前值。
		//Info.AttributeValue = Pair.Value.Execute().GetNumericValue(AS); //委托
		Info.AttributeValue = Pair.Value().GetNumericValue(AS); //函数
		AttributeInfoDelegate.Broadcast(Info);
	}


	
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	UAuraAttributeSet* AS = CastChecked<UAuraAttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		//TODO::制作属性TMap 5、更新属性 GetGameplayAttributeValueChangeDelegate--Callfunc广播新属性
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
		[this,Pair,AS](const FOnAttributeChangeData& Data)
		{
			FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(Pair.Key);
			Info.AttributeValue = Pair.Value().GetNumericValue(AS);
			AttributeInfoDelegate.Broadcast(Info);
		});
	}
	
}

/*void UAttributeMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAuraAttributeInfo Info = AttributeInfo->FindAttributeInfoForTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);  //AttributeSet缺少强制转换过程？？
	AttributeInfoDelegate.Broadcast(Info);
}*/
