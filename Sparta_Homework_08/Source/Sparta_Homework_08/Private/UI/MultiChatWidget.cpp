#include "UI/MultiChatWidget.h"

#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include "Components/HorizontalBox.h"
#include "Components/ScrollBox.h"
#include "Components/TextBlock.h"
#include "Player/MultiBaseballController.h"
#include "UI/ChatWidget.h"

void UMultiChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UMultiChatWidget::OnTextCommitted);
	}

	if (ResetButton)
	{
		ResetButton->OnClicked.AddDynamic(this, &UMultiChatWidget::OnResetButtonClicked);
	}
}

void UMultiChatWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter && !Text.IsEmpty())
	{
		AMultiBaseballController* Controller = Cast<AMultiBaseballController>(GetOwningPlayer());
		if (Controller)
		{
			Controller->ServerSendMessage(Text.ToString());
		}

		ChatInputBox->SetText(FText::GetEmpty());
	}
}

void UMultiChatWidget::UpdateChat(const FString& Message)
{
	if (!ChatLogBox) return;

	UTextBlock* NewMessage = NewObject<UTextBlock>(this);
	if (NewMessage)
	{
		NewMessage->SetText(FText::FromString(Message));
		ChatLogBox->AddChild(NewMessage);
		ChatLogBox->ScrollToEnd();
	}
}

void UMultiChatWidget::UpdateSystemLog(const FString& Message)
{
	if (!SystemLogBox) return;

	UTextBlock* NewMessage = NewObject<UTextBlock>(this);
	if (NewMessage)
	{
		NewMessage->SetText(FText::FromString(Message));
		SystemLogBox->AddChild(NewMessage);
		SystemLogBox->ScrollToEnd();
	}
}

void UMultiChatWidget::UpdateMainText(const FString& Message)
{
	if (MainText)
	{
		MainText->SetText(FText::FromString(Message));
	}
}

void UMultiChatWidget::ShowResetButton()
{
	ResetButton->SetVisibility(ESlateVisibility::Visible);
}

void UMultiChatWidget::UpdateAnswerText(const FString& Message)
{
	if (AnswerText)
	{
		AnswerText->SetText(FText::FromString(Message));
	}
}

void UMultiChatWidget::UpdateAnswerText_Other(const FString& Message)
{
	if (AnswerText_Other)
	{
		AnswerText_Other->SetText(FText::FromString(Message));
	}
}

void UMultiChatWidget::UpdateSBO(const int& StrikeNum, const int& BallNum, const int& OutNum)
{
	HideSBOImage();

	for (int i = 1; i <= StrikeNum; i++)
	{
		StrikeBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
	for (int i = 1; i <= BallNum; i++)
	{
		BallBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
	for (int i = 1; i <= OutNum; i++)
	{
		OutBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMultiChatWidget::UpdateSBO_Other(const int& StrikeNum, const int& BallNum, const int& OutNum)
{
	HideSBOImage_Other();

	for (int i = 1; i <= StrikeNum; i++)
	{
		StrikeBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
	for (int i = 1; i <= BallNum; i++)
	{
		BallBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
	for (int i = 1; i <= OutNum; i++)
	{
		OutBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Visible);
	}
}

void UMultiChatWidget::HideSBOImage()
{
	for (int i = 1; i < StrikeBox_Other->GetChildrenCount(); i++)
	{
		StrikeBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
		BallBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
		OutBox->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMultiChatWidget::HideSBOImage_Other()
{
	for (int i = 1; i < StrikeBox_Other->GetChildrenCount(); i++)
	{
		StrikeBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
		BallBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
		OutBox_Other->GetChildAt(i)->SetVisibility(ESlateVisibility::Hidden);
	}
}

void UMultiChatWidget::UpdateLeftTurn(const int& LeftTurn)
{
	FString FullMessage = "";
	FullMessage = FString::Printf(TEXT("LeftTurn : %d"), LeftTurn);

	if (LeftTurnText)
	{
		LeftTurnText->SetText(FText::FromString(FullMessage));
	}
}

void UMultiChatWidget::UpdateLeftTurn_Other(const int& LeftTurn)
{
	FString FullMessage = "";
	FullMessage = FString::Printf(TEXT("LeftTurn : %d"), LeftTurn);

	if (LeftTurnText_Other)
	{
		LeftTurnText_Other->SetText(FText::FromString(FullMessage));
	}
}

void UMultiChatWidget::UpdateWinCount(const int& InWinCount)
{
	if (WinCount)
	{
		WinCount->SetText(FText::FromString(FString::Printf(TEXT("%d"), InWinCount)));
	}
}

void UMultiChatWidget::UpdateWinCount_Other(const int& InWinCount)
{
	if (WinCount_Other)
	{
		WinCount_Other->SetText(FText::FromString(FString::Printf(TEXT("%d"), InWinCount)));
	}
}

void UMultiChatWidget::OnResetButtonClicked()
{
	UE_LOG(LogTemp, Warning, TEXT("버튼이 클릭됨!"));

	APlayerController* LocalController = GetWorld()->GetFirstPlayerController();
	if (LocalController)
	{
		AMultiBaseballController* PlayerController = Cast<AMultiBaseballController>(LocalController);
		if (PlayerController)
		{
			PlayerController->ServerSendGameReset();
		}
	}
}

void UMultiChatWidget::UpdateTargetText(const FString& TargetNumber)
{
	if (TargetText)
	{
		TargetText->SetText(FText::FromString(TargetNumber));
	}
}

void UMultiChatWidget::UpdateTimerText()
{
	int Time = Cast<AMultiBaseballGameState>(GetWorld()->GetGameState())->RemainingTime;
	if (TimerText)
	{
		TimerText->SetText(FText::FromString(FString::Printf(TEXT("%d"), Time)));
	}
}

void UMultiChatWidget::SetInit(const FString& ID, const int& LeftTurn, bool bCallBegin)
{
	if (ChatInputBox)
	{
		ChatInputBox->SetText(FText::FromString(ID));
	}

	if (AnswerText)
	{
		AnswerText->SetText(FText::FromString(""));
	}

	if (AnswerText_Other)
	{
		AnswerText_Other->SetText(FText::FromString(""));
	}

	if (MainText)
	{
		MainText->SetText(FText::FromString(""));
	}

	if (ResetButton)
	{
		ResetButton->SetVisibility(ESlateVisibility::Hidden);
	}

	HideSBOImage();
	HideSBOImage_Other();
	
	if (LeftTurnText)
	{
		UpdateLeftTurn(LeftTurn);
	}

	if (LeftTurnText_Other)
	{
		UpdateLeftTurn_Other(LeftTurn);
	}

	if (bCallBegin)
	{
		if (WinCount)
		{
			WinCount->SetText(FText::FromString("0"));
		}

		if (WinCount_Other)
		{
			WinCount_Other->SetText(FText::FromString("0"));
		}
	}

	if (TargetText)
	{
		TargetText->SetText(FText::FromString("???"));
	}
}
