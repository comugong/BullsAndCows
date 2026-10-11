#include "BCGameModeBase.h"
#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "Player/BCPlayerState.h"
#include "EngineUtils.h"

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
		BCPlayerController->NotificationText = FText::FromString(TEXT("Connected to the game server."));
		AllPlayerControllers.Add(BCPlayerController);

		ABCPlayerState* BCPS = BCPlayerController->GetPlayerState<ABCPlayerState>();
		if (IsValid(BCPS) == true)
		{
			BCPS->PlayerNameString = TEXT("Player") + FString::FromInt(AllPlayerControllers.Num());

			ABCGameStateBase* BCGameStateBase = GetGameState<ABCGameStateBase>();
			if (IsValid(BCGameStateBase) == true)
			{
				BCGameStateBase->MulticastRPCBroadcastLoginMessage(BCPS->PlayerNameString);
			}
		}
	}
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

	for (const auto& CXPlayerController : AllPlayerControllers)
	{
		ABCPlayerState* BCPS = CXPlayerController->GetPlayerState<ABCPlayerState>();
		if (IsValid(BCPS) == true)
		{
			BCPS->CurrentGuessCount = 0;
		}
	}
}

void ABCGameModeBase::JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount)
{
	if (!IsValid(InChattingPlayerController))
	{
		return;
	}

	if (InStrikeCount == 3)
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
