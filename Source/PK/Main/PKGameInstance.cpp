// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "HUD/JoinGameMenuUserWidget.h"
#include "PKClasses/PKGameViewportClient.h"
#include "PKClasses/HiddenCursorPlayerController.h"
#include "NetPlayer.h"
#include "PKPlayerState.h"
#include "PKGameInstance.h"
#include "PKGameSession.h"

#include "Runtime/Online/OnlineSubsystemNull/Private/OnlineSessionInterfaceNull.h"


UPKGameInstance::UPKGameInstance(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	/** Bind function for CREATING a Session */
	OnCreateSessionCompleteDelegate = FOnCreateSessionCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnCreateSessionComplete);
	OnStartSessionCompleteDelegate = FOnStartSessionCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnStartOnlineGameComplete);

	/** Bind function for FINDING a Session */
	OnFindSessionsCompleteDelegate = FOnFindSessionsCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnFindSessionsComplete);

	/** Bind function for JOINING a Session */
	OnJoinSessionCompleteDelegate = FOnJoinSessionCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnJoinSessionComplete);

	/** Bind function for DESTROYING a Session */
	OnDestroySessionCompleteDelegate = FOnDestroySessionCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnDestroySessionComplete);

	/** Bind function for ENDING a Session */
	OnEndSessionCompleteDelegate = FOnEndSessionCompleteDelegate::CreateUObject(this, &UPKGameInstance::OnEndSessionComplete);
	
	OpenLevelTimerDelegate.BindUFunction(this, FName("OpenLevel"));
	ClientTravelTimerDelegate.BindUFunction(this, FName("ClientTravel"));
	SetFocusTimerDelegate.BindUFunction(this, FName("SetFocus"));

	static ConstructorHelpers::FObjectFinder<UClass>DummyWidget(TEXT("Class'/Game/HUD/Menu/DummyWidget.DummyWidget_C'"));
	DummyWidgetClass = (UClass*)DummyWidget.Object;
	
	static ConstructorHelpers::FObjectFinder<UClass>MainMenuWidget(TEXT("Class'/Game/HUD/Menu/MainMenu.MainMenu_C'"));
	MainMenuWidgetClass = (UClass*)MainMenuWidget.Object;

	static ConstructorHelpers::FObjectFinder<UClass>MPMenuWidget(TEXT("Class'/Game/HUD/Menu/Multiplayer/MultiplayerMenu.MultiplayerMenu_C'"));
	MultiplayerMenuWidgetClass = (UClass*)MPMenuWidget.Object;

	static ConstructorHelpers::FObjectFinder<UClass>ThrobberWidget(TEXT("Class'/Game/HUD/Menu/Multiplayer/Throbber.Throbber_C'"));
	ThrobberWidgetClass = (UClass*)ThrobberWidget.Object;
	

	if (!IsRunningDedicatedServer()){
		// workaround sounds caching
		class USoundAttenuation* AttenuationSettings = CreateDefaultSubobject<USoundAttenuation>(TEXT("DefaultSoundAttenuation"));
		AttenuationSettings->Attenuation.DistanceAlgorithm = ESoundDistanceModel::ATTENUATION_NaturalSound;
		AttenuationSettings->Attenuation.dBAttenuationAtMax = -45.f;
		AttenuationSettings->Attenuation.OmniRadius = 200.f; //100
		AttenuationSettings->Attenuation.FalloffDistance = 6000.f;

		CACHE.Init(AttenuationSettings);
	}
}

void UPKGameInstance::Init()
{

	Super::Init();

	FindMaps(DMLevels, "DM_");
	FindMaps(CTFLevels, "CTF_");
	FindMaps(PCFLevels, "PCF_");

	RestoreCurrentMaps(CurrentMapsList);

	FString ProjectVersion;
	GConfig->GetString(
		TEXT("/Script/EngineSettings.GeneralProjectSettings"),
		TEXT("ProjectVersion"),
		ProjectVersion,
		GGameIni
		);

	ServerName = ServerName.Append("(" + ProjectVersion + ")");
	
	// loading screen stuff
	FCoreUObjectDelegates::PreLoadMap.AddUObject(this, &UPKGameInstance::BeginLoadingScreen);
	FCoreUObjectDelegates::PostLoadMap.AddUObject(this, &UPKGameInstance::EndLoadingScreen);
}

