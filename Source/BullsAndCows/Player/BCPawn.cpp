#include "BCPawn.h"
#include "BullsAndCows.h"

void ABCPawn::BeginPlay()
{
	Super::BeginPlay();

	FString NetRoleString = BullsAndCowsFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::BeginPlay() %s [%s]"), *BullsAndCowsFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BullsAndCowsFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}

void ABCPawn::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	FString NetRoleString = BullsAndCowsFunctionLibrary::GetRoleString(this);
	FString CombinedString = FString::Printf(TEXT("CXPawn::PossessedBy() %s [%s]"), *BullsAndCowsFunctionLibrary::GetNetModeString(this), *NetRoleString);
	BullsAndCowsFunctionLibrary::MyPrintString(this, CombinedString, 10.f);
}
