// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/CharacterMovementComponent.h"
#include "Util.h"
#include "PKCharacterMovementComponent.generated.h"

/**
 * 
 */
typedef TSharedPtr<class FPKSavedMove_Character> FPKSavedMovePtr;

UCLASS()
class PK_API UPKCharacterMovementComponent : public UCharacterMovementComponent
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float BaseWalkSpeed = 924.f; // 924.f (11 * 84)
	//float BaseWalkSpeed = 2352.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float MaxBunnyHopVelocity = 2100.f; // 2100.f (26 * 84)

	float MaxRocketJumpVelocity = MaxBunnyHopVelocity * 1.12f; // 2352.f (28 * 84)

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float ExplosionRange = 600.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float BaseFallingLateralFriction = 0.3f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay, meta = (ClampMin = "0", UIMin = "0"))
	float MaxFallingLateralFriction = 3.f;
	
	/*See if the pawn slides down the slope*/
	UPROPERTY(ReplicatedUsing = OnRep_SlideAlongSlope, BlueprintReadOnly, Category = "Pawn|Components|CharacterMovement")
	bool bSlideAlongSlope = false;
	UFUNCTION()
	void OnRep_SlideAlongSlope();

	virtual void CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration) override;

	void GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const override;

	/**************************************************************************/
	UPKCharacterMovementComponent(const FObjectInitializer& ObjectInitializer);
	/**************************************************************************/
	
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction) override;
		
	virtual class FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool ShouldCatchAir(const FFindFloorResult& OldFloor, const FFindFloorResult& NewFloor) override;

	virtual bool DoJump(bool bReplayingMoves) override;

	virtual float GetMaxSpeed() const override;

	virtual bool StepUp(const FVector& GravDir, const FVector& Delta, const FHitResult &Hit, struct UCharacterMovementComponent::FStepDownResult* OutStepDownResult = NULL) override;

	/** Called if bNotifyApex is true and character has just passed the apex of its jump. */
	virtual void NotifyJumpApex() override;
	
	/** Handle falling movement. */
	virtual void PhysFalling(float deltaTime, int32 Iterations) override;

	/** Transition from walking to falling */
	virtual void StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc) override;
	
	/** Perform movement on an autonomous client */
	virtual void PerformMovement(float DeltaTime) override;
	
	void ScheduleRocketJump(TSubclassOf<class APKProjectile>  ProjectileCls);
	UFUNCTION() // Delegate bind
	void ApplyRocketJump(FHitResult RocketHit, FHitResult WallHit, FVector Rebound);
	FORCEINLINE void ReceiveImpact(FVector Impulse)
	{
		ReceivedImpulse += Impulse;
	}
protected:
	UFUNCTION(Server, Reliable, WithValidation)
	void ApplyImpulse(FVector_NetQuantize10 Impulse);
	FORCEINLINE void ApplyImpulse_Implementation(FVector_NetQuantize10 Impulse){ PendingImpulseToApply += Impulse; };
	FORCEINLINE bool ApplyImpulse_Validate(FVector_NetQuantize10 Impulse){ return true; };
public:
	UPROPERTY()
	uint32 bRocketJump : 1; // speed up at MaxRocketJumpVelocity
	UPROPERTY()
	uint32 bWantsToStop : 1;
	UPROPERTY()
	uint32 bSpeedBit01 : 1;
	UPROPERTY()
	uint32 bSpeedBit10 : 1;

	void MoveForward(float Value);
	void MoveRight(float Value);
	/*FORCEINLINE */void Jump()/* { PendingJump = JumpZVelocity / -GetGravityZ(); }*/;

	UFUNCTION(Client, Reliable)
	void ReceiveImpactRPC(FVector_NetQuantize10 HitForce);
	FORCEINLINE void ReceiveImpactRPC_Implementation(FVector_NetQuantize10 HitForce){
		ReceivedImpulse += HitForce;

		// reduce speed two steps down on hit by PainHead, pellets or stake
		if (bRocketJump)
			SetMovementFlags(0, 1, 0, bWantsToStop);
		else
		{
			SetMovementFlags(0, 0, 0, bWantsToStop);
		}
	}

