#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "BCGameStateBase.generated.h"

UCLASS()
class BULLSANDCOWS_API ABCGameStateBase : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UFUNCTION(NetMulticast, Reliable)
	void MulticastRPCBroadcastLoginMessage(const FString& InNameString);
};
