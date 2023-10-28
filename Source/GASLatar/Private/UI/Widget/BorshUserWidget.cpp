// Copyright Latar


#include "UI/Widget/BorshUserWidget.h"

void UBorshUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
