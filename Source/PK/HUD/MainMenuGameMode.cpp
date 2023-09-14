// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "MainMenuPlayerController.h"
#include "MainMenuGameMode.h"
#include "Main/PKGameInstance.h"

AMainMenuGameMode::AMainMenuGameMode(const FObjectInitializer& ObjectInitializer)
: APKGameMode(ObjectInitializer)
{
	// TODO: get this from game instance
	static ConstructorHelpers::FObjectFinder<UClass>MainMenuWidget(TEXT("Class'/Game/HUD/Menu/MainMenu.MainMenu_C'"));
	StartingWidgetClass = (UClass*)MainMenuWidget.Object;	
	
	PlayerControllerClass = AMainMenuPlayerController::StaticClass();
}

void AMainMenuGameMode::StartPlay()
{
	Super::StartPlay();
}

void AMainMenuGameMode::BeginPlay()
{
	Super::BeginPlay();

	UWorld* World = GetWorld();
	if (World != NULL)
	{
		UPKGameInstance* GI = Cast<UPKGameInstance>(World->GetGameInstance());

		if (IsRunningDedicatedServer())	{			
			if (GI->CurrentMapsList.Num() == 0){
				for (auto item : GI->DMLevels){
					GI->CurrentMapsList.Add(item);
				}
			}
			
			TEnumAsByte<EMyEnum> Branches;
			GI->StartOnlineGame(Branches);
			switch (Branches){
			case EMyEnum::BranchA:			
				//MessageBox(NULL, _T("StartOnlineGameComplete"), _T(""), MB_OK | MB_APPLMODAL);
				break;
			case EMyEnum::BranchB:
				//MessageBox(NULL, _T("Faild to StartOnlineGame"), _T(""), MB_OK | MB_ICONERROR | MB_APPLMODAL);
				break;
			default:
				//MessageBox(NULL, _T("default:"), _T(""), MB_OK | MB_APPLMODAL);
				break;
			}
		}
		else {
			ChangeMenuWidget(StartingWidgetClass);
		}
	}
	else{
		
	}
}

void AMainMenuGameMode::SaveServerMaps(TArray<FString> Maps)
{
	ServerMaps = "";
	for (auto It = Maps.CreateConstIterator(); It; ++It)
	{
		ServerMaps += *It;
		ServerMaps += TEXT(",");
	}
	ServerMaps.RemoveFromEnd(",");
	SaveConfig();
	SetGameInstanceCurrentMapsList();
}

FString AMainMenuGameMode::GetServerMapsString()
{
	return ServerMaps;
}

void AMainMenuGameMode::SetGameInstanceCurrentMapsList()
{	
	ServerMaps.ParseIntoArray(&Cast<UPKGameInstance>(GetGameInstance())->CurrentMapsList, TEXT(","), true);
}