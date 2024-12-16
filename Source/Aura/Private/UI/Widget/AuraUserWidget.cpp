// YanWei.


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	//将捕获的InWidgetController设置为WidgetController 后调用WidgetControllerSet()事件
	WidgetController = InWidgetController;
	//当设置了WC时(在蓝图中调用this函数)触发下列事件蓝图Event
	WidgetControllerSet();

	//Tips:游戏刚开始时会先自动(PossessedBy)设置OverlayWidgetController
	//然后调用一次WidgetControllerSet()事件
	//让后在该事件执行的地方再次设置其他 小WidgetController ----三者此时是一个Widget控制器
}
