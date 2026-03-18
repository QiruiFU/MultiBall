// Copyright Autonomix. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "NotificationWidget.generated.h"

class UTextBlock;

/**
 * Large centered notification that auto-fades after a duration.
 */
UCLASS()
class MULTIBALL_API UNotificationWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Show a message for the given duration, then auto-remove. */
	UFUNCTION(BlueprintCallable, Category = "UI")
	void ShowMessage(const FString& Message, float Duration = 2.0f);

protected:
	virtual void NativeConstruct() override;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget, OptionalWidget), Category = "UI")
	UTextBlock* MessageText;

private:
	FTimerHandle FadeTimerHandle;

	UFUNCTION()
	void HideMessage();
};
