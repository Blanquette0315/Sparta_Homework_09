#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BaseballGameMode.generated.h"

class UChatWidget;

UCLASS()
class SPARTA_HOMEWORK_08_API ABaseballGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	//UFUNCTION(NetMulticast, Reliable)
	void BroadcastMessage(const FString& UserID, const FString& Message);

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UChatWidget> ChatWidgetClass;

protected:
	virtual void BeginPlay() override;
};