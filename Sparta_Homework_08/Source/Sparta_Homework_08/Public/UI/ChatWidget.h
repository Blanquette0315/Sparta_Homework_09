#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ChatWidget.generated.h"

class UVerticalBox;
class UEditableTextBox;

UCLASS()
class SPARTA_HOMEWORK_08_API UChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> ChatLogBox;

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION(Server, Reliable)
	void SendMessageToServer(const FString& Message);

	UFUNCTION()
	void UpdateChat(const FString& Message);
};
