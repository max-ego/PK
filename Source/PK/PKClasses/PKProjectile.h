// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "GameFramework/DamageType.h"
#include "PKClasses/PKDamageType.h"
#include "PKClasses/DamageTypes/PainrotorDamage.h"
#include "PKClasses/DamageTypes/RocketDamage.h"
#include "PKClasses/DamageTypes/PainrotorDamage.h"
#include "PKClasses/DamageTypes/PainheadDamage.h"
#include "PKClasses/DamageTypes/PainbeamDamage.h"
#include "PKClasses/DamageTypes/MinigunDamage.h"
#include "PKClasses/DamageTypes/IcebulletDamage.h"
#include "PKClasses/DamageTypes/GrenadeDamage.h"
#include "PKClasses/DamageTypes/StakeDamage.h"
#include "PKClasses/DamageTypes/ShellDamage.h"
#include "PKClasses/DamageTypes/OutOfWorldDamage.h"
#include "PKClasses/DamageTypes/GravityDamage.h"

#include "ParticleDefinitions.h"
#include "Components/BillboardComponent.h"
#include "Main/NetPlayerController.h"
#include "PKClasses/DestructibleItemBase.h"


#include "PKProjectile.generated.h"

UCLASS()
class PK_API APKProjectile : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USkeletalMeshComponent* SkeletalMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UStaticMeshComponent* StaticMeshComponent;

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;
			
public:

	// Sets default values for this actor's properties
	APKProjectile(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class TSubclassOf<UPKDamageType> DamageType;

	UPROPERTY(EditDefaultsOnly, Category = Projectile)
	class TSubclassOf<UPKDamageType> SpecificDamageType;

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void GatherCurrentMovement() override;
	virtual void OnRep_ReplicatedMovement() override;

	void PostInitializeComponents() override;

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComp() const { return SkeletalMeshComponent; }

	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }

	/** called when projectile hits something */
	UFUNCTION()
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);

	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
	void OnDeactivate();
	
	// Disable the projectile after the 'TimeToLive' expires but keep it for some time 
	// to let the trail disappear naturally
	UFUNCTION(NetMulticast, Reliable)
	void Deactivate();
	virtual void Deactivate_Implementation();

	bool bIsActive = true;
	
	// Used to add some velocity from player
	UPROPERTY(ReplicatedUsing = OnRep_AddVelocity)
	FVector_NetQuantize100 AddVelocity = FVector::ZeroVector;
	UFUNCTION()
	virtual void OnRep_AddVelocity();

protected:
	// owning player
	ANetPlayerController* PC;
	bool b3rdPerson;

	// return PainHead
	UPROPERTY(ReplicatedUsing = OnRep_PainHeadWithdraw)
	bool bPainHeadWithdraw = false;
	UFUNCTION()
	virtual void OnRep_PainHeadWithdraw();

	virtual void HookUpEnemy(AActor* OtherActor){};
	
	bool bHitScanEnabled = true;
	virtual void HitScan(){};
	virtual void HitScan(float DeltaTime){};
	AActor* Enemy = nullptr;
	UPROPERTY(Replicated)
	bool bHitEnemy = 0;	// if true spawn blood particle
	FORCEINLINE void ApplySpecificDamage(float SpecDamage, float DeltaTime)
	{
		if (HasAuthority() && Enemy)
		{
			Damage += SpecDamage * DeltaTime;
			if (Damage > SpecDamage / 6.f) // consume damage less frequently
			{
				ApplyDamage(Enemy, SpecificDamageType);
				Damage = 0;
			}
		}
	}
	// Apply damage, clear Enemy ptr, zero out Damage
	FORCEINLINE void ApplyRemainingDamage()
	{
		if (HasAuthority() && Enemy)
		{
			ApplyDamage(Enemy, SpecificDamageType);
			Enemy = nullptr;
			Damage = 0;
			bHitEnemy = 0;
			if (PC) PC->bSpecificHitSound = false;
		}
	}
	float ParticleTimeout = 0.f;

public:

	FORCEINLINE void ActivateWithdraw()
	{
		if (HasAuthority()){
			GetWorldTimerManager().ClearTimer(TimerHandle_GetPainHeadBack);
			bPainHeadWithdraw = true;
			ApplyRemainingDamage();
			OnRep_PainHeadWithdraw(); // localy on server
		}
	};

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> & OutLifetimeProps) const override;

protected:

	// head
	bool bPainHead;
	
	float PainHeadRange = 4800.f; // max. range
	float PainHeadSpeed = 2400.f;
	float PainHeadBackSpeed = 4800.f;
	float PainHeadDamage = 50.f;
	float PainHeadSpinningSpeed = 1200.f;
	float PainHeadSpinningDamage = 300.f; // combo with knife
	float PainHeadBackImpulse = -1200.f; // impuls pulling objects
	float PainHeadMonstersBackVelocity = -800.f; // Monster pulling Speed
	// mower (косилка)
	float PainKnifeDamage = 5.f * 30; // damage done in 30 * sec
	float PainKnifeImpulse = 700.f; // repulsive force
	float PainKnifeRange = 1.8f; // damage distance from the player
	// ray
	float PainRayDamage = 10.f * 30; // damage done in 30 * sec
	float PainRayTolerance = 2.f; // tolerancja dla polaczenia
	
	// Timeout to 'deactivation'
	float TimeToLive = 0.f;
	// Life span after 'deactivation'
	float DeactivatedLifeSpan = 0.5f;
	
	FTimerHandle TimerHandle_GetPainHeadBack;
	FTimerHandle TimerHandle_Deactivate;
	void SetDeactivateTimer(float TimeToLive);

	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	TScriptDelegate<FWeakObjectPtr> OnProjectileDestoyedDelegate;
	UFUNCTION()
	void PainHeadReload();

	bool bCombo = false;
	bool bHitOwner = false;

	FHitResult HitResult;
	float Damage = 0.f;
	virtual void ApplyDamage(AActor* OtherActor, TSubclassOf<UPKDamageType> DamageTypeClass = 0);

	UAudioComponent* AudioComponent;

public:
	/**
	* painhead			0x00
	* stake				0x01
	* grenade			0x02
	* shell				0x04
	* icebullet			0x08
	* minigun bullet	0x10
	* shuriken			0x20
	* electro			0x40
	* painrotor			0x80
	*/
	uint8 AmmoType = 0x00;

	float DefaultProjectileSpeed = SMALL_NUMBER; // avoid division by zero

	UPROPERTY(ReplicatedUsing = OnRep_ProjectileSpeed)
	float ProjectileSpeed = SMALL_NUMBER; // avoid division by zero
	UFUNCTION()
	void OnRep_ProjectileSpeed();

	FORCEINLINE float GetDamage(){ return Damage; };

};
