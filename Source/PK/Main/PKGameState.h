// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/GameState.h"
#include "NetPlayer.h"
#include "PKGameState.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDefaultTimerSignature);
/**
 * 
 */
UCLASS()
class PK_API APKGameState : public AGameState
{
	GENERATED_BODY()
	
public:

	APKGameState(const FObjectInitializer& ObjectInitializer);

	/** Called periodically, overridden by subclasses */
	virtual void DefaultTimer() override;

	/** Event triggered when the DefaultTimer is updated. */
	UPROPERTY(BlueprintAssignable, Category = "Game")
	FDefaultTimerSignature OnDefaultTimer;

	/** used to synchronize elapsed time on the client */
	UPROPERTY(ReplicatedUsing = OnRep_SyncElapsedTime)
	int32 SyncElapsedTime;
	UFUNCTION()
	void OnRep_SyncElapsedTime();

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

	TArray<APlayerState*> GetPlayerStatesSortedById();
	uint16 GetIdxFromPlayerStates(APlayerState* PlayerState);
	ANetPlayer* GetPlayerByFlag(uint16 Flag);

	// match time in seconds
	UPROPERTY(Replicated, EditAnywhere, BlueprintReadWrite, Category = "Config")
	int32 TimeLimit = 0x7FFFFFFF; // maximum positive, approx 68 years
	UFUNCTION(BlueprintCallable, Category = "Config")
	void SetMatchTimeLimit();

	void SayToAllWhoseFrag(const class UDamageType* DamageType, class AController* PC, class AController* InstigatedBy, AActor* DamageCauser);
	void PlayerJoinedMsg(APlayerState* PlayerState);
	void PlayerLeftMsg(AController* Exiting);
	void PlayerNameChangedMsg(APlayerState* PlayerState);

protected:

	/** Called when the state transitions to InProgress */
	virtual void HandleMatchHasStarted() override;

	void UpdateMatchTimeLimit();

	FTimerHandle CountdownTimerHandle;
	void EndOfMatchCountdown();
	uint8 Count;
	void SendClockFlags(uint8 Flags);
};