void UPKGameInstance::BeginLoadingScreen()
{
	if (!IsRunningDedicatedServer() /*&& !WITH_EDITOR*/)
	{	
//#if PLATFORM_WINDOWS
		FLoadingScreenAttributes LoadingScreen;
		LoadingScreen.bAutoCompleteWhenLoadingCompletes = false;
		LoadingScreen.WidgetLoadingScreen = FLoadingScreenAttributes::NewTestLoadingScreenWidget();
		GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
//#endif
		ShowWidgetClassOf(nullptr);
	}
}

void UPKGameInstance::EndLoadingScreen()
{
	if (!IsRunningDedicatedServer()){
		const UWorld* World = GetWorld();
		if (World)
		{
			UPKGameViewportClient* GameViewportClient = Cast<UPKGameViewportClient>(World->GetGameViewport());
			if (GameViewportClient)
			{
				GameViewportClient->Fade(0.5f, false, 2.0f);
			}
		}
	}
}

// Creating a Session

bool UPKGameInstance::HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers; // real value will be set later in UpdateSessionSettings
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;
			
			SessionSettings->Set(SETTING_SERVERNAME, ServerName, EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_MAPNAME, LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_NUMPLAYERS, 0, EOnlineDataAdvertisementType::ViaOnlineService);
			
			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			return Sessions->CreateSession(*UserId, SessionName, *SessionSettings);
		}
	}

	return false;
}

bool UPKGameInstance::HostSession(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			SessionSettings = MakeShareable(new FOnlineSessionSettings());

			SessionSettings->bIsLANMatch = bIsLAN;
			SessionSettings->bUsesPresence = bIsPresence;
			SessionSettings->NumPublicConnections = MaxNumPlayers;
			SessionSettings->NumPrivateConnections = 0;
			SessionSettings->bAllowInvites = true;
			SessionSettings->bAllowJoinInProgress = true;
			SessionSettings->bShouldAdvertise = true;
			SessionSettings->bAllowJoinViaPresence = true;
			SessionSettings->bAllowJoinViaPresenceFriendsOnly = false;

			SessionSettings->Set(SETTING_SERVERNAME, ServerName, EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_MAPNAME, LevelName.ToString(), EOnlineDataAdvertisementType::ViaOnlineService);
			SessionSettings->Set(SETTING_NUMPLAYERS, 0, EOnlineDataAdvertisementType::ViaOnlineService);

			OnCreateSessionCompleteDelegateHandle = Sessions->AddOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegate);
			return Sessions->CreateSession(0, SessionName, *SessionSettings);
		}
	}

	return false;
}

void UPKGameInstance::OnCreateSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnCreateSessionCompleteDelegate_Handle(OnCreateSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				OnStartSessionCompleteDelegateHandle = Sessions->AddOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegate);
				Sessions->StartSession(SessionName);
			}
		}

	}
}

void UPKGameInstance::OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnStartSessionCompleteDelegate_Handle(OnStartSessionCompleteDelegateHandle);
		}
	}

	if (bWasSuccessful)
	{
		if (!IsRunningDedicatedServer()){
			GetFirstLocalPlayerController()->bShowMouseCursor = false;
			ShowWidgetClassOf(nullptr);
		}		
		GetWorld()->GetTimerManager().SetTimerForNextTick(OpenLevelTimerDelegate);
	}
	
	bStartOnlineGameComplete = bWasSuccessful;
}

// Searching and Finding a Session

void UPKGameInstance::FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			SessionSearch = MakeShareable(new FOnlineSessionSearch());

			SessionSearch->bIsLanQuery = bIsLAN;
			SessionSearch->MaxSearchResults = 20;
			SessionSearch->PingBucketSize = 50;
			
			if (bIsPresence)
			{
				SessionSearch->QuerySettings.Set(SEARCH_PRESENCE, bIsPresence, EOnlineComparisonOp::Equals);
			}

			TSharedRef<FOnlineSessionSearch> SearchSettingsRef = SessionSearch.ToSharedRef();

			OnFindSessionsCompleteDelegateHandle = Sessions->AddOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegate);
			bOnFindSessionsCompleteDelegateHandle = true;
			Sessions->FindSessions(*UserId, SearchSettingsRef);
		}
	}
	else
	{
		OnFindSessionsComplete(false);
	}
}

