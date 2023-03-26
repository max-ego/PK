// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Character.h"
#include "Main/PKGameInstance.h"
#include "PKCharacterMovementComponent.h"
#include "Util.h"
#include "PKClasses/PKProjectile.h"
#include "PKClasses/PKWeapon.h"
#include "HUD/MainMenuGameMode.h"

#include "NetPlayer.generated.h"


UCLASS(config = Game)
class ANetPlayer : public ACharacter
{
	GENERATED_BODY()

	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
	class USkeletalMeshComponent* PKW_head;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	class UCameraComponent* FirstPersonCameraComponent;

public:
	virtual void ReceiveDestroyed() override;

	/** Called when our Controller no longer possesses us.	 */
	virtual void UnPossessed() override;

	// Sets default values for this character's properties
	ANetPlayer(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TArray<class UChildActorComponent*> Weapons;
	
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* PainKiller;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* Shotgun;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* StakeGunGL;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* MiniGunRL;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* DriverElectro;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* RifleFlameThrower;
	UPROPERTY(VisibleDefaultsOnly)
	class UChildActorComponent* BoltGunHeater;
		
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	TArray<FName> WeaponBones;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Gameplay)
	float DemonWarp = 0.f;

	UPROPERTY(ReplicatedUsing = OnRep_AppearFrozen)
	uint8 bAppearFrozen = 0;
	UFUNCTION()
	void OnRep_AppearFrozen();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	uint8 CurWeaponIndex;

	UPROPERTY(Replicated, BlueprintReadOnly, Category = Gameplay)
	bool bPKHeadOpen = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float BaseWalkSpeed = 924.f; // 924.f (11 * 84)
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float MaxBunnyHopVelocity = 2352.f; // 2352.f (28 * 84)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Ragdoll, meta = (ClampMin = "0", UIMin = "0"))
	float RagdollLifeSpan = 8.0f;

	/** Base turn rate, in deg/sec. Other scaling may affect final turn rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseTurnRate;

	/** Base look up/down rate, in deg/sec. Other scaling may affect final rate. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera)
	float BaseLookUpRate;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
	FVector GunOffset;

	/** Projectile class to spawn */
	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	TSubclassOf<class APKProjectile> ProjectileClass;

	/**
	* Called upon landing when falling, to perform actions based on the Hit result. Triggers the OnLanded event.
	* Note that movement mode is still "Falling" during this event. Current Velocity value is the velocity at the time of landing.
	* Consider OnMovementModeChanged() as well, as that can be used once the movement mode changes to the new mode (most likely Walking).
	*
	* @param Hit Result describing the landing that resulted in a valid landing spot.
	* @see OnMovementModeChanged()
	*/
	virtual void Landed(const FHitResult& Hit) override;

	float collisionMinSpeed = 25.f * 84;
	float collisionMaxSpeed = 75.f * 84;
	float GroundSndDelay = -1;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* InputComponent) override;

	/** Trigger jump if jump button has been pressed. */
	virtual void CheckJumpInput(float DeltaTime) override;

	/** called when the actor falls out of the world 'safely' (below KillZ and such) */
	virtual void FellOutOfWorld(const class UDamageType& dmgType) override;
protected:
	bool bFellOutOfWorld = false;
