// YanWei.


#include "UI/HUD/AuraHUD.h"

#include "UI/Widget/AuraUserWidget.h"
#include "UI/WidgetController/OverlayWidgetController.h"

UOverlayWidgetController* AAuraHUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		//若是HUD没有WC则New一个新的Object  根据蓝图分配的UClass
		OverlayWidgetController = NewObject<UOverlayWidgetController>(this,OverlayWidgetControllerClass);
		//TODO::让OverlayController可以访问到WCParams,,其他同理 如MenuController
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		//首次HUD设置WC时，进行AS的多播委托绑定,该绑定用于每当ASValue发生改变时调用对应的回调func去广播NewValue
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

void AAuraHUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC,
	UAttributeSet* AS) 
{
	//TODO::去显示UI
	checkf(OverlayWidgetClass, TEXT("OverlayWidgetClass uninitialized , please fill out BP_AuraHUD"));
	checkf(OverlayWidgetControllerClass, TEXT("OverlayWidgetControllerClass uninitialized , please fill out BP_AureHUD"));
	//获得控件
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(),OverlayWidgetClass);
	OverlayWidget = Cast<UAuraUserWidget>(Widget);
	//获得控件控制器
	const FWidgetControllerParams WidgetControllerParams(PC,PS,ASC,AS);
	UOverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);
	//设置控件的控制器 联系二者
	OverlayWidget->SetWidgetController(WidgetController); //TODO::当调用该函数时同时触发蓝图实现WCSetEvent
	
	//之后调用WC的 广播初始化Values 自定义函数
	WidgetController->BroadcastInitialValues();
	
	Widget->AddToViewport();
}

