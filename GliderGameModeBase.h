// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "GliderGameModeBase.generated.h"


class AGlider_Pawn;

UCLASS()
class GLIDER_API AGliderGameModeBase : public AGameModeBase
{
	GENERATED_BODY()

	public:
		/** Returns the total number of coins in the level*/
		UFUNCTION(BlueprintCallable) int GetTotalNumOfCoins();
		/** Returns the total number of coins collected by the player*/
		UFUNCTION(BlueprintCallable) int GetCoinsCollected();
		/** Increments the number of coins collected by the player*/
		UFUNCTION(BlueprintCallable) void IncrementCoinsCollected();

	private:
		AGlider_Pawn* PlayerPawn;
		APlayerController* Controller;
		FString LevelName;
		/** Total number of coins in the map*/
		UPROPERTY(EditAnywhere, Category = "Coins")	int TotalNumOfCoins = 30;
		/** The number of coins collected by the player*/
		UPROPERTY(EditAnywhere, Category = "Coins")	int CoinsCollected= 0;

		/** Reference to the main menu widget*/
		UPROPERTY(EditAnywhere, Category="Widgets")	TSubclassOf<UUserWidget> MainMenuWidget;
		/** Reference to the HUD widget*/
		UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> HUDWidget;
		/** Reference to the GameOver widget*/
		UPROPERTY(EditAnywhere, Category = "Widgets") TSubclassOf<UUserWidget> GameOverWidget;

		AGliderGameModeBase();

		virtual void BeginPlay();
		virtual void Tick(float DeltaTime);
		virtual void GameOver();
};
