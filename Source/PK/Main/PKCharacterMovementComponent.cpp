// Fill out your copyright notice in the Description page of Project Settings.

#include "PK.h"
#include "PKCharacterMovementComponent.h"
#include "UnrealNetwork.h"
#include "NetPlayer.h"
#include "PKPlayerState.h"
#include "PKGameState.h"

#include "Templates/Weapons/Projectiles/Rocket.h"

// MAGIC NUMBERS
const float MAX_STEP_SIDE_Z = 0.08f;	// maximum z value for the normal on the vertical side of steps
const float VERTICAL_SLOPE_NORMAL_Z = 0.001f; // Slope is vertical if Abs(Normal.Z) <= this threshold. Accounts for precision problems that 

// Version that does not use inverse sqrt estimate, for higher precision.
FORCEINLINE FVector GetClampedToMaxSizePrecise(const FVector& V, float MaxSize)
{
	if (MaxSize < KINDA_SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}

	const float VSq = V.SizeSquared();
	if (VSq > FMath::Square(MaxSize))
	{
		return V * (MaxSize / FMath::Sqrt(VSq));
	}
	else
	{
		return V;
	}
}

// Version that does not use inverse sqrt estimate, for higher precision.
FORCEINLINE FVector GetSafeNormalPrecise(const FVector& V)
{
	const float VSq = V.SizeSquared();
	if (VSq < SMALL_NUMBER)
	{
		return FVector::ZeroVector;
	}
	else
	{
		return V * (1.f / FMath::Sqrt(VSq));
	}
}

UPKCharacterMovementComponent::UPKCharacterMovementComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	//bUseFlatBaseForFloorChecks = true;
	bMaintainHorizontalGroundVelocity = false; //false = walking movement maintains velocity magnitude parallel to the ramp surface.

	GetNavAgentPropertiesRef().bCanJump = true;
	GetNavAgentPropertiesRef().bCanWalk = true;
	SetJumpAllowed(true);
	//This only needs to be called by engine code when constructing blueprint components.
	//SetNetAddressable(); // Make DSO components net addressable (DSO DefaultSubobject)
	SetIsReplicated(true); // Enable replication by default

	MaxWalkSpeed = BaseWalkSpeed;
	//MaxCustomMovementSpeed = 200.f; // slide off the slope speed

	AirControl = 0.35f;
	JumpZVelocity = 800.f;
	/*MaxStepHeight = 25.0f;*/ // 45.0f;
	SetWalkableFloorAngle(65.0f); // Acos(0.71f) ~ 45 deg
	
	MaxAcceleration = 99999.f;
	GroundFriction = 3.0f;
	BrakingDecelerationWalking = 1024.f;
	FallingLateralFriction = BaseFallingLateralFriction;

	RocketJumpTimerDelegate.BindUFunction(this, FName("ApplyRocketJump"));
}

//OVERRIDDEN FUNCs
void UPKCharacterMovementComponent::TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction *ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	MovementControl(DeltaTime);
}

FPKNetworkPredictionData_Client_Character::FPKNetworkPredictionData_Client_Character()
: Super()
{

}

FPKSavedMove_Character::FPKSavedMove_Character()
: Super()
{
	
}

FNetworkPredictionData_Client* UPKCharacterMovementComponent::GetPredictionData_Client() const
{
	// Should only be called on client in network games
	check(CharacterOwner != NULL);
	check(CharacterOwner->Role < ROLE_Authority);
	check(GetNetMode() == NM_Client);

	if (!ClientPredictionData)
	{
		UPKCharacterMovementComponent* MutableThis = const_cast<UPKCharacterMovementComponent*>(this);
		MutableThis->ClientPredictionData = new FPKNetworkPredictionData_Client_Character();
	}

	return ClientPredictionData;
}

FSavedMovePtr FPKNetworkPredictionData_Client_Character::AllocateNewMove()
{
	return FSavedMovePtr(new FPKSavedMove_Character());
}

void FPKSavedMove_Character::Clear()
{
	Super::Clear();

	bSavedRocketJump = false;
	bSavedWantsToStop = false;
	bSavedSpeedBit01 = false;
	bSavedSpeedBit10 = false;
}

void FPKSavedMove_Character::SetMoveFor(ACharacter* Character, float InDeltaTime, FVector const& NewAccel, FVector const& NewReceivedImp, class FNetworkPredictionData_Client_Character & ClientData)
{
	Super::SetMoveFor(Character, InDeltaTime, NewAccel, NewReceivedImp, ClientData);

	UPKCharacterMovementComponent* CharMov = Cast<UPKCharacterMovementComponent>(Character->GetCharacterMovement());
	if (CharMov)
	{
		bSavedRocketJump = CharMov->bRocketJump;
		bSavedWantsToStop = CharMov->bWantsToStop;		
		bSavedSpeedBit01 = CharMov->bSpeedBit01;		
		bSavedSpeedBit10 = CharMov->bSpeedBit10;
	}
}

bool FPKSavedMove_Character::IsImportantMove(const FSavedMovePtr& LastAckedMove) const
{	
	FPKSavedMove_Character* LAM = const_cast<FPKSavedMove_Character*>((FPKSavedMove_Character*)&LastAckedMove);
	/*const FPKSavedMove_Character* LAM = (FPKSavedMove_Character*)&LastAckedMove;*/
	
	if (LAM)
	{
		if (bSavedRocketJump != LAM->bSavedRocketJump
			|| bSavedWantsToStop != LAM->bSavedWantsToStop
			|| bSavedSpeedBit01 != LAM->bSavedSpeedBit01
			|| bSavedSpeedBit10 != LAM->bSavedSpeedBit10)
		{
			return true;
		}
	}

	return Super::IsImportantMove(LastAckedMove);
}

