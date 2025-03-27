#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "System/MultiBaseballGameState.h"
#include "MultiBaseballController.generated.h"

enum EMessage_Type : int;
class UMultiChatWidget;

UCLASS()
class SPARTA_HOMEWORK_08_API AMultiBaseballController : public APlayerController
{
	GENERATED_BODY()

protected:
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UMultiChatWidget> ChatWidgetClass;
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "UI")
	TObjectPtr<UMultiChatWidget> ChatWidget;

public:
	UPROPERTY(Replicated, VisibleAnywhere, BlueprintReadOnly)
	FString PlayerID;

	UPROPERTY(ReplicatedUsing=ReceiveLeftTurn, VisibleAnywhere, BlueprintReadOnly)
	int LeftChance;
	UPROPERTY(ReplicatedUsing=ReceiveWinCount, VisibleAnywhere, BlueprintReadOnly)
	int WinCount;

private:
	int MaxChance;

public:
	AMultiBaseballController();
	
	UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(const FString& SenderName, const FString& Message, EMessage_Type MessageType = EMessage_Type::Message);
	UFUNCTION(Server, Reliable)
	void ServerSendMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveGameReset();
	UFUNCTION(Server, Reliable)
	void ServerSendGameReset();

	UFUNCTION(Client, Reliable)
	void ClientReceiveSBO(const FString& SenderName, const FString& Answer, int StrikeNum, int BallNum, int OutNum);
	UFUNCTION(Client, Reliable)
	void ClientReceiveFinishResult(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientReceiveWhoTurn(const FString& Message);
	UFUNCTION(Client, Reliable)
	void ClientReceiveLeftTurn(const FString& ID, const int& InLeftChance);

	void SetPlayerID(const FString& ID) { PlayerID = ID; }
	FString GetPlayerID() { return PlayerID; }

	void PlayerWin() { WinCount++; }
	int GetWinCount() { return WinCount; }

	int GetLeftChance() { return LeftChance; }
	void SetLeftChance(int NewLeftChance) { LeftChance = NewLeftChance; }
	void ResetLeftChance() { LeftChance = MaxChance; }
	
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	void ReceiveLeftTurn();
	UFUNCTION()
	void ReceiveWinCount();
	UFUNCTION(Client, Reliable)
	void ClientReceiveOtherWinCount(const FString& ID, const int& OtherWinCount);
	UFUNCTION(Client, Reliable)
	void ClientReceiveTargetNum(const FString& TargetNum);

	UFUNCTION()
	void ClientReceiveTimerText();
	
protected:
	virtual void BeginPlay() override;
};
