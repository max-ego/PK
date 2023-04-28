// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "SoulMP.generated.h"

UCLASS()
class PK_API ASoulMP : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* SkeletalMeshComponent;

	/*UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class UStaticMeshComponent* StaticMeshComponent;*/

	/** Projectile movement component */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Movement, meta = (AllowPrivateAccess = "true"))
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* PKW;
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* ASG;
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* KGR;
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* CRL;
	/*UPROPERTY(VisibleDefaultsOnly, Category = Projectile)
	class USkeletalMeshComponent* ESL;*/
	TArray<class USkeletalMeshComponent*> WeapItems;
	
public:	
	// Sets default values for this actor's properties
	ASoulMP(const FObjectInitializer& ObjectInitializer);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	virtual void GatherCurrentMovement() override;
	virtual void OnRep_ReplicatedMovement() override;

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }
	/** Returns ProjectileMovement subobject **/
	FORCEINLINE class UProjectileMovementComponent* GetProjectileMovement() const { return ProjectileMovement; }

	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComp() const { return SkeletalMeshComponent; }

	/*FORCEINLINE class UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }*/

	/** called when projectile hits something */
	UFUNCTION()
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	virtual void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);
	
	UPROPERTY(EditDefaultsOnly, Category = Properties)
	USoundBase* PickupSound;
	
	UFUNCTION(BlueprintImplementableEvent, Category = "CustomEvents")
	void OnDeactivate();

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:

	// Disable the projectile after the 'TimeToLive' expires but keep it for some time 
	// to let the trail disappear naturally
	UFUNCTION(NetMulticast, Reliable)
	void Deactivate();
	virtual void Deactivate_Implementation();

	float DefaultProjectileSpeed = SMALL_NUMBER;

	UAnimationAsset* AnimAsset;

	// Timeout to 'deactivation'
	float TimeToLive = 0.f;
	// Life span after 'deactivation'
	float DeactivatedLifeSpan = 0.75f;

	FTimerHandle TimerHandle_Deactivate;
	void SetDeactivateTimer(float TimeToLive);

	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	ACharacter* Owner;

	uint8 WeapIdx = 0;
	uint8 Health = 15;

	uint8 stake = 0;
	uint8 grenade = 0;
	uint8 shell = 0;
	uint8 icebullet = 0;
	uint8 bullets = 0;

	bool bLoaded = false;

	UPROPERTY(ReplicatedUsing = OnRep_Taken)
	bool bPicked = false;
	UFUNCTION()
	void OnRep_Taken();

	UPROPERTY(ReplicatedUsing = OnRep_SpawnDir)
	FVector_NetQuantize10 SpawnDir = FVector::ZeroVector;
	UFUNCTION()
	void OnRep_SpawnDir();

	virtual void SpawnParticles();
	virtual void KillParticles();

	UParticleSystem* energy_red;

	/*UParticleSystemComponent* root;*/
	UParticleSystemComponent* e1;
	UParticleSystemComponent* e2;
	UParticleSystemComponent* e3;
	UParticleSystemComponent* e4;
	UParticleSystemComponent* e5;
	UParticleSystemComponent* e6;
};
