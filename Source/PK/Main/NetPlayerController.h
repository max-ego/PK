// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/PlayerController.h"
#include "Main/Util.h"

#include "NetPlayerController.generated.h"

/**
 * 
 */

UENUM()
/* enum EState{
	none,
	possessed,
	unpossessed,
}; */

enum EInputMode{
	GameAndUI,
	UIOnly,
	GameOnly,
};

enum ESound : uint8
{
	PLAYER_has_entered_the_arena,
	PLAYER_chickened_and_left_the_arena,
	ClockTick,
	ClockTock,
	ClockBell,
	ClockBellBigger,
	lucifer_comemychildren,
	Lucifer_fight,
	Lucifer_verybad,
	Lucifer_bad,
	Lucifer_good,
	Lucifer_excellent,
	Lucifer_time01,
	Lucifer_time02,
	Lucifer_time03,
	Lucifer_time04,
	Lucifer_time05,
	Lucifer_time06,
	Lucifer_time07,
	Lucifer_time08,
	Lucifer_time09,
	Lucifer_time10,
	Lucifer_time15,
	Lucifer_time20,
	Lucifer_time25,
	wrong_place,
	respawn_m,
};

UCLASS()
class PK_API ANetPlayerController : public APlayerController
{
	GENERATED_BODY()
	
public:
	ANetPlayerController(const FObjectInitializer& ObjectInitializer);
	
	virtual void BeginPlay() override;

	// Pawn has been possessed
	virtual void BeginPlayingState() override;

	virtual void AcknowledgePossession(class APawn* P) override;

	/*virtual void ServerAcknowledgePossession_Implementation(APawn* P) override;*/

	virtual void Possess(APawn* InPawn) override;

	virtual void UnPossess() override;

	virtual void Tick(float DeltaSeconds) override;

	virtual void SetupInputComponent() override;

	virtual void PreClientTravel(const FString& PendingURL, ETravelType TravelType, bool bIsSeamlessTravel) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;
	
	TWeakObjectPtr<APawn> LastPawn = NULL;

protected:
	//!!!!!!!!!!!!!!!!!!!
	//UPROPERTY()
	TSubclassOf<class UUserWidget> ScoreBoardWidgetClass;
	//UPROPERTY()
	TSubclassOf<class UUserWidget> ChatWidgetClass;
		
	FTimerHandle TimerHandle;
	float Delay = 1.0f;
	UFUNCTION()
	void SetPawnDestoyedDelegate();
	UFUNCTION()
	void SetRespawnTimer();
	UFUNCTION()
	void RespawnPlayer();
		
	/*******************************
	* INPUTS
	*******************************/
	bool bBlockOnFire;
	UFUNCTION()
	void UnblockOnFire();
	void OnFire();
	void OnEscape();
	UFUNCTION(Server, Reliable, WithValidation)
	void Info();
	void Info_Implementation();
	FORCEINLINE bool Info_Validate(){ return true; }
		
	void ToggleChat();
	void HideStats();
public:
	// lock spawn till score acked, preventing Lucifer's 'fight' call when respawn after the first death
	// as the client still has zero deaths for some time due to delayed response from the server
	bool bLockSpawn = false;
	bool bChat = false;

	void ShowStats();

	UFUNCTION(Server, Reliable, WithValidation)
	void RequestRespawn();
	FORCEINLINE void RequestRespawn_Implementation(){ SetRespawnTimer(); }
	FORCEINLINE bool RequestRespawn_Validate(){ return true; }
		
	UFUNCTION(Client, Reliable)
	void SetupOnRagdollView();
	void SetupOnRagdollView_Implementation();

	UFUNCTION(Server, Reliable, WithValidation)
	void SeverAbandonPawn();
	FORCEINLINE void SeverAbandonPawn_Implementation(){ UnPossess(); }
	FORCEINLINE bool SeverAbandonPawn_Validate(){ return true; }
	
	bool bDrawHUD;
	UPROPERTY(Replicated)
	uint8 Health = 0;
	UPROPERTY(Replicated)
	uint8 Armor = 0;

	enum EArmorType{
		None,
		Weak,
		Medium,
		Strong
	};
	void FreezeArmor();
protected:
	FTimerHandle FreezeArmorTimerHandle;
	uint8 FrozenArmor = 0;
	float FrozenTime = 5.f;
	UFUNCTION()
	void UnFreezeArmor();