void UPKGameInstance::OnFindSessionsComplete(bool bWasSuccessful)
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		SearchResultsArr.Empty();
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();
		if (Sessions.IsValid())
		{
			Sessions->ClearOnFindSessionsCompleteDelegate_Handle(OnFindSessionsCompleteDelegateHandle);
			bOnFindSessionsCompleteDelegateHandle = false;

			if (SessionSearch.IsValid() && SessionSearch->SearchResults.Num() > 0)
			{
				for (int32 SearchIdx = 0; SearchIdx < SessionSearch->SearchResults.Num(); SearchIdx++)
				{										
					FOnlineSession session = SessionSearch->SearchResults[SearchIdx].Session;
					
					const FOnlineSessionInfoNull* SearchSessionInfo = (const FOnlineSessionInfoNull*)session.SessionInfo.Get();
					FString IP = SearchSessionInfo->HostAddr.Get()->ToString(true);
					
					FString SessionName = session.OwningUserName;
					FString ServerName; 
					session.SessionSettings.Get(SETTING_SERVERNAME, ServerName);
					FString MapName;
					session.SessionSettings.Get(SETTING_MAPNAME, MapName);
					int32 NumPlayers;
					session.SessionSettings.Get(SETTING_NUMPLAYERS, NumPlayers);
					int32 MaxNumPlayers = session.SessionSettings.NumPublicConnections;
					FString GameType = "FFA";
					int32 PingInMs = SessionSearch->SearchResults[SearchIdx].PingInMs;
					int32 Ping = PingInMs == -1 ? 0 : PingInMs * 2;
					FString SearchResult = FString::Printf(TEXT("%s|%s|%s|%d/%d|%s|%d|%s"),
						*SessionName, // session to join to

						// the followings are for display in widget
						*ServerName,
						*MapName,
						NumPlayers,
						MaxNumPlayers,
						*GameType,
						Ping,
						*IP
						);
					
					SearchResultsArr.Add(SearchResult);
				}
				UpdateJoinGameMenuWidget(1); return;
			}
		}
	}
	UpdateJoinGameMenuWidget(0);
}

// Joining a Session

bool UPKGameInstance::JoinSession(ULocalPlayer* LocalPlayer, /*FName SessionName,*/ const FOnlineSessionSearchResult& SearchResult)
{
	bool bSuccessful = false;

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();

	TSharedPtr<const FUniqueNetId> UserId = LocalPlayer->GetPreferredUniqueNetId();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid() && UserId.IsValid())
		{
			OnJoinSessionCompleteDelegateHandle = Sessions->AddOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegate);

			bSuccessful = Sessions->JoinSession(*UserId, GameSessionName, SearchResult);
		}
	}
	
	return bSuccessful;
}

void UPKGameInstance::OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result)
{
	if (static_cast<int32>(Result) > 0)
	{
		FString msg;

		switch (Result){
		case EOnJoinSessionCompleteResult::UnknownError:
			msg = L"UnknownError!";
			break;
		case EOnJoinSessionCompleteResult::AlreadyInSession:
			msg = L"AlreadyInSession!";
			break;
		case EOnJoinSessionCompleteResult::CouldNotRetrieveAddress:
			msg = L"CouldNotRetrieveAddress!";
			break;
		case EOnJoinSessionCompleteResult::SessionDoesNotExist:
			msg = L"SessionDoesNotExist!";
			break;
		case EOnJoinSessionCompleteResult::SessionIsFull:
			msg = L"SessionIsFull!";
			break;
		default:
			break;
		}
		GEngine->BroadcastTravelFailure(WorldContext->World(), ETravelFailure::TravelFailure, msg);
		
		return;
	}

	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnJoinSessionCompleteDelegate_Handle(OnJoinSessionCompleteDelegateHandle);

			APlayerController * const PlayerController = GetFirstLocalPlayerController();

			FString TravelURL;
			
			if (PlayerController && Sessions->GetResolvedConnectString(SessionName, TravelURL))
			{
				// WORKAROUND: to hide mouse when re-join
				_PlayerController = PlayerController;
				_TravelURL = TravelURL;
				GetWorld()->GetTimerManager().SetTimerForNextTick(ClientTravelTimerDelegate);
			}
			else GEngine->BroadcastTravelFailure(WorldContext->World(), ETravelFailure::TravelFailure, L"Unresolved Connect String!");
		}
	}
}

