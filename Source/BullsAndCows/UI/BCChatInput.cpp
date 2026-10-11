#include "BCChatInput.h"
#include "Game/BCGameStateBase.h"
#include "Player/BCPlayerState.h"
#include "Player/BCPlayerController.h"
#include "Components/EditableTextBox.h"

void UBCChatInput::NativeConstruct()
{
	Super::NativeConstruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == false)
	{
		EditableTextBox_ChatInput->OnTextCommitted.AddDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UBCChatInput::NativeDestruct()
{
	Super::NativeDestruct();

	if (EditableTextBox_ChatInput->OnTextCommitted.IsAlreadyBound(this, &ThisClass::OnChatInputTextCommitted) == true)
	{
		EditableTextBox_ChatInput->OnTextCommitted.RemoveDynamic(this, &ThisClass::OnChatInputTextCommitted);
	}
}

void UBCChatInput::UpdateChatInputState()
{
	APlayerController* PC = GetOwningPlayer();
	if (!IsValid(PC) || !IsValid(EditableTextBox_ChatInput))
	{
		return;
	}

	ABCGameStateBase* BCGS = GetWorld()->GetGameState<ABCGameStateBase>();
	ABCPlayerState* MyPS = PC->GetPlayerState<ABCPlayerState>();
	if (!IsValid(BCGS) || !IsValid(MyPS))
	{
		EditableTextBox_ChatInput->SetIsEnabled(false);
		return;
	}

	const bool bIsMyTurn = BCGS->CurrentTurnPlayer == MyPS;
	const bool bIsTimeRemaining = BCGS->GetServerWorldTimeSeconds() < BCGS->TurnEndTime;
	EditableTextBox_ChatInput->SetIsEnabled(bIsMyTurn && bIsTimeRemaining);
}

void UBCChatInput::OnChatInputTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::OnEnter)
	{
		APlayerController* OwningPlayerController = GetOwningPlayer();
		if (IsValid(OwningPlayerController) == true)
		{
			ABCPlayerController* OwningBCPlayerController = Cast<ABCPlayerController>(OwningPlayerController);
			if (IsValid(OwningBCPlayerController) == true)
			{
				OwningBCPlayerController->SetChatMessageString(Text.ToString());

				EditableTextBox_ChatInput->SetText(FText());
			}
		}
	}
}
