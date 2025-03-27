#include "System/MultiBaseballGameState.h"
#include "Net/UnrealNetwork.h"
#include "Player/MultiBaseballController.h"


AMultiBaseballGameState::AMultiBaseballGameState()
	: TargetNumber("")
	, RemainingTime(60)
	, PlayerTurnID("None")
{
	
}

void AMultiBaseballGameState::BroadcastGameReset_Implementation()
{
	UE_LOG(LogTemp, Warning, TEXT("🎉 TargetNumber 초기 값: %d"), FCString::Atoi(*TargetNumber));
	if (HasAuthority())
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		StartTurnTimer();
		
		CreateRandomTargetNumber();

		for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
		{
			TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(*PlayerControllerIter);
			if (PlayerController)
			{
				PlayerController->ResetLeftChance();
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("🎉 TargetNumber 업데이트됨: %d"), FCString::Atoi(*TargetNumber));

	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveGameReset();
		}
	}

	if (HasAuthority())
	{
		PlayerTurnID = "Host";
		BroadcastWhoTurn(PlayerTurnID);
	}
}

void AMultiBaseballGameState::BroadcastLeftChance_Implementation(const FString& SenderName, const int& LeftChance)
{
	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
		{
			TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(*PlayerControllerIter);
			if (PlayerController)
			{
				PlayerController->ClientReceiveLeftTurn(SenderName, LeftChance);
			}
		}
	}
}

void AMultiBaseballGameState::BroadcastOtherWinCount_Implementation(const FString& SenderName, const int& OtherWinCount)
{
	if (HasAuthority())
	{
		for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
		{
			TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(*PlayerControllerIter);
			if (PlayerController)
			{
				PlayerController->ClientReceiveOtherWinCount(SenderName, OtherWinCount);
			}
		}
	}
}

void AMultiBaseballGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AMultiBaseballGameState, TargetNumber);
	DOREPLIFETIME(AMultiBaseballGameState, RemainingTime);
	DOREPLIFETIME(AMultiBaseballGameState, PlayerTurnID);
}

void AMultiBaseballGameState::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority())
	{
		CreateRandomTargetNumber();
		StartTurnTimer();
	}
	UE_LOG(LogTemp, Warning, TEXT("🎉 TargetNumber 업데이트됨: %d"), FCString::Atoi(*TargetNumber));
}

void AMultiBaseballGameState::CreateRandomTargetNumber()
{
	bool Memo[10] = { false };
	FString TargetNumberString = TEXT("");
    
	while (TargetNumberString.Len() < 3)
	{
		int32 RandomNumber = FMath::RandRange(1, 9);
		if (!Memo[RandomNumber])
		{
			TargetNumberString += FString::FromInt(RandomNumber);
			Memo[RandomNumber] = true;
		}
	}
	TargetNumber = TargetNumberString;
}

void AMultiBaseballGameState::CheckMessage_Implementation(const FString& SenderName, const FString& Message, APlayerController* SenderController)
{
	FString Answer = "";
	FString FullMessage = "";

	EMessage_Type MessageType = CheckMessageType(Message, Answer, SenderController);
	if (EMessage_Type::WrongAnswer == MessageType)
	{		
		FullMessage = FString::Printf(TEXT("[System]: %s"), *Answer);
		
		TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(SenderController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveMessage(SenderName, FullMessage, EMessage_Type::SystemMessage);
		}
	}
	else
	{
		if (EMessage_Type::Answer == MessageType)
		{
			GetWorldTimerManager().ClearTimer(TurnTimerHandle);
			BroadcastMessage(SenderName, Answer);
			if (CheckAnswer(SenderName, Answer)) // 정답을 맞췄을 떄
			{
				TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(SenderController);
				if (PlayerController)
				{
					PlayerController->PlayerWin();
					BroadcastOtherWinCount(SenderName, PlayerController->GetWinCount());
				}
				
				BroadcastFinishResult(SenderName, "Win!!!");
				return;
			}
			else // 아직 정답을 맞추지 못했을 때
			{
				// + 클라이언트 횟수 줄이기
				TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(SenderController);
				if (PlayerController)
				{
					int NewLeftChance = PlayerController->GetLeftChance() - 1;
					PlayerController->SetLeftChance(NewLeftChance);

					FString LeftChanceMessage = FString::Printf(TEXT("%s님의 남은 횟수 : %d"), *SenderName, NewLeftChance);
					BroadcastMessage("System", LeftChanceMessage, EMessage_Type::SystemMessage);

					// 남은 턴 UI 출력
					BroadcastLeftChance(SenderName, NewLeftChance);

					// 무승부인지 확인
					if (IsDraw())
					{
						BroadcastFinishResult("", "Draw!!!");
						return;
					}
				}
			}
			SwitchPlayerTurn();
			StartTurnTimer();
		}
		else
		{
			BroadcastMessage(SenderName, Message);
		}
	}
}

void AMultiBaseballGameState::BroadcastFinishResult_Implementation(const FString& SenderName, const FString& Message)
{
	FString FullMessage = FString::Printf(TEXT("%s %s"), *SenderName, *Message);

	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveFinishResult(FullMessage);
			// UI 띄우기
			PlayerController->ClientReceiveTargetNum(TargetNumber);
		}
	}
}

void AMultiBaseballGameState::BroadcastMessage_Implementation(const FString& SenderName, const FString& Message, EMessage_Type MessageType)
{
	FString FullMessage = FString::Printf(TEXT("[%s]: %s"), *SenderName, *Message);

	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveMessage(SenderName, FullMessage, MessageType);
		}
	}
}