bool FPKSavedMove_Character::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InPawn, float UserDelta) const
{
	FPKSavedMove_Character* NM = const_cast<class FPKSavedMove_Character*>((FPKSavedMove_Character*)&NewMove);
	/*const FPKSavedMove_Character* NM = (FPKSavedMove_Character*)&NewMove;*/

	if (NM)
	{
		if (bSavedRocketJump != NM->bSavedRocketJump
			|| bSavedWantsToStop != NM->bSavedWantsToStop
			|| bSavedSpeedBit01 != NM->bSavedSpeedBit01
			|| bSavedSpeedBit10 != NM->bSavedSpeedBit10)
		{
			return false;
		}
	}
	
	return Super::CanCombineWith(NewMove, InPawn, UserDelta);
}

uint8 FPKSavedMove_Character::GetCompressedFlags() const
{
	uint8 Result = Super::GetCompressedFlags();

	if (bSavedRocketJump)
	{
		Result |= FLAG_Custom_0;
	}
	
	if (bSavedWantsToStop)
	{
		Result |= FLAG_Custom_1;
	}
	
	if (bSavedSpeedBit01)
	{
		Result |= FLAG_Custom_2;
	}
	
	if (bSavedSpeedBit10)
	{
		Result |= FLAG_Custom_3;
	}

	return Result;
}

void UPKCharacterMovementComponent::UpdateFromCompressedFlags(uint8 Flags)
{
	Super::UpdateFromCompressedFlags(Flags);
	
	if (!CharacterOwner) return;

	bRocketJump = ((Flags & FSavedMove_Character::FLAG_Custom_0) != 0);
	bWantsToStop = ((Flags & FSavedMove_Character::FLAG_Custom_1) != 0);	
	bSpeedBit01 = ((Flags & FSavedMove_Character::FLAG_Custom_2) != 0);	
	bSpeedBit10 = ((Flags & FSavedMove_Character::FLAG_Custom_3) != 0);
}

bool UPKCharacterMovementComponent::ShouldCatchAir(const FFindFloorResult& OldFloor, const FFindFloorResult& NewFloor)
{
	return false;
}

void UPKCharacterMovementComponent::MoveAlongFloor(const FVector& InVelocity, float DeltaSeconds, FStepDownResult* OutStepDownResult)
{
	Super::MoveAlongFloor(InVelocity, DeltaSeconds, OutStepDownResult);
}

