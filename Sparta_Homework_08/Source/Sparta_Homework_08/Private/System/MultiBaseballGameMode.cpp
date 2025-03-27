#include "System/MultiBaseballGameMode.h"
#include "Player/MultiBaseballController.h"

AMultiBaseballGameMode::AMultiBaseballGameMode()
{
	
}

void AMultiBaseballGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (NewPlayer->IsLocalController())
	{
		Cast<AMultiBaseballController>(NewPlayer)->SetPlayerID(TEXT("Host"));
	}
	else
	{
		Cast<AMultiBaseballController>(NewPlayer)->SetPlayerID(TEXT("Guest"));
	}

	// PostLogin() 이후에 ID를 부여하면, 동작 시점이 안맞는거 같다.
	Super::PostLogin(NewPlayer);
}

void AMultiBaseballGameMode::BeginPlay()
{
	Super::BeginPlay();

	Cast<AMultiBaseballGameState>(GetWorld()->GetGameState())->SetPlayerTurnID(TEXT("Host"));
}

