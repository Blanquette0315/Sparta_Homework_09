#include "Player/BaseballController.h"
#include "Blueprint/UserWidget.h"
#include "GameFramework/GameStateBase.h"
#include "System/BaseballGameMode.h"
#include "UI/ChatWidget.h"

void ABaseballController::BeginPlay()
{
	Super::BeginPlay();
	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	if (!IsLocalController())
	{
		UE_LOG(LogTemp, Warning, TEXT("❌ 로컬 컨트롤러가 아님, UI 생성 안 함"));
		return;
	}
	
	FString AssignedID = HasAuthority() ? TEXT("Host") : TEXT("Guest");

	UE_LOG(LogTemp, Warning, TEXT("🎭 플레이어 ID 할당 (클라이언트에서 판별): %s"), *AssignedID);

	OnLoginWithID(AssignedID);

	ChatWidget = CreateWidget<UChatWidget>(this, ChatWidgetClass);
	if (ChatWidget)
	{
		ChatWidget->AddToViewport();
	}

	UE_LOG(LogTemp, Warning, TEXT("현재 플레이어 수: %d"), GetWorld()->GetGameState<AGameStateBase>()->PlayerArray.Num());
}

// 서버에서 메세지 처리 후 GameMode에 전달
void ABaseballController::ServerSendMessage_Implementation(const FString& Message)
{
	TObjectPtr<ABaseballGameMode> GameMode = GetWorld()->GetAuthGameMode<ABaseballGameMode>();
	if (GameMode)
	{
		GameMode->BroadcastMessage(PlayerID, Message);
	}
}

// 서버 RPC 검증 (필수)
bool ABaseballController::ServerSendMessage_Validate(const FString& Message)
{
	return !Message.IsEmpty();
}

// 클라이언트 메세지 수신
void ABaseballController::ClientReceiveMessage_Implementation(const FString& Message)
{
	UE_LOG(LogTemp, Display, TEXT("📥 클라이언트에서 메시지 수신: %s"), *Message);

	if (ChatWidget)
	{
		ChatWidget->UpdateChat(Message);
	}
}

// 서버에서 플레이어 로그인 처리
void ABaseballController::OnLoginWithID_Implementation(const FString& UserID)
{
	UE_LOG(LogTemp, Warning, TEXT("📩 서버에서 받은 UserID: %s"), *UserID);

	if (UserID.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("🚨 UserID가 비어 있음!"));
		return;
	}
	
	PlayerID = UserID;
}

bool ABaseballController::OnLoginWithID_Validate(const FString& UserID)
{
	return !UserID.IsEmpty();
}