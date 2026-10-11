#include "BCGameStateBase.h"
#include "Player/BCPlayerController.h"
#include "Kismet/GameplayStatics.h"

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
