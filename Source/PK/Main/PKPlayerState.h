// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerState.h"
#include "PKPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class PK_API APKPlayerState : public APlayerState
{
	GENERATED_BODY()
	
public:

	APKPlayerState(const FObjectInitializer& ObjectInitializer);

	// Begin AActor Interface
	virtual void PostInitializeComponents() override;
	virtual void Reset() override;
	virtual FString GetHumanReadableName() const override;

	virtual void BeginPlay() override;

	virtual void OnRep_Score() override;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = PlayerState)
	FString PlayerNickName;
	
	UPROPERTY(Replicated, BlueprintReadOnly, Category = PlayerState)
	float Health = 100.f;

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPlayerNickName(const FString& NickName);
	void SetPlayerNickName_Implementation(const FString& NickName);
	FORCEINLINE bool SetPlayerNickName_Validate(const FString& NickName){ return true; };

	void UpdatePlayerNickName(FString NewNickName);

	// Console
	UFUNCTION(Client, Reliable, WithValidation)
	void AddConsoleMessage(const FString& msg);
	void AddConsoleMessage_Implementation(const FString& msg);
	FORCEINLINE bool AddConsoleMessage_Validate(const FString& msg){ return true; }

	UPROPERTY()
	FString InputString = FString();
	UPROPERTY()
	TArray<FString> Messages;
	UPROPERTY()
	TArray<FString> HudMessages;
		
	UPROPERTY()
	FString FragMessage = FString::FString(); // Draw on Hud for a while
	
	UFUNCTION(Client, Reliable, WithValidation)
	void SetFragMessage(const FString& VictimName);
	void SetFragMessage_Implementation(const FString& VictimName);
	FORCEINLINE bool SetFragMessage_Validate(const FString& VictimName){ return true; }
	
	FTimerHandle FragMsgTimerHandle;
	void ClearFragMessage() { FragMessage.Empty(); };

	UUserWidget* Chat = nullptr;

	void UpdateConsole();
	void UpdateHudMsgs();
	//

	int32 LuciferBadGood = 0;
	void LucifersCommentary(bool Up);
	void FragUp();
	void FragDown(bool IsSuicide);

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;
	
protected:
	FString CfgNickName;
	uint32 _bHasBeenWelcomed : 1;

	uint16 Kills = 0;
	uint16 Deaths = 0;
	uint16 Suicides = 0;

	FTimerHandle TimerHandle;
	UFont* Font;
	float WipeTimeOut = 10.0f;
	UFUNCTION()
	void WipeTopLine();
	void HudAddMessage(FString Text);
	void UpdHudMessages(FString Text);

	TArray<FString> HudMsgs;
	TArray<int32> MsgTime;
};