bool UPKCharacterMovementComponent::StepUp(const FVector& InGravDir, const FVector& Delta, const FHitResult &InHit, FStepDownResult* OutStepDownResult)
{
	/*return Super::StepUp(InGravDir, Delta, InHit, OutStepDownResult);*/

	if (!CanStepUp(InHit))
	{
		return false;
	}

	if (MaxStepHeight <= 0.f)
	{
		return false;
	}

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	// Don't bother stepping up if top of capsule is hitting something.
	const float InitialImpactZ = InHit.ImpactPoint.Z;
	if (InitialImpactZ > OldLocation.Z + (PawnHalfHeight - PawnRadius))
	{
		return false;
	}

	// Don't step up if the impact is below us
	if (InitialImpactZ <= OldLocation.Z - PawnHalfHeight)
	{
		return false;
	}

	const FVector GravDir = InGravDir.GetSafeNormal();
	if (GravDir.IsZero())
	{
		return false;
	}

	float JumpOverHeight = PawnHalfHeight;

	float StepTravelUpHeight = JumpOverHeight/*MaxStepHeight*/;
	float StepTravelDownHeight = StepTravelUpHeight;
	const float StepSideZ = -1.f * (InHit.ImpactNormal | GravDir);
	float PawnInitialFloorBaseZ = OldLocation.Z - PawnHalfHeight;
	float PawnFloorPointZ = PawnInitialFloorBaseZ;

	if (IsMovingOnGround() && CurrentFloor.IsWalkableFloor())
	{
		// Since we float a variable amount off the floor, we need to enforce max step height off the actual point of impact with the floor.
		const float FloorDist = FMath::Max(0.f, CurrentFloor.FloorDist);
		PawnInitialFloorBaseZ -= FloorDist;
		StepTravelUpHeight = FMath::Max(StepTravelUpHeight - FloorDist, 0.f);
		StepTravelDownHeight = (JumpOverHeight/*MaxStepHeight*/ + MAX_FLOOR_DIST*2.f);

		const bool bHitVerticalFace = !IsWithinEdgeTolerance(InHit.Location, InHit.ImpactPoint, PawnRadius);
		if (!CurrentFloor.bLineTrace && !bHitVerticalFace)
		{
			PawnFloorPointZ = CurrentFloor.HitResult.ImpactPoint.Z;
		}
		else
		{
			// Base floor point is the base of the capsule moved down by how far we are hovering over the surface we are hitting.
			PawnFloorPointZ -= CurrentFloor.FloorDist;
		}
	}

	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	FScopedMovementUpdate ScopedStepUpMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult SweepUpHit(1.f);
	const FRotator PawnRotation = CharacterOwner->GetActorRotation();
	SafeMoveUpdatedComponent(-GravDir * StepTravelUpHeight, PawnRotation, true, SweepUpHit);

	// step fwd
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

	// If we hit something above us and also something ahead of us, we should notify about the upward hit as well.
	// The forward hit will be handled later (in the bSteppedOver case below).
	// In the case of hitting something above but not forward, we are not blocked from moving so we don't need the notification.
	if (SweepUpHit.bBlockingHit && Hit.bBlockingHit)
	{
		HandleImpact(SweepUpHit);
	}

	// Check result of forward movement
	if (Hit.bBlockingHit)
	{
		if (Hit.bStartPenetrating)
		{
			// Undo movement
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// pawn ran into a wall
		HandleImpact(Hit);
		if (IsFalling())
		{
			return true;
		}

		// adjust and try again
		const float ForwardHitTime = Hit.Time;
		const float ForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		if (IsFalling())
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
		if (ForwardHitTime == 0.f && ForwardSlideAmount == 0.f)
		{
			ScopedStepUpMovement.RevertMove();
			return false;
		}
	}

	// Step down
	SafeMoveUpdatedComponent(GravDir * StepTravelDownHeight, CharacterOwner->GetActorRotation(), true, Hit);

	// If step down was initially penetrating abort the step up
	if (Hit.bStartPenetrating)
	{
		ScopedStepUpMovement.RevertMove();
		return false;
	}

	FStepDownResult StepDownResult;
	if (Hit.IsValidBlockingHit())
	{
		// See if this step sequence would have allowed us to travel higher than our max step height allows.
		const float DeltaZ = Hit.ImpactPoint.Z - PawnFloorPointZ;
		if (DeltaZ > MaxStepHeight)
		{
			ScopedStepUpMovement.RevertMove(); // Revert anyway, whether true or false is returned.
			if (DeltaZ < JumpOverHeight && IsWalkable(Hit) && !Acceleration.IsZero() && !bSlideAlongSlope && (bSpeedBit10 || bSpeedBit01))
			{
				SafeMoveUpdatedComponent(FVector(0, 0, DeltaZ/3), PawnRotation, true, Hit);
				Velocity = FVector::ZeroVector;
				Velocity.Z = FMath::Sqrt(-GetGravityZ() * DeltaZ * 2.f) * (1.f + (bSpeedBit10 << 1 | bSpeedBit01 << 0) * 0.33f);
				if (CharacterOwner->IsLocallyControlled())
				{
					// reduce speed
					if (bRocketJump)
						SetMovementFlags(1, 1, 0, bWantsToStop);
					else
					{
						uint8 rate = (bSpeedBit10 << 1 | bSpeedBit01 << 0) > 2 ? 2 : (bSpeedBit10 << 1 | bSpeedBit01 << 0);
						SetMovementFlags((0x02 & rate) != 0, (0x01 & rate) != 0, 0, bWantsToStop);
					}
				}
				SetMovementMode(MOVE_Falling);
				return true;
			}
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (too high Height %.3f) up from floor base %f to %f"), DeltaZ, PawnInitialFloorBaseZ, NewLocation.Z);
			return false;
		}

		// Reject unwalkable surface normals here.
		if (!IsWalkable(Hit))
		{
			// Reject if normal opposes movement direction
			const bool bNormalTowardsMe = (Delta | Hit.ImpactNormal) < 0.f;
			if (bNormalTowardsMe)
			{
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s opposed to movement)"), *Hit.ImpactNormal.ToString());
				ScopedStepUpMovement.RevertMove();
				return false;
			}

			// Also reject if we would end up being higher than our starting location by stepping down.
			// It's fine to step down onto an unwalkable normal below us, we will just slide off. Rejecting those moves would prevent us from being able to walk off the edge.
			if (Hit.Location.Z > OldLocation.Z)
			{
				//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (unwalkable normal %s above old position)"), *Hit.ImpactNormal.ToString());
				ScopedStepUpMovement.RevertMove();
				return false;
			}
		}

		// Reject moves where the downward sweep hit something very close to the edge of the capsule. This maintains consistency with FindFloor as well.
		if (!IsWithinEdgeTolerance(Hit.Location, Hit.ImpactPoint, PawnRadius))
		{
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (outside edge tolerance)"));
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// Don't step up onto invalid surfaces if traveling higher.
		if (DeltaZ > 0.f && !CanStepUp(Hit))
		{
			//UE_LOG(LogCharacterMovement, VeryVerbose, TEXT("- Reject StepUp (up onto surface with !CanStepUp())"));
			ScopedStepUpMovement.RevertMove();
			return false;
		}

		// See if we can validate the floor as a result of this step down. In almost all cases this should succeed, and we can avoid computing the floor outside this method.
		if (OutStepDownResult != NULL)
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), StepDownResult.FloorResult, false, &Hit);

			// Reject unwalkable normals if we end up higher than our initial height.
			// It's fine to walk down onto an unwalkable surface, don't reject those moves.
			if (Hit.Location.Z > OldLocation.Z)
			{
				// We should reject the floor result if we are trying to step up an actual step where we are not able to perch (this is rare).
				// In those cases we should instead abort the step up and try to slide along the stair.
				if (!StepDownResult.FloorResult.bBlockingHit && StepSideZ < MAX_STEP_SIDE_Z)
				{
					ScopedStepUpMovement.RevertMove();
					return false;
				}
			}

			StepDownResult.bComputedFloor = true;
		}
	}

	// Copy step down result.
	if (OutStepDownResult != NULL)
	{
		*OutStepDownResult = StepDownResult;
	}

	// Don't recalculate velocity based on this height adjustment, if considering vertical adjustments.
	bJustTeleported |= !bMaintainHorizontalGroundVelocity;

	return true;
}

