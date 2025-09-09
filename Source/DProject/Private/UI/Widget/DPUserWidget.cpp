// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/Widget/DPUserWidget.h"

void UDPUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
