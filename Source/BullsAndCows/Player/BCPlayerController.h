#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BCPlayerController.generated.h"

class UBCChatInput;
class UBCNotificationWidget;

UCLASS()
class BULLSANDCOWS_API ABCPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	ABCPlayerController();

	virtual void BeginPlay() override;

	void SetChatMessageString(const FString& InChatMessageString);

	void PrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Client, Reliable)
	void ClientRPCPrintChatMessageString(const FString& InChatMessageString);

	UFUNCTION(Server, Reliable)
	void ServerRPCPrintChatMessageString(const FString& InChatMessageString);

	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

protected:
	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<UBCChatInput> ChatInputWidgetClass;

	UPROPERTY()
	TObjectPtr<UBCChatInput> ChatInputWidgetInstance;

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UBCNotificationWidget> NotificationTextWidgetClass;

	UPROPERTY()
	TObjectPtr<UBCNotificationWidget> NotificationTextWidgetInstance;

public:
	UPROPERTY(Replicated, BlueprintReadOnly)
	FText NotificationText;
};