void UPKGameInstance::ClientTravel()
{
	_PlayerController->ClientTravel(_TravelURL, ETravelType::TRAVEL_Relative);
}

// Destroying a Session

void UPKGameInstance::OnDestroySessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->ClearOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
				bStartOnlineGameComplete = false;
				if (bJoinGame && SessionSearch.IsValid()){
					JoinOnlineGame();
				}
				else if (bStartNewGame){
					TEnumAsByte<EMyEnum> Branches;
					StartOnlineGame(Branches);
				}
				else{
					GoToMainMenu();
				}
			}
		}
	}
}

// Ending a Session

void UPKGameInstance::OnEndSessionComplete(FName SessionName, bool bWasSuccessful)
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid()){
			Sessions->ClearOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegateHandle);
			if (bWasSuccessful)
			{
			}
		}
	}
}

void UPKGameInstance::OnStartButtonClick()
{
	if (CurrentMapsList.Num() == 0) return;
	APlayerController * const PlayerController = GetFirstLocalPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	ShowWidgetClassOf(nullptr);

	TEnumAsByte<EMyEnum> Branches;
	ESessionTypeEnum SessionType = GetSessionType();

	switch (SessionType){
	case ESessionTypeEnum::Host:
		MapIdx = 0;
		ServerTravel(CurrentMapsList[0]);
		break;
	case ESessionTypeEnum::Client:
		bStartNewGame = true;
		LeaveGame();
		break;
	case ESessionTypeEnum::None:
		StartOnlineGame(Branches);
		break;
	}
}
/************************************
     BLUEPRINTCALLABLE FUNCTIONS
************************************/
// Creating a Session
void UPKGameInstance::StartOnlineGame(TEnumAsByte<EMyEnum>& Branches)
{
	bStartNewGame = false;
	MapIdx = 0;
	FString MapName = CurrentMapsList[MapIdx];
	// Init 'LevelName' first, since 'MakeSureMapNameIsValid' overrides 'MapName'
	LevelName = FName(*MapName);

	if (!GEngine->MakeSureMapNameIsValid(MapName)) return;

	if (!IsRunningDedicatedServer()){
		ULocalPlayer* const Player = GetFirstGamePlayer();
		HostSession(Player->GetPreferredUniqueNetId(), GameSessionName, true, true, 16);
	}
	else{
		int32 MaxNumPlayers = Cast<APKGameSession>(GetWorld()->GetAuthGameMode()->GameSession)->MaxPlayers;
		HostSession(GameSessionName, true, true, MaxNumPlayers);
	}

	if (bStartOnlineGameComplete)
	{
		Branches = EMyEnum::BranchA;
	}
	else
	{
		Branches = EMyEnum::BranchB;
	}
}
// Searching and Finding a Session
void UPKGameInstance::FindOnlineGames()
{
	if (bOnFindSessionsCompleteDelegateHandle) return;
	ULocalPlayer* const Player = GetFirstGamePlayer();

	FindSessions(Player->GetPreferredUniqueNetId(), true, true);
}

// Joining a Session
void UPKGameInstance::OnJoinButtonClick(int32 SearchIdx, FString ServerIP)
{
	// TODO: break join process on ESC key
	if ( (SessionSearch->SearchResults.Num() == 0 && SearchIdx != -1)
		|| SessionSearch->SearchResults.Num() < SearchIdx+1) return;

	_SearchIdx = SearchIdx;
	_ServerIP = ServerIP;

	APlayerController * const PlayerController = GetFirstLocalPlayerController();
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	ShowWidgetClassOf(nullptr);

	ESessionTypeEnum SessionType = GetSessionType();

	switch (SessionType){
	case ESessionTypeEnum::Host:
		if (SearchIdx == -1) { // avoid ourself connection by IP
			break;
		}
		bJoinGame = true;
		DestroySessionAndLeaveGame();
		break;
	case ESessionTypeEnum::Client:
		bJoinGame = true;
		DestroySessionAndLeaveGame();
		break;
	case ESessionTypeEnum::None:
		JoinOnlineGame();
		break;
	}
}

