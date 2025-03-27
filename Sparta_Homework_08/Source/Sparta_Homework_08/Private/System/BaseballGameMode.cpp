#include "System/BaseballGameMode.h"
#include "GameFramework/GameStateBase.h"
#include "Player/BaseballController.h"

void ABaseballGameMode::BroadcastMessage(const FString& UserID, const FString& Message)
{
	FString FullMessage = FString::Printf(TEXT("[%s]: %s"), *UserID, *Message);
	UE_LOG(LogTemp, Warning, TEXT("📡 서버가 모든 클라이언트에게 메시지 전송: %s"), *FullMessage);
	
	for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
	{
		TObjectPtr<ABaseballController> PlayerController = Cast<ABaseballController>(*PlayerControllerIter);
		if (PlayerController)
		{
			PlayerController->ClientReceiveMessage(FullMessage);
		}
	}
}

void ABaseballGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
}

void ABaseballGameMode::BeginPlay()
{
	Super::BeginPlay();
	
}