protected:

	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;

	virtual void MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult = NULL) override;
	
	/** @note Movement update functions should only be called through StartNewPhysics()*/
	virtual void PhysWalking(float deltaTime, int32 Iterations) override;

	/** @note Movement update functions should only be called through StartNewPhysics()*/
	virtual void PhysFlying(float deltaTime, int32 Iterations);

	/** Handle a blocking impact. Calls ApplyImpactPhysicsForces for the hit, if bEnablePhysicsInteraction is true. */
	virtual void HandleImpact(FHitResult const& Hit, float TimeSlice = 0.f, const FVector& MoveDelta = FVector::ZeroVector) override;

	virtual void FindFloor(const FVector& CapsuleLocation, struct FFindFloorResult& OutFloorResult, bool bZeroDelta, const FHitResult* DownwardSweepResult = NULL) const override;
		
	FTimerHandle TimerHandle;
	FTimerDelegate RocketJumpTimerDelegate;
			
	float PendingForward = 0.f;
	float PendingRight = 0.f;

	uint8 BunnyHopCount = 0;
	float SecondsWhenYouCanBunnyHopAfterLanding = 0.2f;
	float TimeOutAfterLanding = 0.f;

	void MovementControl(float DeltaTime);
	void SetDirection(float &Forth, float &Across, float Value);
	FVector SlideVector = FVector::ZeroVector;
	void SlideAlongSlope(float deltaTime);
		
	/*apply the 'force' received from hitting a minigun*/
	void ReceiveMinigunHit(/*FVector &FWD, FVector &RGT*/float DeltaTime);
	/*average vector of attacks from enemies*/
	FVector GetAverageHitDirection(uint16 AttackingEnemyFlags);
	FVector MinigunHitReceivedImpulse = FVector::ZeroVector;

	/**
	* 00  924 = 924 * (1 + 0.(42)*0)
	* 01 1316 = 924 * (1 + 0.(42)*1)
	* 10 1708 = 924 * (1 + 0.(42)*2)
	* 11 2100 = 924 * (1 + 0.(42)*3)
	* rj 2352 = MaxRocketJumpVelocity
	*/
	FORCEINLINE void SetMovementFlags(
		bool SpeedBit10,
		bool SpeedBit01,
		bool RocketJump,
		bool WantsToStop
		)
	{
		bSpeedBit10 = SpeedBit10;
		bSpeedBit01 = SpeedBit01;
		bRocketJump = RocketJump;
		bWantsToStop = WantsToStop;
		BunnyHopCount = (bSpeedBit10 << 1 | bSpeedBit01 << 0);
	}

	// climbing the barrier hitted at jump apex.
	// the height of the barrier is greater than the maximum jump height
	// and less than the maximum jump height plus half the pawn height
	bool bJumpApex = false;
	void ClimbOn(FHitResult const& Hit, const FVector& MoveDelta);

	bool bMoveInput = false;
	
public:
	
	void HandleJumpPad(float JumpStrength);
};

class FPKNetworkPredictionData_Client_Character : public FNetworkPredictionData_Client_Character
{
public:

	FPKNetworkPredictionData_Client_Character();
	typedef FNetworkPredictionData_Client_Character Super;

	virtual FSavedMovePtr AllocateNewMove() override;
};

	/**************************************************************************/
	
	/**************************************************************************/

/** FSavedMove_Character represents a saved move on the client that has been sent to the server and might need to be played back. */
class FPKSavedMove_Character : public FSavedMove_Character
{
public:
	FPKSavedMove_Character();
	typedef FSavedMove_Character Super;

	uint32 bSavedRocketJump : 1;
	uint32 bSavedWantsToStop : 1;
	uint32 bSavedSpeedBit01 : 1;
	uint32 bSavedSpeedBit10 : 1;

	/** Clear saved move properties, so it can be re-used. */
	virtual void Clear() override;

	/** Called to set up this saved move (when initially created) to make a predictive correction. */
	virtual void SetMoveFor(ACharacter* C, float DeltaTime, FVector const& NewAccel, FVector const& NewReceivedImp, class FNetworkPredictionData_Client_Character & ClientData) override;

	/** @Return true if this move is an "important" move that should be sent again if not acked by the server */
	virtual bool IsImportantMove(const FSavedMovePtr& LastAckedMove) const override;

	/** @Return true if this move can be combined with NewMove for replication without changing any behavior */
	virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InPawn, float MaxDelta) const override;
		
	/** @returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
	virtual uint8 GetCompressedFlags() const override; // Setup FLAGS
};

