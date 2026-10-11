#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BCNotificationWidget.generated.h"

class UTextBlock;

UCLASS()
class BULLSANDCOWS_API UBCNotificationWidget : public UUserWidget
{
    GENERATED_BODY()

protected:
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

protected:
    UPROPERTY(meta = (BindWidget))
    TObjectPtr<UTextBlock> TextBlock_Timer;
};