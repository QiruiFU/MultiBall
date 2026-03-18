// Copyright Autonomix. All Rights Reserved.

#include "NotificationWidget.h"
#include "Components/TextBlock.h"
#include "TimerManager.h"

void UNotificationWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}

void UNotificationWidget::ShowMessage(const FString& Message, float Duration)
{
	if (MessageText)
	{
		MessageText->SetText(FText::FromString(Message));
	}

	SetVisibility(ESlateVisibility::HitTestInvisible);

	// Clear any existing timer and set a new one
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(FadeTimerHandle);
		GetWorld()->GetTimerManager().SetTimer(FadeTimerHandle, this,
			&UNotificationWidget::HideMessage, Duration, false);
	}
}

void UNotificationWidget::HideMessage()
{
	SetVisibility(ESlateVisibility::Hidden);
}
