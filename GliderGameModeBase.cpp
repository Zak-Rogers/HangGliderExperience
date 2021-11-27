// Copyright Epic Games, Inc. All Rights Reserved.


#include "GliderGameModeBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/UserWidget.h"
#include "Glider_Pawn.h"


AGliderGameModeBase::AGliderGameModeBase()
{
	PrimaryActorTick.bStartWithTickEnabled = true;
	PrimaryActorTick.bCanEverTick = true;
}

void AGliderGameModeBase::BeginPlay()
{
	LevelName = UGameplayStatics::GetCurrentLevelName(GetWorld(), true);
	UUserWidget* Widget;
	PlayerPawn = Cast<AGlider_Pawn>(UGameplayStatics::GetPlayerPawn(GetWorld(), 0));
	Controller = UGameplayStatics::GetPlayerController(GetWorld(), 0);

	if (LevelName == "MainMenu")
	{
		Widget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), MainMenuWidget);
		Widget->AddToViewport();
	}

	if (LevelName == "Game")
	{
		Widget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), HUDWidget);
		Widget->AddToViewport();
		Controller->bShowMouseCursor = false;
	}

}

void AGliderGameModeBase::Tick(float DeltaTime)
{
	if ((PlayerPawn->GetCrashing() ||CoinsCollected == TotalNumOfCoins) && LevelName=="Game")
	{
		FTimerHandle GameOverTimerHandle;
		GetWorld()->GetTimerManager().SetTimer(GameOverTimerHandle, this, &AGliderGameModeBase::GameOver, 7.5f, false);
	}
}

void AGliderGameModeBase::GameOver()
{
	UUserWidget* Widget = CreateWidget<UUserWidget>(UGameplayStatics::GetPlayerController(GetWorld(), 0), GameOverWidget);
	Widget->AddToViewport();
	Controller->bShowMouseCursor = true;
}

int AGliderGameModeBase::GetTotalNumOfCoins()
{
	return TotalNumOfCoins;
}

int AGliderGameModeBase::GetCoinsCollected()
{
	return CoinsCollected;
}

void AGliderGameModeBase::IncrementCoinsCollected()
{
	CoinsCollected++;
}
