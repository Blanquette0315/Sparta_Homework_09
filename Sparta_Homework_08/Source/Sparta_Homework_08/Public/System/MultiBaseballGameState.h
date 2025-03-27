#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "MultiBaseballGameState.generated.h"

UENUM()
enum EMessage_Type
{
	Message,
	Answer,
	WrongAnswer,
	SystemMessage,
	End,
};

UCLASS()
class SPARTA_HOMEWORK_08_API AMultiBaseballGameState : public AGameStateBase
{
	GENERATED_BODY()
	
public:
	UPROPERTY(Replicated)
	FString TargetNumber;
	UPROPERTY(ReplicatedUsing=OnRep_RemainTime)
	int32 RemainingTime;
	UPROPERTY(Replicated, VisibleAnywhere)
	FString PlayerTurnID;
	
	AMultiBaseballGameState();
	
	UFUNCTION(NetMulticast,Reliable)
	void BroadcastMessage(const FString& SenderName, const FString& Message, EMessage_Type MessageType = EMessage_Type::Message);
	UFUNCTION(Server,Reliable)
	void CheckMessage(const FString& SenderName, const FString& Message, APlayerController* SenderController);

	UFUNCTION(NetMulticast,Reliable)
	void BroadcastSBOResult(const FString& SenderName, const FString& Answer, int StrikeNum, int BallNum, int OutNum);
	UFUNCTION(NetMulticast,Reliable)
	void BroadcastFinishResult(const FString& SenderName, const FString& Message);
	UFUNCTION(NetMulticast,Reliable)
	void BroadcastWhoTurn(const FString& InPlayerTurnID);
	UFUNCTION(NetMulticast,Reliable)
	void BroadcastGameReset();

	UFUNCTION(Server,Reliable)
	void BroadcastLeftChance(const FString& SenderName, const int& LeftChance);
	UFUNCTION(Server,Reliable)
	void BroadcastOtherWinCount(const FString& SenderName, const int& OtherWinCount);

	void SetPlayerTurnID(const FString& ID) { PlayerTurnID = ID; }
	
protected:
	virtual void BeginPlay() override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void CreateRandomTargetNumber();

	EMessage_Type CheckMessageType(const FString& Message, FString& Answer, APlayerController* SenderController);

	bool CheckAnswer(const FString& SenderName, const FString& Answer);

	bool IsDraw();

	void StartTurnTimer();
	void UpdateTurnTimer();
	
	UFUNCTION()
	void OnRep_RemainTime();
private:
	/** 실제 타이머 핸들 */
	FTimerHandle TurnTimerHandle;

	void SwitchPlayerTurn();
};
