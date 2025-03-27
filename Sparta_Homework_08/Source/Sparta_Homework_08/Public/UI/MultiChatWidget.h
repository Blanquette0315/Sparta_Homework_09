#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MultiChatWidget.generated.h"

class UButton;
class UHorizontalBox;
class UTextBlock;
class UScrollBox;
class UVerticalBox;
class UEditableTextBox;

UCLASS()
class SPARTA_HOMEWORK_08_API UMultiChatWidget : public UUserWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	
public:
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UEditableTextBox> ChatInputBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> ChatLogBox;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UScrollBox> SystemLogBox;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> MainText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> ResetButton;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TargetText;


	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TimerText;
	
	
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnswerText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> StrikeBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BallBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> OutBox;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftTurnText;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WinCount;
	

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> AnswerText_Other;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> StrikeBox_Other;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> BallBox_Other;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UHorizontalBox> OutBox_Other;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> LeftTurnText_Other;
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> WinCount_Other;
	

	UFUNCTION()
	void OnTextCommitted(const FText& Text, ETextCommit::Type CommitMethod);

	UFUNCTION()
	void UpdateChat(const FString& Message);

	UFUNCTION()
	void UpdateSystemLog(const FString& Message);

	UFUNCTION()
	void UpdateMainText(const FString& Message);

	UFUNCTION()
	void ShowResetButton();
	
	UFUNCTION()
	void UpdateAnswerText(const FString& Message);
	UFUNCTION()
	void UpdateAnswerText_Other(const FString& Message);

	UFUNCTION()
	void UpdateSBO(const int& StrikeNum, const int& BallNum, const int& OutNum);
	UFUNCTION()
	void UpdateSBO_Other(const int& StrikeNum, const int& BallNum, const int& OutNum);

	void HideSBOImage();
	void HideSBOImage_Other();

	UFUNCTION()
	void UpdateLeftTurn(const int& LeftTurn);
	UFUNCTION()
	void UpdateLeftTurn_Other(const int& LeftTurn);

	UFUNCTION()
	void UpdateWinCount(const int& InWinCount);
	UFUNCTION()
	void UpdateWinCount_Other(const int& InWinCount);

	UFUNCTION()
	void OnResetButtonClicked();

	UFUNCTION()
	void UpdateTargetText(const FString& TargetNumber);

	UFUNCTION()
	void UpdateTimerText();
	
	UFUNCTION()
	void SetInit(const FString& ID, const int& LeftTurn, bool bCallBegin = false);
};