// set the step height down to 0.5 of the step height up
void UPKCharacterMovementComponent::FindFloor(const FVector& CapsuleLocation, FFindFloorResult& OutFloorResult, bool bZeroDelta, const FHitResult* DownwardSweepResult) const
{
	// No collision, no floor...
	if (!UpdatedComponent->IsCollisionEnabled())
	{
		OutFloorResult.Clear();
		return;
	}

	// Increase height check slightly if walking, to prevent floor height adjustment from later invalidating the floor result.
	const float HeightCheckAdjust = (IsMovingOnGround() ? MAX_FLOOR_DIST + KINDA_SMALL_NUMBER : -MAX_FLOOR_DIST);

	float _MaxStepHeight = MaxStepHeight * 0.5;
	float FloorSweepTraceDist = FMath::Max(MAX_FLOOR_DIST, _MaxStepHeight + HeightCheckAdjust);
	float FloorLineTraceDist = FloorSweepTraceDist;
	bool bNeedToValidateFloor = true;

	// Sweep floor
	if (FloorLineTraceDist > 0.f || FloorSweepTraceDist > 0.f)
	{
		UPKCharacterMovementComponent* MutableThis = const_cast<UPKCharacterMovementComponent*>(this);

		if (bAlwaysCheckFloor || !bZeroDelta || bForceNextFloorCheck || bJustTeleported)
		{
			MutableThis->bForceNextFloorCheck = false;
			ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
		}
		else
		{
			// Force floor check if base has collision disabled or if it does not block us.
			UPrimitiveComponent* MovementBase = CharacterOwner->GetMovementBase();
			const AActor* BaseActor = MovementBase ? MovementBase->GetOwner() : NULL;
			const ECollisionChannel CollisionChannel = UpdatedComponent->GetCollisionObjectType();

			if (MovementBase != NULL)
			{
				MutableThis->bForceNextFloorCheck = !MovementBase->IsCollisionEnabled()
					|| MovementBase->GetCollisionResponseToChannel(CollisionChannel) != ECR_Block
					|| (MovementBase->Mobility == EComponentMobility::Movable)
					|| MovementBaseUtility::IsDynamicBase(MovementBase)
					|| (Cast<const ADestructibleActor>(BaseActor) != NULL);
			}

			const bool IsActorBasePendingKill = BaseActor && BaseActor->IsPendingKill();

			if (!bForceNextFloorCheck && !IsActorBasePendingKill && MovementBase)
			{
				//UE_LOG(LogCharacterMovement, Log, TEXT("%s SKIP check for floor"), *CharacterOwner->GetName());
				OutFloorResult = CurrentFloor;
				bNeedToValidateFloor = false;
			}
			else
			{
				MutableThis->bForceNextFloorCheck = false;
				ComputeFloorDist(CapsuleLocation, FloorLineTraceDist, FloorSweepTraceDist, OutFloorResult, CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleRadius(), DownwardSweepResult);
			}
		}
	}

	// OutFloorResult.HitResult is now the result of the vertical floor check.
	// See if we should try to "perch" at this location.
	if (bNeedToValidateFloor && OutFloorResult.bBlockingHit && !OutFloorResult.bLineTrace)
	{
		const bool bCheckRadius = true;
		if (ShouldComputePerchResult(OutFloorResult.HitResult, bCheckRadius))
		{
			float MaxPerchFloorDist = FMath::Max(MAX_FLOOR_DIST, _MaxStepHeight + HeightCheckAdjust);
			if (IsMovingOnGround())
			{
				MaxPerchFloorDist += FMath::Max(0.f, PerchAdditionalHeight);
			}

			FFindFloorResult PerchFloorResult;
			if (ComputePerchResult(GetValidPerchRadius(), OutFloorResult.HitResult, MaxPerchFloorDist, PerchFloorResult))
			{
				// Don't allow the floor distance adjustment to push us up too high, or we will move beyond the perch distance and fall next time.
				const float AvgFloorDist = (MIN_FLOOR_DIST + MAX_FLOOR_DIST) * 0.5f;
				const float MoveUpDist = (AvgFloorDist - OutFloorResult.FloorDist);
				if (MoveUpDist + PerchFloorResult.FloorDist >= MaxPerchFloorDist)
				{
					OutFloorResult.FloorDist = AvgFloorDist;
				}

				// If the regular capsule is on an unwalkable surface but the perched one would allow us to stand, override the normal to be one that is walkable.
				if (!OutFloorResult.bWalkableFloor)
				{
					OutFloorResult.SetFromLineTrace(PerchFloorResult.HitResult, OutFloorResult.FloorDist, FMath::Min(PerchFloorResult.FloorDist, PerchFloorResult.LineDist), true);
				}
			}
			else
			{
				// We had no floor (or an invalid one because it was unwalkable), and couldn't perch here, so invalidate floor (which will cause us to start falling).
				OutFloorResult.bWalkableFloor = false;
			}
		}
	}
}

