#include "BCGameModeBase.h"
#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "Player/BCPlayerState.h"
#include "EngineUtils.h"
#include "TimerManager.h"

void ABCGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	SecretNumberString = GenerateSecretNumber();
	UE_LOG(LogTemp, Error, TEXT("%s"), *SecretNumberString);
}

void ABCGameModeBase::OnPostLogin(AController* NewPlayer)
{
	Super::OnPostLogin(NewPlayer);

	ABCPlayerController* BCPlayerController = Cast<ABCPlayerController>(NewPlayer);
	if (IsValid(BCPlayerController) == true)
	{
		BCPlayerController->NotificationText = FText::FromString(TEXT("Connected to server."));
		AllPlayerControllers.Add(BCPlayerController);

		ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();
		if (IsValid(BCPS) == true)
		{
			BCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());

			ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();
			if (IsValid(BCGS) == true)
			{
				BCGS->MulticastRPCBroadcastLoginMessage(BCPS->PlayerNameString);
				
				if (!IsValid(BCGS->CurrentTurnPlayer))
				{
					StartTurn(BCPlayerController);
				}
			}
		}
	}
}

void ABCGameModeBase::Logout(AController* Exiting)
{
	ABCPlayerController* ExitingPC = Cast<ABCPlayerController>(Exiting);
	ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();

	ABCPlayerState* ExitingPS = IsValid(ExitingPC) ? ExitingPC->GetPlayerState<ABCPlayerState>() : nullptr;
	const bool bWasCurrentTurn = IsValid(BCGS) && IsValid(ExitingPS) && BCGS->CurrentTurnPlayer == ExitingPS;

	if (IsValid(ExitingPC))
	{
		AllPlayerControllers.Remove(ExitingPC);
	}

	if (bWasCurrentTurn)
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);

		BCGS->CurrentTurnPlayer = nullptr;

		AdvanceTurn();
	}

	Super::Logout(Exiting);
}

FString ABCGameModeBase::GenerateSecretNumber()
{
	TArray<int32> Numbers;
	for (int32 i = 1; i <= 9; ++i)
	{
		Numbers.Add(i);
	}

	FString Result;

	for (int32 i = 0; i < 3; ++i)
	{
		int32 Index = FMath::RandRange(0, Numbers.Num() - 1);
		Result.Append(FString::FromInt(Numbers[Index]));
		Numbers.RemoveAt(Index);
	}

	return Result;
}

bool ABCGameModeBase::IsGuessNumberString(const FString& InNumberString)
{
	bool bCanPlay = false;

	do {

		if (InNumberString.Len() != 3)
		{
			break;
		}

		bool bIsUnique = true;
		TSet<TCHAR> UniqueDigits;
		for (TCHAR C : InNumberString)
		{
			if (FChar::IsDigit(C) == false || C == '0')
			{
				bIsUnique = false;
				break;
			}

			if (UniqueDigits.Contains(C))
			{
				bIsUnique = false;
				break;
			}

			UniqueDigits.Add(C);
		}

		if (bIsUnique == false)
		{
			break;
		}

		bCanPlay = true;

	} while (false);

	return bCanPlay;
}

void ABCGameModeBase::IncreaseGuessCount(ABCPlayerController* InChattingPlayerController)
{
	ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();
	if (IsValid(BCPS) == true)
	{
		BCPS->CurrentGuessCount++;
	}
}

FString ABCGameModeBase::JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString)
{
	int32 StrikeCount = 0, BallCount = 0;

	for (int32 i = 0; i < 3; ++i)
	{
		if (InSecretNumberString[i] == InGuessNumberString[i])
		{
			StrikeCount++;
		}
		else
		{
			FString PlayerGuessChar = FString::Printf(TEXT("%c"), InGuessNumberString[i]);
			if (InSecretNumberString.Contains(PlayerGuessChar))
			{
				BallCount++;
			}
		}
	}

	if (StrikeCount == 0 && BallCount == 0)
	{
		return TEXT("OUT");
	}

	return FString::Printf(TEXT("%dS%dB"), StrikeCount, BallCount);
}

void ABCGameModeBase::ResetGame()
{
	SecretNumberString = GenerateSecretNumber();

	for (const auto& BCPC : AllPlayerControllers)
	{
		if (IsValid(BCPC))
		{
			BCPC->NotificationText = FText::FromString(TEXT("Connected to server"));

			ABCPlayerState* BCPS = BCPC->GetPlayerState<ABCPlayerState>();
			if (IsValid(BCPS) == true)
			{
				BCPS->CurrentGuessCount = 0;
			}
		}
	}
}

void ABCGameModeBase::JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (!IsValid(InChattingPlayerController))
	{
		return;
	}

	if (InStrikeCount == 5)
	{
		ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();

		if (!IsValid(BCPS))
		{
			return;
		}

		FString WinMessage = BCPS->PlayerNameString + TEXT(" WIN");

		for (const auto& BCPlayerController : AllPlayerControllers)
		{
			if (IsValid(BCPlayerController))
			{
				BCPlayerController->NotificationText = FText::FromString(WinMessage);
			}
		}

		ResetGame();

		return;
	}
	else
	{
		bool bIsDraw = true;
		for (const auto& BCPlayerController : AllPlayerControllers)
		{
			ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();
			if (IsValid(BCPS) == true)
			{
				if (BCPS->CurrentGuessCount < BCPS->MaxGuessCount)
				{
					bIsDraw = false;
					break;
				}
			}
		}

		if (bIsDraw)
		{
			for (const auto& BCPlayerController : AllPlayerControllers)
			{
				if (IsValid(BCPlayerController))
				{
					BCPlayerController->NotificationText = FText::FromString(TEXT("DRAW"));
				}
			}

			ResetGame();

			return;
		}
	}
}