EMessage_Type AMultiBaseballGameState::CheckMessageType(const FString& Message, FString& Answer, APlayerController* SenderController)
{
	Answer = TEXT("");
	if (!Message.IsEmpty())
	{
		if (Message[0] == '/')
		{
			if (TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(SenderController))
			{
				if (PlayerTurnID == PlayerController->GetPlayerID())
				{
					if (PlayerController->GetLeftChance() > 0)
					{
						if (Message.Len() == 4)
						{
							Answer = Message.Mid(1, 4);
							return EMessage_Type::Answer;
						}
						else
						{
							Answer = TEXT("잘못된 명령어 입니다.");
							return EMessage_Type::WrongAnswer;
						}
					}
					else
					{
						Answer = TEXT("플레이어의 남은 횟수가 부족합니다.");
						return EMessage_Type::WrongAnswer;
					}
				}
				else
				{
					Answer = TEXT("플레이어의 차례가 아닙니다.");
					return EMessage_Type::WrongAnswer;
				}
			}
		}
	}

	Answer = Message;
	return EMessage_Type::Message;
}

bool AMultiBaseballGameState::CheckAnswer(const FString& SenderName, const FString& Answer)
{
	bool Is_Correct = false;
	int StrikeNum = 0, BallNum = 0, OutNum = 0;
	
	for (int i = 0; i < Answer.Len(); ++i)
	{
		int FoundIndex;
		if (!TargetNumber.FindChar(Answer[i], FoundIndex))
		{
			OutNum++;
		}
		else
		{
			if (FoundIndex == i)
			{
				StrikeNum++;
			}
			else
			{
				BallNum++;
			}
		}
	}

	if (StrikeNum == 3)
	{
		Is_Correct = true;
	}
	
	FString Message = FString::Printf(TEXT("%s의 결과 [S : %d B : %d Out : %d]"), *SenderName, StrikeNum, BallNum, OutNum);
	BroadcastMessage("System", Message, EMessage_Type::SystemMessage);
	BroadcastSBOResult(SenderName, Answer, StrikeNum, BallNum, OutNum);
	return Is_Correct;
}

void AMultiBaseballGameState::BroadcastSBOResult_Implementation(const FString& SenderName, const FString& Answer, int StrikeNum, int BallNum, int OutNum)
{
	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveSBO(SenderName, Answer, StrikeNum, BallNum, OutNum);
		}
	}
}

bool AMultiBaseballGameState::IsDraw()
{
	bool Result = true;
	for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
	{
		TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(*PlayerControllerIter);
		if (PlayerController)
		{
			if (PlayerController->GetLeftChance() > 0)
			{
				Result = false;
				break;
			}
		}
	}

	return Result;
}

void AMultiBaseballGameState::StartTurnTimer()
{
	RemainingTime = 60; // 턴이 시작될 때 60초로 초기화
	GetWorldTimerManager().SetTimer(TurnTimerHandle, this, &AMultiBaseballGameState::UpdateTurnTimer, 1.0f, true);

	if (HasAuthority())
	{
		APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
		if (LocalController)
		{
			AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
			if (PlayerController)
			{
				PlayerController->ClientReceiveTimerText();
			}
		}
	}
}

void AMultiBaseballGameState::UpdateTurnTimer()
{
	if (RemainingTime > 0)
	{
		RemainingTime--;

		APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
		if (LocalController)
		{
			AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
			if (PlayerController)
			{
				PlayerController->ClientReceiveTimerText();
			}
		}
	}
	else
	{
		GetWorldTimerManager().ClearTimer(TurnTimerHandle);
		
		for (FConstPlayerControllerIterator PlayerControllerIter = GetWorld()->GetPlayerControllerIterator(); PlayerControllerIter; ++PlayerControllerIter)
		{
			TObjectPtr<AMultiBaseballController> PlayerController = Cast<AMultiBaseballController>(*PlayerControllerIter);
			if (PlayerController->GetPlayerID() == PlayerTurnID)
			{
				int NewLeftChance = PlayerController->GetLeftChance() - 1;
				PlayerController->SetLeftChance(NewLeftChance);

				FString LeftChanceMessage = FString::Printf(TEXT("%s님의 남은 횟수 : %d"), *PlayerTurnID, NewLeftChance);
				BroadcastMessage("System", LeftChanceMessage, EMessage_Type::SystemMessage);

				// 남은 턴 UI 출력
				BroadcastLeftChance(PlayerTurnID, NewLeftChance);
				
				// 무승부인지 확인
				if (IsDraw())
				{
					BroadcastFinishResult("", "Draw!!!");
					return;
				}
			}
		}

		SwitchPlayerTurn();
		StartTurnTimer();
	}
}

void AMultiBaseballGameState::OnRep_RemainTime()
{
	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveTimerText();
		}
	}
}

void AMultiBaseballGameState::SwitchPlayerTurn()
{
	if (PlayerTurnID == "Host")
	{
		PlayerTurnID = "Guest";
	}
	else
	{
		PlayerTurnID = "Host";
	}
	BroadcastWhoTurn(PlayerTurnID);
}

void AMultiBaseballGameState::BroadcastWhoTurn_Implementation(const FString& InPlayerTurnID)
{
	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ClientReceiveWhoTurn(InPlayerTurnID);
		}
	}
}