	void SetFrozenAppearance(bool bFrozen);

	FTimerHandle HealthDecTimerHandle;
	FORCEINLINE void HealthDecay()
	{
		if (Health > 100)
		{
			if (HealthDecTimerHandle.IsValid())
			{
				Health--;
			}
			GetWorldTimerManager().SetTimer(HealthDecTimerHandle, this, &ANetPlayerController::HealthDecay, 1.0f, false);
		}
		else{
			GetWorldTimerManager().ClearTimer(HealthDecTimerHandle);
			HealthDecTimerHandle.Invalidate();
		}
	}

public:
	/*TArray<float> ArmorRescueFactor;*/

	bool OnDamage(float Damage);
		
	UPROPERTY(ReplicatedUsing = OnRep_AckStake)
	uint8 stake = 0;
	UFUNCTION()
	void OnRep_AckStake();
	UPROPERTY(ReplicatedUsing = OnRep_AckGrenade)
	uint8 grenade = 0;
	UFUNCTION()
	void OnRep_AckGrenade();
	UPROPERTY(ReplicatedUsing = OnRep_AckShell)
	uint8 shell = 0;
	UFUNCTION()
	void OnRep_AckShell();
	UPROPERTY(ReplicatedUsing = OnRep_AckIcebullet)
	uint8 icebullet = 0;
	UFUNCTION()
	void OnRep_AckIcebullet();
	UPROPERTY(ReplicatedUsing = OnRep_AckBullets)
	uint8 bullets = 0; //minigun
	UFUNCTION()
	void OnRep_AckBullets();

	// lock firing till ack ammo
	uint8 LockedWeapons = 0;
	void AckAmmo(uint8 AmmoType);

	void ConsumeAmmo(uint8 AmmoType)
	{
		stake -= (AmmoType & 0x01) >> 0;
		grenade -= (AmmoType & 0x02) >> 1;
		shell -= (AmmoType & 0x04) >> 2;
		icebullet -= (AmmoType & 0x08) >> 3;
		//bullets   -= (AmmoType & 0x10) >> 3; //x2
		// consuming two bullets at once leads to a rollover to 255 when reaching 0
		uint8 bulletsCons = AmmoType & 0x10 ? (AmmoType & 0x10) >> 3: 0;
		bullets = bullets - bulletsCons < 0 ? 0 : bullets - bulletsCons;

		if (AmmoType & 0x01) UpdateStakeVisibility(); // server side
	}

	bool AmmoAdd(uint8 AmmoType, uint8 AmmoAdd, uint8 AltAmmoAdd = 0);

	FORCEINLINE void ResetAmmo(){
		stake = 0;
		grenade = 0;
		shell = 0;
		icebullet = 0;
		bullets = 0;
	}

	void SetStartAmmo(APawn* InPawn);

	uint8 GetAmmo(uint8 AmmoType)
	{
		switch (AmmoType){
		case 0x01:
			return stake;
			break;
		case 0x02:
			return grenade;
			break;
		case 0x04:
			return shell;
			break;
		case 0x08:
			return icebullet;
			break;
		case 0x10:
			return bullets;
			break;
		default:
			return 0;
			break;
		}
	}

	bool IsMaxAmmo(uint8 AmmoType)
	{
		switch (AmmoType){
		case 0x01:
			return stake == 100;
			break;
		case 0x02:
			return grenade == 100;
			break;
		case 0x04:
			return shell == 100;
			break;
		case 0x08:
			return icebullet == 100;
			break;
		case 0x10:
			return bullets == 250;
			break;
		default:
			return true;
			break;
		}
	}

	// Net
	uint32 PacketLoss;

	FString OpposingPawnName = FString();

protected:
	void UpdateOpposingPawnName();

	UFUNCTION()
	void UpdatePacketLoss();
	
	UFUNCTION()
	void HandleEndPlay();
	bool bLuciferTimer = 0;

	TWeakObjectPtr<USkeletalMeshComponent> Ragdoll;
	TWeakObjectPtr<ACameraActor> Camera;
	void SetCamera();
	float FOV;
	float FOVTransTime = 2.0;
	float FOVBlendVal = 0.f;
				
	/* EState PawnState = none; */		
	
	UFUNCTION()
	void SetWarmUpView();
	