public:

	/** Event when this actor takes ANY damage */
	/*virtual void ReceiveAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser) override;*/
	TScriptDelegate<FWeakObjectPtr> OnTakeAnyDamageDeligate;
	UFUNCTION()
	void TakeAnyDamage(float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
	
	TScriptDelegate<FWeakObjectPtr> OnTakePointDamageDeligate;
	UFUNCTION()
	void TakePointDamage(float Damage, class AController* InstigatedBy, FVector HitLocation, class UPrimitiveComponent* FHitComponent, FName BoneName, FVector ShotFromDirection, const class UDamageType* DamageType, AActor* DamageCauser);
	
	TScriptDelegate<FWeakObjectPtr> OnEndPlayDeligate;
	UFUNCTION()
	void CustomEndPlay(const EEndPlayReason::Type EndPlayReason);

	/** Event when this actor starts play jump animation (fired up in mp-model_AnimBlueprint) */
	UFUNCTION(BlueprintCallable, Category = "Animation")
	void OnStartJumpAnim();
	
	UPROPERTY(ReplicatedUsing = OnRep_DamageImpulse)
	FVector_NetQuantize DamageImpulse = FVector::ZeroVector;
	UFUNCTION()
	void OnRep_DamageImpulse();
	bool bLocallyControlled;

	virtual void Restart() override;
	virtual void ClearJumpInput() override;

	UFUNCTION(Server, Reliable, WithValidation)
	void SetPKHeadOpen(bool open);
	FORCEINLINE void SetPKHeadOpen_Implementation(bool open){ bPKHeadOpen = open; };
	FORCEINLINE bool SetPKHeadOpen_Validate(bool open){ return true; };

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnProjectile(TSubclassOf<class APKProjectile>  ProjectileCls);
	void SpawnProjectile_Implementation(TSubclassOf<class APKProjectile>  ProjectileCls);
	FORCEINLINE bool SpawnProjectile_Validate(TSubclassOf<class APKProjectile>  ProjectileCls){ return true; }
		
	UFUNCTION(Server, Reliable, WithValidation)
	void StopMinigunAltFire();
	FORCEINLINE void StopMinigunAltFire_Implementation(){
		if (MinigunAltFire != NULL){
			MinigunAltFire/*.Get()*/->Deactivate();
		}
	}
	FORCEINLINE bool StopMinigunAltFire_Validate(){ return true; }

	UFUNCTION(Server, Reliable, WithValidation)
	void StopPainKillerFire();
	FORCEINLINE void StopPainKillerFire_Implementation(){
		if (PainKillerFire != NULL){
			PainKillerFire/*.Get()*/->Deactivate();
		}
	}
	FORCEINLINE bool StopPainKillerFire_Validate(){ return true; }
	
	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	/*******************************
	* SOUND
	*******************************/
	uint8 OldCurWeaponIndex;
	float JumpSndDelay = 0.f;

	float HurtSoundTimeOut = 0;

	UPROPERTY(ReplicatedUsing = OnRep_HurtSmall)
	bool bHurtSmall = 0;
	UFUNCTION()
	void OnRep_HurtSmall();

	UPROPERTY(ReplicatedUsing = OnRep_HurtMedium)
	bool bHurtMedium = 0;
	UFUNCTION()
	void OnRep_HurtMedium();

	UPROPERTY(ReplicatedUsing = OnRep_HurtBig)
	bool bHurtBig = 0;
	UFUNCTION()
	void OnRep_HurtBig();

	UPROPERTY(ReplicatedUsing = OnRep_Death)
	bool bDeath = 0;
	UFUNCTION()
	void OnRep_Death();


	UPROPERTY(ReplicatedUsing = OnRep_SoundFlags)
	uint8 JumpSoundFlags = 0;
	uint8 LastJumpSoundFlags = JumpSoundFlags;
	UFUNCTION()
	void OnRep_SoundFlags();
	
	UPROPERTY(ReplicatedUsing = OnRep_SoundFlags)
	uint8 HurtSoundFlags = 0;
	uint8 LastHurtSoundFlags = HurtSoundFlags;

	// JUMP SOUNDS FLAGS
	uint8 JumpEnd = 0x01;
	uint8 JumpEndHurt = 0x02;
	// HURT / DEATH SOUNDS FLAGS
	uint8 HurtSmall = 0x01;
	uint8 HurtMedium = 0x02;
	uint8 HurtBig = 0x04;
	uint8 DeathFlag = 0x08;

public:
	bool bIsInLava = false;
	float LavaDamage = 0.f;
protected:
	float LavaHurtDelay = 0.f;
	USoundBase* LavaHurtSnd;


	// not used
	UAudioComponent* PlayerHurtAudioComponent;
	USoundCue* HurtSmallSound;
	USoundCue* HurtMediumSound;
	USoundCue* HurtBigSound;

	/*******************************
	* Damage
	*******************************/
	UPROPERTY(Replicated)
	int32 bFallingDamage = 0;

	FLinearColor DefaultColor = FLinearColor(0.96, 0.58, 0.11, 1.f);
	FLinearColor FrozenColor = FLinearColor(0.f, 0.68, 0.95, 1.f);
	float DefaultFrozenColorBlend = 0.f; // 0 - default, 1 - frozen
public:
	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Appearance)
	FLinearColor PlayerColor = DefaultColor;
	FLinearColor TargetColor = DefaultColor;
	float BlendTime = 0.f;
