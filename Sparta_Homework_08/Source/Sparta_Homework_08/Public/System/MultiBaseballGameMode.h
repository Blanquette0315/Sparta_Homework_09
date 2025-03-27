#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MultiBaseballGameMode.generated.h"

UCLASS()
class SPARTA_HOMEWORK_08_API AMultiBaseballGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
public:
	AMultiBaseballGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	
protected:
	virtual void BeginPlay() override;
};
