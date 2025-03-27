#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BaseballController.generated.h"


class UChatWidget;

UCLASS()
class SPARTA_HOMEWORK_08_API ABaseballController : public APlayerController
{
	GENERATED_BODY()

private:
	FString PlayerID;
	TObjectPtr<UChatWidget> ChatWidget;
	
public:
	UFUNCTION(Server, Reliable, WithValidation)
	void ServerSendMessage(const FString& Message);

	UFUNCTION(Client, Reliable)
	void ClientReceiveMessage(const FString& Message);

	UFUNCTION(Server, Reliable, WithValidation)
	void OnLoginWithID(const FString& UserID);

	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UChatWidget> ChatWidgetClass;

protected:
	virtual void BeginPlay() override;
};