	UFUNCTION(Client, Reliable)
	void SetWarmUpViewClnt();
	void SetWarmUpViewClnt_Implementation(){};
	
	FTimerHandle RespawnTimerHandle;
	TScriptDelegate<FWeakObjectPtr> OnPawnDestoyedDelegate;
	FTimerDelegate RespawnTimerDelegate;
	FTimerDelegate WarmUpViewTimerDelegate;

	TScriptDelegate<FWeakObjectPtr> OnDefaultTimerDelegate;
	TScriptDelegate<FWeakObjectPtr> OnEndPlayDelegate;

	int32 LastElapsedTime = 0;
	void LuciferTimer();
	FTimerHandle CountdownTimerHandle;
	void EndOfMatchCountdown();
	uint8 Count;

public:
	FORCEINLINE TSubclassOf<class UUserWidget> GetScoreBoard(){ return ScoreBoardWidgetClass; };
		
	//Note: Replicated FString parameters must be passed by const reference
	UFUNCTION(Server, Reliable, WithValidation)
	void ConsoleSendMessage(const FString& msg);
	void ConsoleSendMessage_Implementation(const FString& msg);
	FORCEINLINE bool ConsoleSendMessage_Validate(const FString& msg){ return true; };

protected:
	USoundBase* HitSound;
	USoundBase* KillSound;
	USoundBase* NewPlayerJoinedServer;
	USoundBase* PlayerLeft;

	USoundBase* TickSnd;
	USoundBase* TockSnd;
	USoundBase* BellSnd;
	USoundBase* BellBiggerSnd;

public:
	
	UPROPERTY(Replicated)
	bool bSpecificHitSound = 0;
	float SpecificHitSoundTimeOut = -1;

	UFUNCTION(Client, Reliable, WithValidation)
	void PlayHitSound();
	void PlayHitSound_Implementation()
	{
		FSlateSound NewSound;
		NewSound.SetResourceObject(HitSound);
		FSlateApplication::Get().PlaySound(NewSound);
	}
	FORCEINLINE bool PlayHitSound_Validate(){ return true; }

	UFUNCTION(Client, Reliable, WithValidation)
	void PlayKillSound();
	void PlayKillSound_Implementation()
	{
		FSlateSound NewSound;
		NewSound.SetResourceObject(KillSound);
		FSlateApplication::Get().PlaySound(NewSound);
	}
	FORCEINLINE bool PlayKillSound_Validate(){ return true; }

	UFUNCTION(Client, Reliable, WithValidation)
	void PlaySnd(uint8 snd);
	void PlaySnd_Implementation(uint8 snd);
	FORCEINLINE bool PlaySnd_Validate(uint8 snd){ return true; }

	UPROPERTY(ReplicatedUsing = OnRep_ClockFlags)
	uint8 ClockFlags = 0x00;
	uint8 LastClockFlags = ClockFlags;
	UFUNCTION()
	void OnRep_ClockFlags();
	// clocks
	uint8 ClockTickBit = 0x01;
	uint8 ClockTockBit = 0x02;
	uint8 ClockBellBit = 0x04;
	uint8 ClockBellBiggerBit = 0x08;

	/*FlashScreen*/
	uint8 PickUpFlashBit = 0x10;
	uint8 DamageFlashBit = 0x20;

	FLinearColor PickUpFlashColor = FLinearColor(0.3, 0.3, 0.7, 0.f);
	FLinearColor DamageFlashColor = FLinearColor(0.8, 0.f, 0.f, 0.f);

protected:
	float TickCount = -1.f;
	float TickCountDamage = -1.f;

	void PickUpFlash()
	{
		if (TickCount < 0)
			TickCount = 0.f;
		else if (TickCount > PI)
			TickCount = 2 * PI - TickCount;
	}

	void DamageFlash()
	{
		if (TickCountDamage < 0)
			TickCountDamage = 0.f;
		else if (TickCountDamage > PI)
			TickCountDamage = 2 * PI - TickCountDamage;
	}

	bool CheckPickUp(bool bFlash)
	{
		if (bFlash)
		{
			ClockFlags = ClockFlags ^ PickUpFlashBit;
			OnRep_ClockFlags(); // server side
			return true;
		}
		return false;
	}
	
	void UpdateScreenFlash(float DeltaTime);

	void UpdateStakeVisibility();
};