void UPKGameInstance::SetFocus()
{
	if (WidgetToFocus)
	{
		FSlateApplication::Get().SetUserFocus(GetFirstGamePlayer()->GetControllerId(), WidgetToFocus->GetCachedWidget(), EFocusCause::SetDirectly);

		TArray<UUserWidget*> Widgets;
		UTIL.FindAllObjectsClassOf(Widgets);
		for (auto widget : Widgets){
			if (widget->IsA(WidgetToFocus->GetClass()))
			{
				GetWorld()->GetTimerManager().SetTimerForNextTick(SetFocusTimerDelegate);
			}
		}
	}
}

void UPKGameInstance::JoinOnlineGame()
{
	bJoinGame = false;

	ULocalPlayer* const Player = GetFirstGamePlayer();
	FOnlineSessionSearchResult SearchResult;

	WidgetToFocus = UTIL.ShowWidgetClassOf(this, ThrobberWidgetClass);
	GetWorld()->GetTimerManager().SetTimerForNextTick(SetFocusTimerDelegate);

	if (SessionSearch->SearchResults.Num() > 0 && SessionSearch->SearchResults.Num()> _SearchIdx && _SearchIdx > -1 && _SearchIdx < 19)
	{
		if (SessionSearch->SearchResults[_SearchIdx].Session.OwningUserId != Player->GetPreferredUniqueNetId())
		{
			SearchResult = SessionSearch->SearchResults[_SearchIdx];
			/*bool b = */JoinSession(Player, /*GameSessionName,*/ SearchResult);
		}
	}
	else if (_SearchIdx == -1){ // workaround
		APlayerController * const PlayerController = GetFirstLocalPlayerController();
		_PlayerController = PlayerController;
		_TravelURL = _ServerIP;
		GetWorld()->GetTimerManager().SetTimerForNextTick(ClientTravelTimerDelegate);
	}
}
// Destroying a Session
void UPKGameInstance::DestroySessionAndLeaveGame()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			OnDestroySessionCompleteDelegateHandle = Sessions->AddOnDestroySessionCompleteDelegate_Handle(OnDestroySessionCompleteDelegate);

			Sessions->DestroySession(GameSessionName);
		}
	}
}

void UPKGameInstance::EndSession()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			OnEndSessionCompleteDelegateHandle = Sessions->AddOnEndSessionCompleteDelegate_Handle(OnEndSessionCompleteDelegate);

			Sessions->EndSession(GameSessionName);
		}
	}
}

// Utils
TArray<FString> UPKGameInstance::GetSearchResultArr()
{
	return SearchResultsArr;
}

void UPKGameInstance::UpdateJoinGameMenuWidget(bool bSuccess)
{
	TArray<UJoinGameMenuUserWidget*> Widgets;
	UTIL.FindAllObjectsClassOf(Widgets);
	if (Widgets.Num() > 0){
		if (bSuccess)
			Cast<UJoinGameMenuUserWidget>(Widgets[0])->_UpdateServerList(SearchResultsArr);
		else
		{
			TArray<FString> DummySearchResultsArr = {};
			Cast<UJoinGameMenuUserWidget>(Widgets[0])->_UpdateServerList(DummySearchResultsArr);
		}
	}
}

void UPKGameInstance::OnSearchResultsCompleted()
{	
	TArray<UUserWidget*> Widgets;
	UTIL.FindAllObjectsClassOf(Widgets);
	for (UUserWidget* widget : Widgets){
		if (widget->IsA(UJoinGameMenuUserWidget::StaticClass())){
			Cast<UJoinGameMenuUserWidget>(widget)->UpdateServerList(); //!!!!!!!!!!!!!
		}
	}
}

void UPKGameInstance::OnJoinSessionCompleted()
{
	TArray<UUserWidget*> Widgets;
	UTIL.FindAllObjectsClassOf(Widgets);
	for (UUserWidget* widget : Widgets){
		if (widget->IsA(UJoinGameMenuUserWidget::StaticClass())){
			Cast<UJoinGameMenuUserWidget>(widget)->JoinSessionCompleted();
		}
	}
}

void UPKGameInstance::FindMaps(TArray<FString> &Levels, FString MapType){
	
	TArray<FString> MapNames;

	IFileManager::Get().FindFilesRecursive(MapNames, *FPaths::GameContentDir(), TEXT("*.umap"), true, false);

	for (auto name : MapNames)
	{
		FURL Url(*name);
		if (Url.ToString().StartsWith(MapType))
			Levels.AddUnique(Url.ToString());
	}
	
	Levels.Sort();
}

