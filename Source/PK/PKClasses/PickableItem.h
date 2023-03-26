// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"
#include "Main/NetPlayerController.h"
#include "PickableItem.generated.h"

// /Engine/Source/Runtime/Engine/Classes/Engine/EngineTypes.h

// Note: Explicitly specified enum values must be greater than any previous value and less than 256
UENUM()
namespace EAmmoType
{
	enum Type{
		megapack      = 0x00,
		stake         = 0x01,
		grenade       = 0x02,
		shell         = 0x04,
		icebullet     = 0x08,
		bullets       = 0x10,
		shuriken      = 0x20,
		electro       = 0x40,
		reserved      = 0x80,
		//
		health        = 129,
		megahealth    = 130,
		armor         = 131,
		Painkiller    = 132,
		Shotgun       = 133,
		StakeGunGL    = 134,
		MiniGunRL     = 135,
		DriverElectro = 136
	};
}

UCLASS()
class PK_API APickableItem : public AActor
{
	GENERATED_BODY()

	/*show transform options in Blueprint*/
	class USceneComponent* Scene;

	/** Sphere collision component */
	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USphereComponent* CollisionComp;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class UStaticMeshComponent* StaticMeshComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Properties)
	class USkeletalMeshComponent* SkeletalMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	APickableItem(const FObjectInitializer& ObjectInitializer);

	UPROPERTY(EditDefaultsOnly, Category = Properties)
	TEnumAsByte<EAmmoType::Type> AmmoType;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	uint8 AmmoAdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	uint8 AltAmmoAdd;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Properties)
	float RespawnTime;

#if WITH_EDITOR
	virtual bool CanEditChange(const UProperty* InProperty) const override;
#endif

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	// Called every frame
	virtual void Tick( float DeltaSeconds ) override;

	/** Returns CollisionComp subobject **/
	FORCEINLINE class USphereComponent* GetCollisionComp() const { return CollisionComp; }

	FORCEINLINE class USkeletalMeshComponent* GetSkeletalMeshComp() const { return SkeletalMeshComponent; }

	FORCEINLINE class UStaticMeshComponent* GetStaticMeshComp() const { return StaticMeshComponent; }

	/** called when projectile hits something */
	UFUNCTION()
	virtual void OnHit(AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

	UFUNCTION()
	void OnBeginOverlap(AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool fromSweep, const FHitResult& Hit);
	
	UPROPERTY(EditDefaultsOnly, Category = Properties)
	USoundBase* RespawnSound;

	UPROPERTY(EditDefaultsOnly, Category = Properties)
	USoundBase* PickupSound;

	void GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const override;

protected:
	TScriptDelegate<FWeakObjectPtr> BeginOverlapDelegate;

	virtual bool OnTake(ANetPlayerController* PC);

	UPROPERTY(ReplicatedUsing = OnRep_Toggle)
	bool bActive = true;
	UFUNCTION()
	void OnRep_Toggle();
	bool bFirstTime = true;
	virtual void HideItem(bool bHide);

	UFUNCTION()
	void Respawn();

	UFUNCTION(NetMulticast, Reliable)
	void Deactivate();
	virtual void Deactivate_Implementation();

	UFUNCTION(NetMulticast, Reliable)
	void Activate();
	virtual void Activate_Implementation();
	
	FTimerHandle TimerHandle;

	class USoundAttenuation* AttenuationSettings;
};