void UPKCharacterMovementComponent::PhysWalking(float deltaTime, int32 Iterations)
{
	/*Super::PhysWalking(deltaTime, Iterations); return;*/

	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!CharacterOwner || (!CharacterOwner->Controller && !bRunPhysicsWithNoController && !HasRootMotion()))
	{
		Acceleration = FVector::ZeroVector;
		Velocity = FVector::ZeroVector;
		return;
	}

	if (!UpdatedComponent->IsCollisionEnabled())
	{
		SetMovementMode(MOVE_Walking);
		return;
	}

	checkf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN before Iteration (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Velocity.ToString());

	bJustTeleported = false;
	bool bCheckedFall = false;
	bool bTriedLedgeMove = false;
	float remainingTime = deltaTime;

	// Perform the move
	while ((remainingTime >= MIN_TICK_TIME) && (Iterations < MaxSimulationIterations) && CharacterOwner && (CharacterOwner->Controller || bRunPhysicsWithNoController || HasRootMotion()))
	{
		Iterations++;
		bJustTeleported = false;
		const float timeTick = GetSimulationTimeStep(remainingTime, Iterations);
		remainingTime -= timeTick;

		// Save current values
		UPrimitiveComponent * const OldBase = GetMovementBase();
		const FVector PreviousBaseLocation = (OldBase != NULL) ? OldBase->GetComponentLocation() : FVector::ZeroVector;
		const FVector OldLocation = UpdatedComponent->GetComponentLocation();
		const FFindFloorResult OldFloor = CurrentFloor;
		
		// Ensure velocity is horizontal.
		MaintainHorizontalGroundVelocity();
		Velocity.Z = 0.f;
		const FVector OldVelocity = Velocity;

		// Apply acceleration
		Acceleration.Z = 0.f;
		if (!HasRootMotion())
		{
			CalcVelocity(timeTick, GroundFriction, false, BrakingDecelerationWalking);
		}
		checkf(!Velocity.ContainsNaN(), TEXT("PhysWalking: Velocity contains NaN after CalcVelocity (%s: %s)\n%s"), *GetPathNameSafe(this), *GetPathNameSafe(GetOuter()), *Velocity.ToString());
		
		// Compute move parameters
		const FVector MoveVelocity = Velocity;
		const FVector Delta = timeTick * MoveVelocity;
		const bool bZeroDelta = Delta.IsNearlyZero();
		FStepDownResult StepDownResult;
		
		if (bZeroDelta)
		{
			remainingTime = 0.f;
		}
		else
		{
			// check if we hit the wall before trying to pull away from the slope
			FScopedMovementUpdate ScopedMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);
			FHitResult Hit(1.f);
			bool bMove = SafeMoveUpdatedComponent(Delta, CharacterOwner->GetActorRotation(), true, Hit);
			if (bMove) ScopedMovement.RevertMove();

			if (!Hit.IsValidBlockingHit() && !bSlideAlongSlope && (Acceleration.GetSafeNormal2D() | CurrentFloor.HitResult.ImpactNormal) > 0.42f)
			{
				// step off the slope ~> 25 deg
				SetMovementMode(MOVE_Falling);
			}
			else{
				// try to move forward
				MoveAlongFloor(MoveVelocity, timeTick, &StepDownResult);
			}

			if (IsFalling())
			{
				// pawn decided to jump up
				const float DesiredDist = Delta.Size();
				if (DesiredDist > KINDA_SMALL_NUMBER)
				{
					const float ActualDist = (CharacterOwner->GetActorLocation() - OldLocation).Size2D();
					remainingTime += timeTick * (1.f - FMath::Min(1.f, ActualDist / DesiredDist));
				}
				StartNewPhysics(remainingTime, Iterations);
				return;
			}
			else if (IsSwimming()) //just entered water
			{
				StartSwimming(OldLocation, OldVelocity, timeTick, remainingTime, Iterations);
				return;
			}
		}

		// Update floor.
		// StepUp might have already done it for us.
		if (StepDownResult.bComputedFloor)
		{
			CurrentFloor = StepDownResult.FloorResult;
		}
		else
		{
			FindFloor(UpdatedComponent->GetComponentLocation(), CurrentFloor, bZeroDelta, NULL);
			
			// 'ski jumping'
			if ((CurrentFloor.HitResult.ImpactNormal | Acceleration.GetSafeNormal2D()) > -0.7071 &&
				(OldFloor.HitResult.ImpactNormal | CurrentFloor.HitResult.ImpactNormal) < 0.98)
			{
				Velocity = 1/1.5 * (CharacterOwner->GetActorLocation() - OldLocation) / timeTick;
				if (CharacterOwner->IsLocallyControlled())
				{
					// reduce speed
					if (bRocketJump)
						SetMovementFlags(1, 1, 0, bWantsToStop);
					else
					{
						uint8 rate = FMath::Max((int)((bSpeedBit10 << 1 | bSpeedBit01 << 0) - 1), 0);
						SetMovementFlags((0x02 & rate) != 0, (0x01 & rate) != 0, 0, bWantsToStop);
					}
				}
				SetMovementMode(MOVE_Falling);

				StartNewPhysics(remainingTime, Iterations);
				return;
			}
		}

		// check for ledges here
		const bool bCheckLedges = !CanWalkOffLedges();
		if (bCheckLedges && !CurrentFloor.IsWalkableFloor())
		{
			// calculate possible alternate movement
			const FVector GravDir = FVector(0.f, 0.f, -1.f);
			const FVector NewDelta = bTriedLedgeMove ? FVector::ZeroVector : GetLedgeMove(OldLocation, Delta, GravDir);
			if (!NewDelta.IsZero())
			{
				// first revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, false);

				// avoid repeated ledge moves if the first one fails
				bTriedLedgeMove = true;

				// Try new movement direction
				Velocity = NewDelta / timeTick;
				remainingTime += timeTick;
				continue;
			}
			else
			{
				// see if it is OK to jump
				// @todo collision : only thing that can be problem is that oldbase has world collision on
				bool bMustJump = bZeroDelta || (OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;

				// revert this move
				RevertMove(OldLocation, OldBase, PreviousBaseLocation, OldFloor, true);
				remainingTime = 0.f;
				break;
			}
		}
		else
		{
			// Validate the floor check
			if (CurrentFloor.IsWalkableFloor())
			{
				if (ShouldCatchAir(OldFloor, CurrentFloor))
				{
					CharacterOwner->OnWalkingOffLedge();
					if (IsMovingOnGround())
					{
						// If still walking, then fall. If not, assume the user set a different mode they want to keep.
						StartFalling(Iterations, remainingTime, timeTick, Delta, OldLocation);
					}
					return;
				}

				AdjustFloorHeight();
				SetBase(CurrentFloor.HitResult.Component.Get(), CurrentFloor.HitResult.BoneName);
			}
			else if (CurrentFloor.HitResult.bStartPenetrating && remainingTime <= 0.f)
			{
				// The floor check failed because it started in penetration
				// We do not want to try to move downward because the downward sweep failed, rather we'd like to try to pop out of the floor.
				FHitResult Hit(CurrentFloor.HitResult);
				Hit.TraceEnd = Hit.TraceStart + FVector(0.f, 0.f, MAX_FLOOR_DIST);
				const FVector RequestedAdjustment = GetPenetrationAdjustment(Hit);
				ResolvePenetration(RequestedAdjustment, Hit, CharacterOwner->GetActorRotation());
			}

			// check if just entered water
			if (IsSwimming())
			{
				StartSwimming(OldLocation, Velocity, timeTick, remainingTime, Iterations);
				return;
			}

			// See if we need to start falling.
			if (!CurrentFloor.IsWalkableFloor() && !CurrentFloor.HitResult.bStartPenetrating)
			{
				const bool bMustJump = bJustTeleported || bZeroDelta || (OldBase == NULL || (!OldBase->IsCollisionEnabled() && MovementBaseUtility::IsDynamicBase(OldBase)));
				if ((bMustJump || !bCheckedFall) && CheckFall(CurrentFloor.HitResult, Delta, OldLocation, remainingTime, timeTick, Iterations, bMustJump))
				{
					return;
				}
				bCheckedFall = true;
			}
		}


		// Allow overlap events and such to change physics state and velocity
		if (IsMovingOnGround())
		{
			// Make velocity reflect actual move
			if (!bJustTeleported && !HasRootMotion() && timeTick >= MIN_TICK_TIME)
			{
				Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / timeTick;
			}
		}

		// If we didn't move at all this iteration then abort (since future iterations will also be stuck).
		if (CharacterOwner->GetActorLocation() == OldLocation)
		{
			remainingTime = 0.f;
			break;
		}
	}

	if (IsMovingOnGround())
	{
		MaintainHorizontalGroundVelocity();
	}
}

void UPKCharacterMovementComponent::HandleImpact(FHitResult const& Hit, float TimeSlice, const FVector& MoveDelta)
{
	Super::HandleImpact(Hit, TimeSlice, MoveDelta);

	if (IsFalling() && CharacterOwner->IsLocallyControlled())
	{
		if (bJumpApex)
		{
			bJumpApex = false;
			ClimbOn(Hit, MoveDelta);
		}
	}

	if ((Velocity.GetSafeNormal2D() | Hit.ImpactNormal) < -0.98f) SetMovementFlags(0, 0, 0, bWantsToStop);
}