void UPKGameInstance::LoadNextMap()
{
	if (CurrentMapsList.Num()>0)
	{
		MapIdx < CurrentMapsList.Num() - 1 ? MapIdx++ : MapIdx = 0;
		ServerTravel(CurrentMapsList[MapIdx]);
	}
}

void UPKGameInstance::ServerTravel(FString NewMapName)
{
	if (!GEngine->MakeSureMapNameIsValid(NewMapName)) return;

	UWorld* World = GEngine->GetWorldFromContextObject(this);
	FWorldContext &WorldContext = GEngine->GetWorldContextFromWorldChecked(World);

	// Destroying all the pawns before traveling.
	// NOTE:
	// After the current pawn is destroyed, PlayerController can respawn a new one only in the next frame.
	// This avoids spawning since the match will end at this point.
	TArray<APlayerState*> PlayerArray = World->GameState->PlayerArray;
	for (int i = 0; i < PlayerArray.Num(); i++){
		APlayerController* PC = Cast<APlayerController>(PlayerArray[i]->GetNetOwningPlayer()->PlayerController);
		if (PC->AcknowledgedPawn) PC->AcknowledgedPawn->Destroy();
	}

	/* NOTE:
	* TRAVEL_Absolute gives URL like "/Game/FirstPerson/Maps/DM_Sacred"
	* TRAVEL_Relative gives URL like "/Game/FirstPerson/Maps/DM_Sacred?game='/Script/PK.FFAGameMode'?listen"
	*
	* TRAVEL_Relative fails to travel when 7777 port is occupied by another server and we are on 7778...9...etc
	*/
	FURL TestURL(&WorldContext.LastURL, *NewMapName, TRAVEL_Absolute);
	World->ServerTravel(TestURL.ToString());
}

void UPKGameInstance::UpdateSessionSettings()
{
	IOnlineSubsystem* const OnlineSub = IOnlineSubsystem::Get();

	if (OnlineSub)
	{
		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			Sessions->GetSessionSettings(GameSessionName)->Set(SETTING_MAPNAME, GetWorld()->GetName(), EOnlineDataAdvertisementType::ViaOnlineService);
			Sessions->GetSessionSettings(GameSessionName)->Set(SETTING_NUMPLAYERS, GetWorld()->GetAuthGameMode()->NumPlayers, EOnlineDataAdvertisementType::ViaOnlineService);
			Sessions->GetSessionSettings(GameSessionName)->NumPublicConnections = GetWorld()->GetAuthGameMode()->GameSession->MaxPlayers;
		}
	}
}

void UPKGameInstance::ShowWidgetClassOf(TSubclassOf<class UUserWidget> WidgetClass)
{
	UTIL.ShowWidgetClassOf(this, WidgetClass);
	WidgetToFocus = nullptr;
}

void UPKGameInstance::OnEscape()
{
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	PlayerController->bShowMouseCursor = true;
	ShowWidgetClassOf(MainMenuWidgetClass);
	PlayerController->SetInputMode(FInputModeUIOnly());
	CenterCursor();
}

void UPKGameInstance::OnResume()
{
	// TODO: resume if not main menu
	APlayerController * const PlayerController = GetFirstLocalPlayerController();
	ShowWidgetClassOf(nullptr);
	PlayerController->SetInputMode(FInputModeGameOnly());
	PlayerController->bShowMouseCursor = false;
}

void UPKGameInstance::OnDisconnect()
{
	APlayerController * const PlayerController = GetFirstLocalPlayerController();	
	PlayerController->bShowMouseCursor = false;
	PlayerController->SetInputMode(FInputModeGameOnly());
	ShowWidgetClassOf(nullptr);

	ESessionTypeEnum SessionType = GetSessionType();

	switch (SessionType){
	case ESessionTypeEnum::None:
		GoToMainMenu();
		break;
	default:
		LeaveGame();
		break;
	}	
}

void UPKGameInstance::GoToMainMenu()
{
	UWorld* World = GEngine->GetWorldFromContextObject(this);

	FString Option = "game=Class'/Script/PK.MainMenuGameMode'";
	UGameplayStatics::OpenLevel(World, "DM_Sacred", true, Option);

	CenterCursor();
}