protected:
	FLinearColor BlendColor(FLinearColor A, FLinearColor B, float blend);
	/*******************************
	* RAGDOLL
	*******************************/
	FTimerHandle TimerHandle;
	FORCEINLINE void DestroyAfterRagdoll(){ Destroy(); };

	FTimerHandle DisappearTimerHandle;
	void RagdollDisappear();

	/*******************************
	* PLAYER CONTROLLER
	*******************************/
	class ANetPlayerController* PC;	
	
	/*******************************
	* CONTROL
	*******************************/
	/** Handles moving forward/backward */
	void MoveForward(float Val);

	/** Handles stafing movement, left and right */
	void MoveRight(float Val);

	/**
	* Called via input to turn at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void TurnAtRate(float Rate);

	/**
	* Called via input to turn look up/down at a given rate.
	* @param Rate	This is a normalized rate, i.e. 1.0 means 100% of desired turn rate
	*/
	void LookUpAtRate(float Rate);

	FORCEINLINE void Turn(float Value){	AddControllerYawInput(Value); }

	FORCEINLINE void LookUp(float Value){ AddControllerPitchInput(Value); }

	/*******************************
	* MOVEMENT
	*******************************/
	//UCharacterMovementComponent* CharacterMovement;
	class UPKCharacterMovementComponent* CharacterMovement;

	/*******************************
	* UTIL
	*******************************/
	
	/*******************************
	* INPUTS
	*******************************/
	void PendingJump();

	void OnPause();

	void OnDisconnect();
	
	void ServerTravel();

	void ShowStats();
public:
	void HideStats();
protected:
	//UFUNCTION(Client, Reliable)
	UFUNCTION(Server, Reliable, WithValidation)
	void OnGetNetPlayerIndex();
	void OnGetNetPlayerIndex_Implementation();
	FORCEINLINE bool OnGetNetPlayerIndex_Validate(){ return true; };

	void SelectWeap1(){ QuickWeaponSelection(1); }
	void SelectWeap2(){ QuickWeaponSelection(2); }
	void SelectWeap3(){ QuickWeaponSelection(3); }
	void SelectWeap4(){ QuickWeaponSelection(4); }

	void QuickWeaponSelection(uint8 weap);

	/*******************************
	* WEAPON
	*******************************/
public:

	/*APPLY DRIVING FORCE FROM HITS*/	
	/*FLAGS*/
	uint16 GetPlayerFlag();
	UPROPERTY(Replicated)
	uint16 MinigunAttackingPlayerFlags;
	UPROPERTY(Replicated)
	uint16 ShotgunAttackingPlayerFlags;
	
	/*Minigun*/
	bool bMinigunFire = false; // server side only
	TWeakObjectPtr<AActor> MinigunLastHitEnemy; // server side only
	
	UFUNCTION(Server, Reliable, WithValidation)
	void MinigunFire(bool val);
	FORCEINLINE void MinigunFire_Implementation(bool val){ bMinigunFire = val; }
	FORCEINLINE bool MinigunFire_Validate(bool val){ return true; }
	
	uint16 ClearMinigunLastHitEnemy();
	
	void MinigunHitScan();
	
	/*Shotgun*/
	UFUNCTION(Server, Reliable, WithValidation)
	void ShotgunFire();
	void ShotgunFire_Implementation();
	FORCEINLINE bool ShotgunFire_Validate(){ return true; };
	uint8 Pellets;

	UFUNCTION(NetMulticast, Reliable)
	void ShotgunFireFX();
	void ShotgunFireFX_Implementation();
	UParticleSystem* ShotgunHitWallParticle;

	/*Demon FX*/
	float WarpTime = 1000.f;
	void CalcWarp(float DeltaTime);
	
protected:
	APKWeapon* cw; // current weapon
	APKWeapon* dummyweap;
	// "PainKiller","Shotgun","StakeGunGL","MiniGunRL","DriverElectro","RifleFlameThrower","BoltGunHeater"
	bool EnabledWeapons[8];
	
	//UPROPERTY(Replicated/*Using = OnRep_SelectBestWeapon*/)
	uint8 AvailableWeapons = 0x01; // 00000101 = !DriverElectro !MiniGunRL StakeGunGL !Shotgun PainKiller

