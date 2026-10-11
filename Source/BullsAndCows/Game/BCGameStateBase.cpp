#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "Player/BCPlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

void ABCGameStateBase::MulticastRPCBroadcastLoginMessage_Implementation(const FString& InNameString)
{
	if (HasAuthority() == false)
	{
		APlayerController* PC = UGameplayStatics::GetPlayerController(GetWorld(), 0);
		if (IsValid(PC) == true)
		{
			ABCPlayerController* BCPC = Cast<ABCPlayerController>(PC);
			if (IsValid(BCPC) == true)
			{
				FString NotificationString = InNameString + TEXT(" has joined the game.");
				BCPC->PrintChatMessageString(NotificationString);
			}
		}
	}
}

void ABCGameStateBase::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass, CurrentTurnPlayer);
	DOREPLIFETIME(ThisClass, TurnEndTime);
	DOREPLIFETIME(ABCGameStateBase, bIsRoundResultDisplayed);
}
