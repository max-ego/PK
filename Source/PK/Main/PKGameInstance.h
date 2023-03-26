// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Engine/GameInstance.h"
#include "Util.h"
#include "HUD/MainMenuGameMode.h"
//#if PLATFORM_WINDOWS
#include "Runtime/MoviePlayer/Public/MoviePlayer.h"
//#endif
#include "PKGameInstance.generated.h"


/**
 * 
 */
#define SETTING_SERVERNAME FName(TEXT("SETTING_SERVERNAME"))
#define SETTING_GAMETYPE FName(TEXT("SETTING_GAMETYPE"))
#define SETTING_SERVERIP FName(TEXT("SETTING_SERVERIP"))
#define SETTING_NUMPLAYERS FName(TEXT("SETTING_NUMPLAYERS"))

UENUM(BlueprintType)
enum class EMyEnum : uint8
{
	BranchA,
	BranchB
};

enum class ESessionTypeEnum : uint8
{
	Host,
	Client,
	None
};

UCLASS()/*Config = GameUserSettings*/
class PK_API UPKGameInstance : public UGameInstance
{
	GENERATED_BODY()

	/*UPROPERTY(Config)
	FString TEST;*/
	
public:
	UPKGameInstance(const FObjectInitializer& ObjectInitializer);

	//WORKAROUND
	int32 _SearchIdx;
	FString _ServerIP;
	bool bStartNewGame;
	bool bJoinGame;

	//UPROPERTY()
	TArray<FString> SearchResultsArr = {};
	//UPROPERTY()
	FString ServerName = "Painkiller";
	//UPROPERTY()
	FName LevelName = FName(TEXT("NoLevel"));

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	TArray<FString> DMLevels = {};
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	TArray<FString> CTFLevels = {};
	UPROPERTY(BlueprintReadOnly, Category = "Game")
	TArray<FString> PCFLevels = {};
	
	TArray<FString> CurrentMapsList = {};
	void LoadNextMap();

	virtual void Init() override;
	
	// loading screen stuff
	UFUNCTION()
	void BeginLoadingScreen();
	UFUNCTION() 
	void EndLoadingScreen();

	void UpdateSessionSettings();

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> MainMenuWidgetClass;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Widgets")
	TSubclassOf<class UUserWidget> MultiplayerMenuWidgetClass;

	void OnEscape();
	void GoToMainMenu();

	bool IsInSession();
	void LeaveGame();
	
	void StoreCurrentMaps(TArray<FString> ServerMaps);
	void RestoreCurrentMaps(TArray<FString> &ServerMaps);
	
	void OnStartButtonClick();
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void OnJoinButtonClick(int32 SearchIdx, FString ServerIP);
	
	UPROPERTY()
	UUserWidget* WidgetToFocus = nullptr;
protected:

	void CenterCursor();
	
	UPROPERTY()
	TSubclassOf<class UUserWidget> ThrobberWidgetClass;
	FTimerDelegate SetFocusTimerDelegate;
	UFUNCTION()
	void SetFocus();

	// My
	UPROPERTY() // keep away from GC
	int32 MapIdx = 0;

	UPROPERTY(BlueprintReadOnly, Category = "Game")
	bool bStartOnlineGameComplete = false;

	void FindMaps(TArray<FString> &Levels, FString MapType);

	ESessionTypeEnum GetSessionType();

	// CREATE