void ABCGameModeBase::PrintChatMessageString(ABCPlayerController* InChattingPlayerController, const FString& InChatMessageString)
{
	if (!IsValid(InChattingPlayerController))
	{
		return;
	}

	ABCPlayerState* BCPS = InChattingPlayerController->GetPlayerState<ABCPlayerState>();
	if (!IsValid(BCPS))
	{
		return;
	}

	ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();
	if (!IsValid(BCGS) || BCGS->CurrentTurnPlayer != BCPS)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("당신의 턴이 아닙니다."));

		return;
	}

	if (BCGS->GetServerWorldTimeSeconds() >= BCGS->TurnEndTime)
	{
		InChattingPlayerController->ClientRPCPrintChatMessageString(TEXT("제한 시간이 종료되었습니다."));

		return;
	}

	const FString GuessNumberString = InChatMessageString;
	FString CombinedMessageString;

	if (!IsGuessNumberString(GuessNumberString))
	{
		CombinedMessageString = BCPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString + TEXT(" - 다시 입력하세요. (서로 다른 1~9 사이의 숫자 3자리)");
	}
	else if (BCPS->CurrentGuessCount >= BCPS->MaxGuessCount)
	{
		CombinedMessageString = BCPS->GetPlayerInfoString() + TEXT(": 추측 횟수를 모두 사용했습니다.");
	}
	else
	{
		const FString JudgeResultString = JudgeResult(SecretNumberString, GuessNumberString);

		IncreaseGuessCount(InChattingPlayerController);

		CombinedMessageString = BCPS->GetPlayerInfoString() + TEXT(": ") + InChatMessageString + TEXT(" -> ") + JudgeResultString;

		const int32 StrikeCount = FCString::Atoi(*JudgeResultString.Left(1));

		JudgeGame(InChattingPlayerController, StrikeCount);
		AdvanceTurn();
	}

	for (TActorIterator<ABCPlayerController> It(GetWorld()); It; ++It)
	{
		ABCPlayerController* BCPlayerController = *It;

		if (IsValid(BCPlayerController))
		{
			BCPlayerController->ClientRPCPrintChatMessageString(CombinedMessageString);
		}
	}
}

void ABCGameModeBase::OnTurnTimeExpired()
{
	ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();
	if (!IsValid(BCGS))
	{
		return;
	}

	ABCPlayerState* CurrentPS = BCGS->CurrentTurnPlayer;
	if (!IsValid(CurrentPS))
	{
		return;
	}

	ABCPlayerController* CurrentPC = nullptr;
	for (ABCPlayerController* PC : AllPlayerControllers)
	{
		if (IsValid(PC) && PC->GetPlayerState<ABCPlayerState>() == CurrentPS)
		{
			CurrentPC = PC;
			break;
		}
	}
	if (!IsValid(CurrentPC))
	{
		return;
	}

	if (CurrentPS->CurrentGuessCount < CurrentPS->MaxGuessCount)
	{
		++CurrentPS->CurrentGuessCount;

		const FString TimeoutMessage = CurrentPS->GetPlayerInfoString() + TEXT(": 제한 시간 초과!");

		for (ABCPlayerController* PC : AllPlayerControllers)
		{
			if (IsValid(PC))
			{
				PC->ClientRPCPrintChatMessageString(TimeoutMessage);
			}
		}
	}

	JudgeGame(CurrentPC, 0);
	AdvanceTurn();
}

void ABCGameModeBase::StartTurn(ABCPlayerController* InPlayerController)
{
	if (!IsValid(InPlayerController))
	{
		return;
	}

	ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();
	ABCPlayerState* BCPS = InPlayerController->GetPlayerState<ABCPlayerState>();

	if (!IsValid(BCGS) || !IsValid(BCPS))
	{
		return;
	}

	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
	BCGS->CurrentTurnPlayer = BCPS;
	BCGS->TurnEndTime = BCGS->GetServerWorldTimeSeconds() + TurnDuration;

	GetWorldTimerManager().SetTimer(
		TurnTimerHandle,
		this,
		&ABCGameModeBase::OnTurnTimeExpired,
		TurnDuration,
		false
	);

	UE_LOG(LogTemp, Log, TEXT("%s's turn start"), *BCPS->PlayerNameString);
}

void ABCGameModeBase::AdvanceTurn()
{
	ABCGameStateBase* BCGS = GetGameState<ABCGameStateBase>();

	if (!IsValid(BCGS) || AllPlayerControllers.IsEmpty())
	{
		return;
	}

	int32 CurrentIndex = AllPlayerControllers.IndexOfByPredicate(
		[BCGS](const TObjectPtr<ABCPlayerController>& PC)
		{
			return IsValid(PC) && PC->GetPlayerState<ABCPlayerState>() == BCGS->CurrentTurnPlayer;
		}
	);

	if (CurrentIndex == INDEX_NONE)
	{
		CurrentIndex = AllPlayerControllers.Num() - 1;
	}

	const int32 PlayerCount = AllPlayerControllers.Num();

	for (int32 Offset = 1; Offset <= PlayerCount; ++Offset)
	{
		const int32 NextIndex = (CurrentIndex + Offset) % PlayerCount;

		ABCPlayerController* NextPC = AllPlayerControllers[NextIndex];

		if (!IsValid(NextPC))
		{
			continue;
		}

		ABCPlayerState* NextPS = NextPC->GetPlayerState<ABCPlayerState>();

		if (!IsValid(NextPS))
		{
			continue;
		}

		if (NextPS->CurrentGuessCount >= NextPS->MaxGuessCount)
		{
			continue;
		}

		StartTurn(NextPC);

		return;
	}

	GetWorldTimerManager().ClearTimer(TurnTimerHandle);
}
