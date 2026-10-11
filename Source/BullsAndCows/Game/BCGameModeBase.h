#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BCGameModeBase.generated.h"

class ABCPlayerController;

UCLASS()
class BULLSANDCOWS_API ABCGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

public:
	virtual void BeginPlay() override;

	virtual void OnPostLogin(AController* NewPlayer) override;

	virtual void Logout(AController* Exiting) override;

	FString GenerateSecretNumber();

	bool IsGuessNumberString(const FString& InNumberString);

	void IncreaseGuessCount(ABCPlayerController* InChattingPlayerController);

	FString JudgeResult(const FString& InSecretNumberString, const FString& InGuessNumberString);

	void ResetGame();

	void JudgeGame(ABCPlayerController* InChattingPlayerController, int InStrikeCount);

	void PrintChatMessageString(ABCPlayerController* InChattingPlayerController, const FString& InChatMessageString);

	void OnTurnTimeExpired();

protected:
	void StartTurn(ABCPlayerController* InPlayerController);

	void AdvanceTurn();

protected:
	TArray<TObjectPtr<ABCPlayerController>> AllPlayerControllers;

	FString SecretNumberString;

	FTimerHandle TurnTimerHandle;

	float TurnDuration = 15.0f;
};