void UPKCharacterMovementComponent::ClimbOn(FHitResult const& InHit, const FVector& Delta)
{
	const FVector GravDir = FVector(0.f, 0.f, -1.f);

	const FVector OldLocation = UpdatedComponent->GetComponentLocation();
	float PawnRadius, PawnHalfHeight;
	CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleSize(PawnRadius, PawnHalfHeight);

	// Don't bother stepping up if top of capsule is hitting something.
	const float InitialImpactZ = InHit.ImpactPoint.Z;
	if (InitialImpactZ > OldLocation.Z + (PawnHalfHeight - PawnRadius))
	{
		return;
	}

	// Scope our movement updates, and do not apply them until all intermediate moves are completed.
	FScopedMovementUpdate ScopedStepUpMovement(UpdatedComponent, EScopedUpdate::DeferredUpdates);

	// step up - treat as vertical wall
	FHitResult SweepUpHit(1.f);
	const FRotator PawnRotation = CharacterOwner->GetActorRotation();
	SafeMoveUpdatedComponent(-GravDir * PawnHalfHeight, PawnRotation, true, SweepUpHit);
	if (SweepUpHit.bBlockingHit)
	{
		return;
	}

	// step fwd
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Delta, PawnRotation, true, Hit);

	// Check result of forward movement
	if (Hit.bBlockingHit)
	{
		if (Hit.bStartPenetrating)
		{
			// Undo movement
			ScopedStepUpMovement.RevertMove();
			return;
		}

		// adjust and try again
		const float ForwardHitTime = Hit.Time;
		const float ForwardSlideAmount = SlideAlongSurface(Delta, 1.f - Hit.Time, Hit.Normal, Hit, true);

		// If both the forward hit and the deflection got us nowhere, there is no point in this step up.
		if (ForwardHitTime == 0.f && ForwardSlideAmount == 0.f)
		{
			ScopedStepUpMovement.RevertMove();
			return;
		}
	}

	// Step down
	SafeMoveUpdatedComponent(GravDir * PawnHalfHeight, CharacterOwner->GetActorRotation(), true, Hit);

	// If step down was initially penetrating abort the step up
	if (Hit.bStartPenetrating)
	{
		ScopedStepUpMovement.RevertMove();
		return;
	}

	if (Hit.IsValidBlockingHit())
	{
		const float DeltaZ = UpdatedComponent->GetComponentLocation().Z - Hit.ImpactPoint.Z;

		if (PawnHalfHeight > DeltaZ)
		{
			ReceivedImpulse += FVector(0.f, 0.f, FMath::Sqrt(-GetGravityZ() * DeltaZ));
		}
	}
	
	ScopedStepUpMovement.RevertMove();
}

void UPKCharacterMovementComponent::NotifyJumpApex()
{
	Super::NotifyJumpApex();

	bJumpApex = true;
}

float UPKCharacterMovementComponent::GetMaxSpeed() const
{
	float MaxSlideSpeed = 80.f;
	
	float SpeedStep = (MaxBunnyHopVelocity / BaseWalkSpeed - 1) / 3; // 0.424242...f
	float MaxWalkSpeed = bRocketJump ?
	MaxRocketJumpVelocity : BaseWalkSpeed * (1.0f + (bSpeedBit10 << 1 | bSpeedBit01 << 0) * SpeedStep);

	switch (MovementMode)
	{
	case MOVE_Walking:
		return IsCrouching() ? MaxWalkSpeedCrouched : MaxWalkSpeed;
	case MOVE_Falling:
		return MaxWalkSpeed;
	case MOVE_Swimming:
		return MaxSwimSpeed;
	case MOVE_Flying:
		return MaxFlySpeed;
	case MOVE_Custom:
		return MaxCustomMovementSpeed;
	case MOVE_None:
	default:
		return 0.f;
	}
}

void UPKCharacterMovementComponent::PhysFalling(float deltaTime, int32 Iterations)
{
	FallingLateralFriction = bWantsToStop ? MaxFallingLateralFriction : BaseFallingLateralFriction;
	Super::PhysFalling(deltaTime, Iterations);
}

void UPKCharacterMovementComponent::PhysFlying(float deltaTime, int32 Iterations)
{
	if (deltaTime < MIN_TICK_TIME)
	{
		return;
	}

	if (!HasRootMotion())
	{
		if (Acceleration.IsZero())
		{
			Velocity = FVector::ZeroVector;
		}
		const float Friction = 0.5f * GetPhysicsVolume()->FluidFriction;
		CalcVelocity(deltaTime, Friction, true, BrakingDecelerationFlying);
	}

	Iterations++;
	bJustTeleported = false;

	FVector OldLocation = CharacterOwner->GetActorLocation();
	const FVector Adjusted = Velocity * deltaTime;
	FHitResult Hit(1.f);
	SafeMoveUpdatedComponent(Adjusted, CharacterOwner->GetActorRotation(), true, Hit);

	if (Hit.Time < 1.f && CharacterOwner)
	{
		const FVector GravDir = FVector(0.f, 0.f, -1.f);
		const FVector VelDir = Velocity.GetSafeNormal();
		const float UpDown = GravDir | VelDir;

		bool bSteppedUp = false;
		if ((FMath::Abs(Hit.ImpactNormal.Z) < 0.2f) && (UpDown < 0.5f) && (UpDown > -0.2f) && CanStepUp(Hit))
		{
			float stepZ = CharacterOwner->GetActorLocation().Z;
			bSteppedUp = StepUp(GravDir, Adjusted * (1.f - Hit.Time), Hit);
			if (bSteppedUp)
			{
				OldLocation.Z = CharacterOwner->GetActorLocation().Z + (OldLocation.Z - stepZ);
			}
		}

		if (!bSteppedUp)
		{
			//adjust and try again
			HandleImpact(Hit, deltaTime, Adjusted);
			SlideAlongSurface(Adjusted, (1.f - Hit.Time), Hit.Normal, Hit, true);
		}
	}

	if (!bJustTeleported && !HasRootMotion() && CharacterOwner)
	{
		Velocity = (CharacterOwner->GetActorLocation() - OldLocation) / deltaTime;
	}
}

