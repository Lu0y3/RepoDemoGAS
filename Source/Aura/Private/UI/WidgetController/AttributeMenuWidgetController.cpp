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
	FAuraAttributeInfo Infotest01 = AttributeInfo->FindAttributeInfoForTag(FMyGameplayTags::Get().Attributes_Primary_Strength);
	Infotest01.AttributeValue = AS->GetStrength();
	//这里直接传是因为在蓝图中已经设置好了Info的其他部分
	AttributeInfoDelegate.Broadcast(Infotest01);

	FAuraAttributeInfo Infotest02 = AttributeInfo->FindAttributeInfoForTag(FMyGameplayTags::Get().Attributes_Secondary_Armor);
	Infotest02.AttributeValue = AS->GetArmor();
	AttributeInfoDelegate.Broadcast(Infotest02);

	
}

void UAttributeMenuWidgetController::BindCallbacksToDependencies()
{
	
}
