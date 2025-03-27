#include "UI/ChatWidget.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/VerticalBox.h"
#include "Player/BaseballController.h"

void UChatWidget::NativeConstruct()
{
	Super::NativeConstruct();

	if (ChatInputBox)
	{
		ChatInputBox->OnTextCommitted.AddDynamic(this, &UChatWidget::OnTextCommitted);
	}
}

void UChatWidget::OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod)
{
	if (CommitMethod == ETextCommit::Type::OnEnter && !Text.IsEmpty())
	{
		ABaseballController* Controller = Cast<ABaseballController>(GetOwningPlayer());
		if (Controller)
		{
			UE_LOG(LogTemp, Warning, TEXT("📩 클라이언트에서 서버로 메시지 전송: %s"), *Text.ToString());
			Controller->ServerSendMessage(Text.ToString());  // ✅ 여기서 서버 RPC 호출!
		}

		ChatInputBox->SetText(FText::GetEmpty());
	}
}

void UChatWidget::UpdateChat(const FString& Message)
{
	UE_LOG(LogTemp, Log, TEXT("Sent Message: %s"), *Message);
	
	if (!ChatLogBox) return;

	UTextBlock* NewMessage = NewObject<UTextBlock>(this);
	if (NewMessage)
	{
		NewMessage->SetText(FText::FromString(Message));
		ChatLogBox->AddChild(NewMessage);
		UE_LOG(LogTemp, Display, TEXT("✅ UI에 메시지 추가: %s"), *Message);
	}
}

void UChatWidget::SendMessageToServer_Implementation(const FString& Message)
{
}