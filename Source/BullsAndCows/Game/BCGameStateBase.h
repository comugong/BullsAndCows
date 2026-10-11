#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BCGameStateBase.generated.h"

class ABCPlayerState;

UCLASS()
class BULLSANDCOWS_API ABCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString);

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

public:
	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Turn")
	TObjectPtr<ABCPlayerState> CurrentTurnPlayer;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Turn")
	float TurnEndTime;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = "Turn")
	bool bIsRoundResultDisplayed = false;
};