public:

	UFUNCTION(Client, Reliable, WithValidation)
	void SelectBestWeapon(uint8 weap);
	void SelectBestWeapon_Implementation(uint8 weap);
	FORCEINLINE bool SelectBestWeapon_Validate(uint8 weap){ return true; }

protected:
	FTimerHandle BestWeaponTimerHandle;
	void RestoreAfterBestWeapon();
	void ChangeWeapon(uint8 NewSlot);
	int32 LastWeaponIndex = -1; // to switch back to last weap after BestWeapon shot
	
	void BestWeaponFire();
	void BestWeaponStopFire();
	
	UPROPERTY(ReplicatedUsing = OnRep_SetCurWeaponIndex)
	uint8 slot;
	// replicate CurWeaponIndex
	UFUNCTION()
	void OnRep_SetCurWeaponIndex();

	UFUNCTION(Server, Reliable, WithValidation)
	void SlotUpdate(uint8 idx);
	void SlotUpdate_Implementation(uint8 idx)
	{
		// simulated proxy (COND_SimulatedOnly)
		slot = idx;
		// instant server
		OnRep_SetCurWeaponIndex();
	}
	FORCEINLINE bool SlotUpdate_Validate(uint8 idx){ return true; }
	
	void NextWeapon() { NextPrevWeapon(true); }
	void PrevWeapon() { NextPrevWeapon(false); }	
	void NextPrevWeapon(bool next);
	
	// delay weapon change
	FTimerHandle WeapChangeTimerHandle;

public: // make it public as a workaround for UVideoOptionsUserWidget::ResetPawnAppearance() 
	// issue is applying 'EffectsQuality' settings affecting hidden bones.
	// so we need to restore visible/invisible state of bones after the changes has been made.
	UFUNCTION(BlueprintCallable, Category = "Mesh")
	void DisplayCorrectWeapon();

protected:
	bool bBlockDisplayCorrectWeapon = false; // for ragdoll
	
	/** Fire a projectile. */
	TWeakObjectPtr<APKProjectile>  PainHead = NULL;
	TWeakObjectPtr<APKProjectile>  MinigunAltFire = NULL;
	TWeakObjectPtr<APKProjectile>  PainKillerFire = NULL;
public:
	APKProjectile* LastProjectile;
	
	UFUNCTION(Server, Reliable, WithValidation)
	void ReturnPainHead();
	FORCEINLINE void ReturnPainHead_Implementation()
	{
		if (PainHead != NULL)
		{
			PainHead->ActivateWithdraw();
		}
	};
	FORCEINLINE bool ReturnPainHead_Validate(){ return true; }
	/**/
	FORCEINLINE AActor* GetCurrWeapon(){ return cw; }

	UFUNCTION(Client, Reliable, WithValidation)
	void OnReturnPainHead();
	FORCEINLINE void OnReturnPainHead_Implementation()
	{
		Cast<APKWeapon>(Weapons[0]->ChildActor)->OnHeadBack();
	};
	FORCEINLINE bool OnReturnPainHead_Validate(){ return true; }
		
	UPROPERTY(ReplicatedUsing = OnRep_PainHeadShot)
	bool bPKHeadHide = false;
	UFUNCTION()
	void OnRep_PainHeadShot();

	FORCEINLINE void HideThirdPersonPainHead(bool bHide)
	{
		bPKHeadHide = bHide;
		OnRep_PainHeadShot(); // server side
	}

	// used to pre-calculate rocket jump (default rocket speed = 3360.f)
	float ProjectileSpeed = 2000.f;
	// called by PlayerController
	FORCEINLINE void OnFire(){ cw->Fire(); };
protected:
	FORCEINLINE void OnFireRelease(){ cw->OnFinishFire(); };
	FORCEINLINE void OnAltFire(){ cw->AltFire(); };
	FORCEINLINE void OnAltFireRelease(){ cw->OnFinishAltFire(); };

public:

	/** Returns FirstPersonCameraComponent subobject **/
	class UCameraComponent* GetFirstPersonCameraComponent() const { return FirstPersonCameraComponent; }
	
};
