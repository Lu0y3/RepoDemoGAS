// YanWei.


#include "UI/Widget/AuraUserWidget.h"

void UAuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	//将捕获的InWidgetController设置为WidgetController 后调用WidgetControllerSet()
	WidgetController = InWidgetController;
	WidgetControllerSet();
	
}