void UPKCharacterMovementComponent::StartFalling(int32 Iterations, float remainingTime, float timeTick, const FVector& Delta, const FVector& subLoc)
{
	Super::StartFalling(Iterations, remainingTime, timeTick, Delta, subLoc);
}

FVector LastVel2D = FVector::ZeroVector;
bool UPKCharacterMovementComponent::DoJump(bool bReplayingMoves)
{
	if (Super::DoJump(bReplayingMoves))
	{	
		if (CharacterOwner->IsLocallyControlled()){
			if (!bWantsToStop) {
				TimeOutAfterLanding = SecondsWhenYouCanBunnyHopAfterLanding;				
				if (!(bSpeedBit10 && bSpeedBit01))
				{
					if (Velocity.Size2D() < BaseWalkSpeed / 2) BunnyHopCount = 0;

					bSpeedBit01 = (0x01 & BunnyHopCount) != 0;
					bSpeedBit10 = (0x02 & BunnyHopCount) != 0;
					
					BunnyHopCount = 0x03 & (bSpeedBit10 << 1 | bSpeedBit01 << 0) + 1;					
				}
			}
			bNotifyApex = true;
		}
		return true;
	}
	return false;
}

void UPKCharacterMovementComponent::PerformMovement(float DeltaTime)
{
	Super::PerformMovement(DeltaTime);
}

void UPKCharacterMovementComponent::CalcVelocity(float DeltaTime, float Friction, bool bFluid, float BrakingDeceleration)
{
	Super::CalcVelocity(DeltaTime, Friction, bFluid, BrakingDeceleration);
	
	// Slide down the slope
	if (CurrentFloor.bBlockingHit)
	{
		const FVector SlideDir = CurrentFloor.HitResult.ImpactNormal.GetSafeNormal2D();
		FVector RampVector = ComputeGroundMovementDelta(SlideDir, CurrentFloor.HitResult, CurrentFloor.bLineTrace);
		float Dot = RampVector | FVector(0, 0, -1);
		FVector SlideVel = SlideDir * Dot * -GetGravityZ() * 1.5 * DeltaTime;

		bSlideAlongSlope = FVector::Coincident(SlideVel.GetSafeNormal2D(), SlideDir) && Acceleration.IsZero();
		if (bSlideAlongSlope) Velocity += SlideVel;
	}
}

//NEW FUNCs

void UPKCharacterMovementComponent::MovementControl(float DeltaTime)
{
	if (!(CharacterOwner && CharacterOwner->IsLocallyControlled())) return;
	
	FVector FWD = CharacterOwner->GetActorForwardVector() * PendingForward;
	FVector RGT = CharacterOwner->GetActorRightVector() * PendingRight;	
	const FVector InputVector = (FWD + RGT).GetSafeNormal();

	// avoid stray movement after landing when stopped
	if (bWantsToStop && !IsFlying()){ FWD = FVector::ZeroVector; RGT = FVector::ZeroVector; }

	// check if we should start braking while bunnyhopping
	if (!bSlideAlongSlope && (Velocity.GetSafeNormal2D() | InputVector) < -0.7071f) SetMovementFlags(0, 0, 0, 1);
	
	if (IsWalking()) {
		bJumpApex = false;
		PendingForward = 0.f; PendingRight = 0.f;

		// update bWantsToStop when dodging
		bWantsToStop = !bSlideAlongSlope && (Velocity.GetSafeNormal2D() | InputVector) < -0.7071f; // FMath::Cos(135.f * PI / 180.f);

		if (Velocity.Size2D() < KINDA_SMALL_NUMBER/*Acceleration.IsZero()*/ || TimeOutAfterLanding <= 0)
		{
			SetMovementFlags(0, 0, 0, 0);
		}
		if (TimeOutAfterLanding > 0) TimeOutAfterLanding -= DeltaTime;
	}
	// apply input
	if (bWantsToStop && !IsFlying()){ FWD = FVector::ZeroVector; RGT = FVector::ZeroVector; }
	ReceiveMinigunHit(DeltaTime);
	
	if (IsFlying()){
		FRotator SpawnRotation = CharacterOwner->GetControlRotation();
		FVector FV = SpawnRotation.RotateVector(FVector::ForwardVector);
		FV *= PendingForward;

		AddInputVector(FV, 0);
		AddInputVector(RGT, 0);

		PendingForward = 0.f; PendingRight = 0.f;
	}
	else
	{
		if (bMoveInput)
		{
			AddInputVector(FWD, 0); AddInputVector(RGT, 0);
		}
	}
}

void UPKCharacterMovementComponent::Jump()
{
	float delay = JumpZVelocity / -GetGravityZ();
	GetWorld()->GetTimerManager().SetTimer(ClearJumpTimerHandle, ClearJumpTimerDelegate, delay, false);

	CharacterOwner->Jump();
}

float LastValue = 0.f;

void UPKCharacterMovementComponent::MoveForward(float Value)
{
	SetDirection(PendingForward, PendingRight, Value);
}

void UPKCharacterMovementComponent::MoveRight(float Value)
{
	SetDirection(PendingRight, PendingForward, Value);
}

void UPKCharacterMovementComponent::SetDirection(float &Forth, float &Across, float Value)
{
	if (Value != 0.f)
	{
		if (IsFalling()) {
			if (Forth == 0.f && Across == 0.f) return;
			if (Forth == -Value) bWantsToStop = true;
			if (Across == 0.f || (Forth != 0.f && Across != 0.f)) Forth = Value;
		}
		else Forth = Value;

		bMoveInput = (Forth != 0.f);
	}

	if (!Value && !LastValue) bMoveInput = false;
	LastValue = Value;
}

