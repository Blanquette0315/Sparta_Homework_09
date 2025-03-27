#include "Player/MultiBaseballController.h"

#include "Blueprint/UserWidget.h"
#include "Net/UnrealNetwork.h"
#include "System/MultiBaseballGameState.h"
#include "UI/MultiChatWidget.h"

AMultiBaseballController::AMultiBaseballController()
	: MaxChance(3)
	, WinCount(0)
{
	LeftChance = MaxChance;
	bReplicates = true;
}

void AMultiBaseballController::ClientReceiveLeftTurn_Implementation(const FString& ID, const int& InLeftChance)
{
	if (ChatWidget)
	{
		if (ID == PlayerID)
		{
			ChatWidget->UpdateLeftTurn(LeftChance);
		}
		else
		{
			ChatWidget->UpdateLeftTurn_Other(InLeftChance);
		}
	}
}

void AMultiBaseballController::ClientReceiveGameReset_Implementation()
{
	if (ChatWidget)
	{
		ChatWidget->SetInit(PlayerID, MaxChance);
	}
}

void AMultiBaseballController::ServerSendGameReset_Implementation()
{
	TObjectPtr<AMultiBaseballGameState> ChatGameState = GetWorld()->GetGameState<AMultiBaseballGameState>();
	if (ChatGameState)
	{
		ChatGameState->BroadcastGameReset();
	}
}

void AMultiBaseballController::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiBaseballController, PlayerID);
	DOREPLIFETIME(AMultiBaseballController, LeftChance);
	DOREPLIFETIME(AMultiBaseballController, WinCount);
}

void AMultiBaseballController::ReceiveLeftTurn()
{
	if (ChatWidget)
	{
		ChatWidget->UpdateLeftTurn(LeftChance);
	}
}

void AMultiBaseballController::ReceiveWinCount()
{
	if (ChatWidget)
	{
		ChatWidget->UpdateWinCount(WinCount);
	}
}

void AMultiBaseballController::ClientReceiveOtherWinCount_Implementation(const FString& ID, const int& OtherWinCount)
{
	if (ChatWidget)
	{
		if (ID == PlayerID)
		{
			ChatWidget->UpdateWinCount(WinCount);
		}
		else
		{
			ChatWidget->UpdateWinCount_Other(OtherWinCount);
		}
	}
}

void AMultiBaseballController::ClientReceiveTargetNum_Implementation(const FString& TargetNum)
{
	if (ChatWidget)
	{
		ChatWidget->UpdateTargetText(TargetNum);
	}
}

void AMultiBaseballController::ClientReceiveTimerText()
{
	if (ChatWidget)
	{
		ChatWidget->UpdateTimerText();
	}
}

void AMultiBaseballController::BeginPlay()
{
	Super::BeginPlay();

	SetInputMode(FInputModeGameAndUI());
	bShowMouseCursor = true;

	if (IsLocalController())
	{
		ChatWidget = CreateWidget<UMultiChatWidget>(this, ChatWidgetClass);
		if (ChatWidget)
		{
			ChatWidget->AddToViewport();
			ChatWidget->SetInit(PlayerID, MaxChance, true);

			TObjectPtr<AMultiBaseballGameState> ChatGameState = GetWorld()->GetGameState<AMultiBaseballGameState>();
			if (ChatGameState)
			{
				ChatWidget->UpdateMainText(ChatGameState->PlayerTurnID);
			}
		}
	}
}

void AMultiBaseballController::ClientReceiveMessage_Implementation(const FString& SenderName, const FString& Message, EMessage_Type MessageType)
{
	if (ChatWidget)
	{
		if (EMessage_Type::SystemMessage == MessageType)
		{
			ChatWidget->UpdateSystemLog(Message);
		}
		else
		{
			ChatWidget->UpdateChat(Message);
		}
	}
}

void AMultiBaseballController::ServerSendMessage_Implementation(const FString& Message)
{
	TObjectPtr<AMultiBaseballGameState> ChatGameState = GetWorld()->GetGameState<AMultiBaseballGameState>();
	if (ChatGameState)
	{
		ChatGameState->CheckMessage(PlayerID, Message, this);
	}
}

void AMultiBaseballController::ClientReceiveSBO_Implementation(const FString& SenderName, const FString& Answer, int StrikeNum, int BallNum, int OutNum)
{
	if (ChatWidget)
	{
		if (PlayerID == SenderName)
		{
			ChatWidget->UpdateSBO(StrikeNum, BallNum, OutNum);
			ChatWidget->UpdateAnswerText(Answer);
		}
		else
		{
			ChatWidget->UpdateSBO_Other(StrikeNum, BallNum, OutNum);
			ChatWidget->UpdateAnswerText_Other(Answer);
		}
	}
}

void AMultiBaseballController::ClientReceiveFinishResult_Implementation(const FString& Message)
{
	if (ChatWidget)
	{
		ChatWidget->UpdateMainText(Message);
		ChatWidget->ShowResetButton();
	}
}

void AMultiBaseballController::ClientReceiveWhoTurn_Implementation(const FString& Message)
{
	if (ChatWidget)
	{
		ChatWidget->UpdateMainText(Message);
	}
}