	/**
	*	Function to host a game!
	*
	*	@Param		UserID			User that started the request
	*	@Param		SessionName		Name of the Session
	*	@Param		bIsLAN			Is this is LAN Game?
	*	@Param		bIsPresence		"Is the Session to create a presence Session"
	*	@Param		MaxNumPlayers	Number of Maximum allowed players on this "Session" (Server)
	*/
	bool HostSession(TSharedPtr<const FUniqueNetId> UserId, FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	bool HostSession(FName SessionName, bool bIsLAN, bool bIsPresence, int32 MaxNumPlayers);
	
	/* Delegate called when session created */
	FOnCreateSessionCompleteDelegate OnCreateSessionCompleteDelegate;
	/* Delegate called when session started */
	FOnStartSessionCompleteDelegate OnStartSessionCompleteDelegate;

	/** Handles to registered delegates for creating/starting a session */
	FDelegateHandle OnCreateSessionCompleteDelegateHandle;
	FDelegateHandle OnStartSessionCompleteDelegateHandle;

	TSharedPtr<class FOnlineSessionSettings> SessionSettings;

	/**
	*	Function fired when a session create request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnCreateSessionComplete(FName SessionName, bool bWasSuccessful);

	/**
	*	Function fired when a session start request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnStartOnlineGameComplete(FName SessionName, bool bWasSuccessful);

	// SEARCH
	
	/**
	*	Find an online session
	*
	*	@param UserId user that initiated the request
	*	@param bIsLAN are we searching LAN matches
	*	@param bIsPresence are we searching presence sessions
	*/
	void FindSessions(TSharedPtr<const FUniqueNetId> UserId, bool bIsLAN, bool bIsPresence);

	/** Delegate for searching for sessions */
	FOnFindSessionsCompleteDelegate OnFindSessionsCompleteDelegate;

	/** Handle to registered delegate for searching a session */
	FDelegateHandle OnFindSessionsCompleteDelegateHandle;
	bool bOnFindSessionsCompleteDelegateHandle = false;

	// Search result
	TSharedPtr<class FOnlineSessionSearch> SessionSearch;

	/**
	*	Delegate fired when a session search query has completed
	*
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnFindSessionsComplete(bool bWasSuccessful);

	// JOIN
	
	/**
	*	Joins a session via a search result
	*
	*	@param SessionName name of session
	*	@param SearchResult Session to join
	*
	*	@return bool true if successful, false otherwise
	*/
	bool JoinSession(ULocalPlayer* LocalPlayer, /*FName SessionName,*/ const FOnlineSessionSearchResult& SearchResult) override;

	/** Delegate for joining a session */
	FOnJoinSessionCompleteDelegate OnJoinSessionCompleteDelegate;

	/** Handle to registered delegate for joining a session */
	FDelegateHandle OnJoinSessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a session join request has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	void OnJoinSessionComplete(FName SessionName, EOnJoinSessionCompleteResult::Type Result);

	// DESTROY

	/** Delegate for destroying a session */
	FOnDestroySessionCompleteDelegate OnDestroySessionCompleteDelegate;

	/** Handle to registered delegate for destroying a session */
	FDelegateHandle OnDestroySessionCompleteDelegateHandle;

	/**
	*	Delegate fired when a destroying an online session has completed
	*
	*	@param SessionName the name of the session this callback is for
	*	@param bWasSuccessful true if the async action completed without error, false if there was an error
	*/
	virtual void OnDestroySessionComplete(FName SessionName, bool bWasSuccessful);

	// Client
	FOnEndSessionCompleteDelegate OnEndSessionCompleteDelegate;
	FDelegateHandle OnEndSessionCompleteDelegateHandle;
	virtual void OnEndSessionComplete(FName SessionName, bool bWasSuccessful);

	UFUNCTION()
	void OpenLevel(); 

	UFUNCTION()
	void ClientTravel();

	void UpdateJoinGameMenuWidget(bool bSuccess);
	
	FTimerDelegate OpenLevelTimerDelegate;

	APlayerController* _PlayerController;
	FString _TravelURL;
	FTimerDelegate ClientTravelTimerDelegate;

	TSubclassOf<class UUserWidget> DummyWidgetClass;

public:
	// BLUEPRINTCALLABLE FUNCTIONS

	// Creating a Session
	UFUNCTION(BlueprintCallable, Category = "Network|Test", Meta = (ExpandEnumAsExecs = "Branches"))
	void StartOnlineGame(TEnumAsByte<EMyEnum>& Branches);
	// Searching and Finding a Session
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void FindOnlineGames();
	// Joining a Session
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void JoinOnlineGame();
	// Destroying a Session
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void DestroySessionAndLeaveGame();
	// Destroying a Session
	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	void EndSession();

	UFUNCTION(BlueprintCallable, Category = "Network|Test")
	TArray<FString> GetSearchResultArr();

	UFUNCTION(BlueprintCallable, Category = "Game|Config")
	void UpdatePlayerNickName(FString NewName);

	UFUNCTION(BlueprintCallable, Category = "Game|Config")
	FString GetPlayerNickName();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void ShowWidgetClassOf(TSubclassOf<class UUserWidget> WidgetClass);

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnResume();

	UFUNCTION(BlueprintCallable, Category = "Menu")
	void OnDisconnect();

	bool InterruptionConnection();
		
	void OnSearchResultsCompleted();
	
	void OnJoinSessionCompleted();

	void ServerTravel(FString NewMapName);

};