void UPKCharacterMovementComponent::ScheduleRocketJump(TSubclassOf<class APKProjectile>  ProjectileCls)
{
	ANetPlayer* PLAYER = Cast<ANetPlayer>(CharacterOwner);

	const FRotator SpawnRotation = PLAYER->GetControlRotation();
	FVector Loc = PLAYER->GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector SpawnLocation = Loc + SpawnRotation.RotateVector(PLAYER->GunOffset);

	const FVector End = SpawnLocation + ExplosionRange * 1.5f * SpawnRotation.RotateVector(FVector::ForwardVector);

	FHitResult RocketHit = FHitResult(1.f);
	FCollisionQueryParams TraceParams = FCollisionQueryParams(FName(TEXT("Trace")), true, PLAYER);

	GetWorld()->SweepSingleByProfile(RocketHit, SpawnLocation, End, FQuat(), TEXT("Spectator"), FCollisionShape::MakeSphere(10.f), TraceParams);
	
	// check if we are facing the wall
	FHitResult WallHit = FHitResult(1.f);
	FVector Rebound = SpawnRotation.RotateVector(FVector::ForwardVector);
	const FVector TraceStart = UpdatedComponent->GetComponentLocation();
	const FVector TraceEnd = TraceStart + (CharacterOwner->GetActorForwardVector() * ExplosionRange * (CharacterOwner->GetActorForwardVector() | Rebound));
	GetWorld()->LineTraceSingleByProfile(WallHit, TraceStart, TraceEnd, TEXT("Spectator"), TraceParams);
	
	if (RocketHit.bBlockingHit){
		float ProjectileSpeed = ProjectileCls.GetDefaultObject()->GetProjectileMovement()->MaxSpeed;
		float RocketJumpDelay = (RocketHit.ImpactPoint - UpdatedComponent->GetComponentLocation()).Size() / ProjectileSpeed;

		RocketJumpTimerDelegate.BindUFunction(this, FName("ApplyRocketJump"), RocketHit, WallHit, -Rebound);
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, RocketJumpTimerDelegate, RocketJumpDelay, false);
	}
}

void UPKCharacterMovementComponent::ApplyRocketJump(FHitResult RocketHit, FHitResult WallHit, FVector Rebound)
{
	FVector RocketJumpImpulse = ARocket::GetExplosionVelocity(RocketHit.ImpactPoint, this);

	if (!RocketJumpImpulse.IsNearlyZero()) {
		if (WallHit.bBlockingHit && (FMath::Abs(RocketHit.ImpactNormal.Z) < 0.26f)) // Z <~ ±15 degrees
		{
			if (bMoveInput) SetMovementFlags(1, 1, 1, 0);
			else SetMovementFlags(0, 1, 0, 0);
			RocketJumpImpulse = Rebound * GetMaxSpeed() * (1 - (WallHit.ImpactPoint - UpdatedComponent->GetComponentLocation()).Size() / (WallHit.TraceStart - WallHit.TraceEnd).Size());
		}
		else if ((CharacterOwner->GetActorForwardVector().GetSafeNormal2D() | RocketJumpImpulse.GetSafeNormal2D()) > -0.7071f)
		{
			SetMovementFlags(bSpeedBit10, bSpeedBit01, bSpeedBit10 || bSpeedBit01, 0);
		}
		ReceiveImpact(RocketJumpImpulse);
	}
}

FVector UPKCharacterMovementComponent::GetAverageHitDirection(uint16 AttackingEnemyFlags)
{
	FVector Average = FVector::ZeroVector;

	APKGameState* GameState = Cast<APKGameState>(GetWorld()->GetGameState());

	for (uint8 idx = 0; idx < 16; idx++) // 16 players max
	{
		uint16 Flag = (0x0001 << idx);
		if ((AttackingEnemyFlags & Flag) == 0) continue;
		ANetPlayer* Pawn = GameState->GetPlayerByFlag(Flag);
		if (Pawn) Average += Pawn->GetBaseAimRotation().RotateVector(FVector::ForwardVector);
	}
	
	return Average;
}

void UPKCharacterMovementComponent::ReceiveMinigunHit(/*FVector &FWD, FVector &RGT*/float DeltaTime)
{
	ANetPlayer* Me = Cast<ANetPlayer>(CharacterOwner);
	uint16 AttackingEnemyFlags = Me->MinigunAttackingPlayerFlags;
	if (AttackingEnemyFlags > 0)
	{
		ReceiveImpact(1.5f * -GetGravityZ() * GetAverageHitDirection(AttackingEnemyFlags) * DeltaTime);
	}
}

void UPKCharacterMovementComponent::SlideAlongSlope(float DeltaTime)
{
	const FVector SlideDir = CurrentFloor.HitResult.ImpactNormal.GetSafeNormal2D();
	FVector RampVector = ComputeGroundMovementDelta(SlideDir, CurrentFloor.HitResult, CurrentFloor.bLineTrace);

	float Dot = RampVector | FVector(0, 0, -1);
	FVector SlideForce = RampVector * Dot * FMath::Pow(-GetGravityZ(), 2.2) * DeltaTime;
	AddForce(SlideForce);

	bSlideAlongSlope = FVector::Coincident(Velocity.GetSafeNormal(), SlideDir) && Acceleration.IsZero();
}

void UPKCharacterMovementComponent::OnRep_SlideAlongSlope()
{
}

void UPKCharacterMovementComponent::HandleJumpPad(float JumpStrength)
{
	Velocity.Z = JumpStrength;
	SetMovementMode(MOVE_Falling);

	if (CharacterOwner && CharacterOwner->IsLocallyControlled() && Velocity.Size2D() < 10.f/*KINDA_SMALL_NUMBER*/)
	{
		FVector SideImpulse = FVector(FMath::RandRange(-600, 600), FMath::RandRange(-600, 600), 0.f);
		PendingForward = 0.f; PendingRight = 0.f;
		SetMovementFlags(0, 0, 0, 0);
		ReceiveImpact(SideImpulse);
	}
}

/************************************************************
	Replication List
***********************************************************/
void UPKCharacterMovementComponent::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// Replicate to everyone

	// Conditional Replication
	DOREPLIFETIME_CONDITION(UPKCharacterMovementComponent, bSlideAlongSlope, COND_SimulatedOnly);
}