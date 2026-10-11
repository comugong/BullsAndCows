#include "BCNotificationWidget.h"
#include "Game/BCGameStateBase.h"
#include "Player/BCPlayerState.h"
#include "Components/TextBlock.h"

void UBCNotificationWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!IsValid(TextBlock_Timer) || !IsValid(TextBlock_CurrentTurnPlayer))
    {
        return;
    }

    ABCGameStateBase* BCGS = GetWorld()->GetGameState<ABCGameStateBase>();

    if (!IsValid(BCGS) || !IsValid(BCGS->CurrentTurnPlayer))
    {
        TextBlock_Timer->SetText(FText::FromString(TEXT("대기 중")));

        TextBlock_CurrentTurnPlayer->SetText(FText::FromString(TEXT("현재 플레이어의 턴")));

        return;
    }

    const float RemainingTime = BCGS->TurnEndTime - BCGS->GetServerWorldTimeSeconds();
    const int32 Seconds = FMath::Max(0, FMath::CeilToInt(RemainingTime));
    TextBlock_Timer->SetText(FText::AsNumber(Seconds));

    const FString CurrentPlayerName = BCGS->CurrentTurnPlayer->PlayerNameString;
    const FText TurnPlayerText = FText::Format(FText::FromString(TEXT("현재 {0}의 턴")), FText::FromString(CurrentPlayerName));
    TextBlock_CurrentTurnPlayer->SetText(TurnPlayerText);
}