void UPKGameInstance::CenterCursor()
{
	//Viewport Size
	const FVector2D ViewportSize = FVector2D(GEngine->GameViewport->Viewport->GetSizeXY());
	//Viewport Center!            
	const FVector2D ViewportCenter = FVector2D(ViewportSize.X / 2, ViewportSize.Y / 2);

	GEngine->GameViewport->Viewport->SetMouse(ViewportCenter.X, ViewportCenter.Y);
}

bool UPKGameInstance::CancelConnection()
{
	if (WorldContext->PendingNetGame)
	{
		UNetDriver* NetDriver = WorldContext->PendingNetGame->NetDriver;
		if (NetDriver)
		{
			GEngine->BroadcastNetworkFailure(WorldContext->World(), NetDriver, ENetworkFailure::ConnectionTimeout);
			return true;
		}
	}
	return false;
}

void UPKGameInstance::OpenLevel()
{
	FString Option = "game=Class'/Script/PK.FFAGameMode'?listen";
	UGameplayStatics::OpenLevel(GetWorld(), LevelName, true, Option);
}

ESessionTypeEnum UPKGameInstance::GetSessionType()
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	
	if (OnlineSub)
	{
		if (OnlineSub->IsDedicated())
		{
			return ESessionTypeEnum::Host;
		}

		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface();

		if (Sessions.IsValid())
		{
			IOnlineIdentityPtr OnlineIdentityInt = Online::GetIdentityInterface();

			if (!OnlineIdentityInt.IsValid())
			{
				return ESessionTypeEnum::None;
			}
			
			FNamedOnlineSession* Session = Sessions.Get()->GetNamedSession(GameSessionName);
						
			if (Session)
			{
				TSharedPtr<FUniqueNetId> UserId = OnlineIdentityInt->GetUniquePlayerId(Session->HostingPlayerNum);

				if ((UserId.IsValid() && (*UserId == *Session->OwningUserId)))
					return ESessionTypeEnum::Host;
				else
					return ESessionTypeEnum::Client;
			}
		}
	}
	
	return ESessionTypeEnum::None;
}

bool UPKGameInstance::IsInSession() // IsHost
{
	IOnlineSubsystem* OnlineSub = IOnlineSubsystem::Get();
	if (OnlineSub)
	{
		if(OnlineSub->IsDedicated())
		{
			return true;
		}

		IOnlineSessionPtr Sessions = OnlineSub->GetSessionInterface(); 

		if (Sessions.IsValid())
		{
			ULocalPlayer* const Player = GetFirstGamePlayer();
			TSharedPtr<const FUniqueNetId> UserId = Player->GetPreferredUniqueNetId();
			return Sessions->IsPlayerInSession(GameSessionName, *UserId);
		}
	}
	return false;
}

void UPKGameInstance::LeaveGame()
{
	SessionSearch.Reset();
	DestroySessionAndLeaveGame();
}

void UPKGameInstance::UpdatePlayerNickName(FString NewName)
{
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	if (PlayerController)
	{
		Cast<APKPlayerState>(PlayerController->PlayerState)->UpdatePlayerNickName(NewName);
	}
}

FString UPKGameInstance::GetPlayerNickName()
{
	APlayerController * const PlayerController = GetFirstLocalPlayerController();

	return PlayerController ? PlayerController->PlayerState->GetHumanReadableName() : FString("NoName");
}

void UPKGameInstance::StoreCurrentMaps(TArray<FString> ServerMaps)
{
	CurrentMapsList.Empty();
	for (auto item : ServerMaps){
		CurrentMapsList.Add(item);
	}

	FString CfgServerMaps;
	for (auto It = CurrentMapsList.CreateConstIterator(); It; ++It)
	{
		CfgServerMaps += *It;
		CfgServerMaps += TEXT(",");
	}
	CfgServerMaps.RemoveFromEnd(",");

	FString Opt = FString::Printf(TEXT("ServerMaps=%s"), *CfgServerMaps);
	FURL URL; URL.AddOption(*Opt);
	URL.SaveURLConfig(TEXT("DeathMatch"), TEXT("ServerMaps"), GGameIni);
}

void UPKGameInstance::RestoreCurrentMaps(TArray<FString> &ServerMaps)
{
	FURL URL;
	URL.LoadURLConfig(TEXT("DeathMatch"), GGameIni);
	FString CfgServerMaps = URL.GetOption(TEXT("ServerMaps="), TEXT(""));
	CfgServerMaps.ParseIntoArray(&ServerMaps, TEXT(","), true);
}