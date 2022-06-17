#include "DashComponent.h"

#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "PhysicsEngine/PhysicsSettings.h"

// DEBUGGING 
float AverageDeltaTime = 0;
float AverageFPS = 0;
UDashComponent::UDashComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UDashComponent::BeginPlay()
{
	Super::BeginPlay();
	Owner = GetOwner();
	MoveComp = Cast<ACharacter>(Owner)->GetCharacterMovement();
	Collider = Cast<UCapsuleComponent>(Owner->GetComponentByClass(UCapsuleComponent::StaticClass()));
	CameraComponent = Cast<USceneComponent>(Owner->GetComponentByClass(UCameraComponent::StaticClass()));
	if (DashMovementCurve == nullptr || DoubleJumpCurve == nullptr)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 100, FColor::Red, TEXT(
			"Error: Dash component is missing reference to movement curve"));
	}
	if (!UPhysicsSettings::Get()->bSubstepping)
	{
		GEngine->AddOnScreenDebugMessage(INDEX_NONE, 100, FColor::Red, TEXT(
			"Error: Physics sub-stepping needs to be enabled in settings"));
	}
}

void UDashComponent::SubStepDash(FPhysScene_PhysX* PhysScene, float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Red, FString::Printf(TEXT(
	//	"inside dash sub-step: Sub-Delta: %f "), DeltaTime));
	if (bIsDashing)
	{
		ElapsedDashTime += DeltaTime;
		DashUpdate();
		MoveComp->MoveSmooth(MoveComp->Velocity, DeltaTime);
	}
}
void UDashComponent::SubStepJump(FPhysScene_PhysX* PhysScene, float DeltaTime)
{
	//GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Red, FString::Printf(TEXT(
	//	"inside jump sub-step: Sub-Delta: %f "), DeltaTime));
	if (bIsDoubleJumping)
	{
		ElapsedJumpTime += DeltaTime;
		DoubleJumpUpdate();

		if (!bIsDashing)
			MoveComp->MoveSmooth(MoveComp->Velocity, DeltaTime);
	}
}
void UDashComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	AverageDeltaTime += ((DeltaTime / Owner->GetActorTimeDilation()) - AverageDeltaTime) * 0.25f;
	AverageFPS = 1 / AverageDeltaTime;
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	GEngine->AddOnScreenDebugMessage(INDEX_NONE, 0, FColor::Emerald, FString::Printf(TEXT(
		"Average FPS: %f "), AverageFPS));

	if (bIsOnCooldown && !bIsDashing)
	{
		TimeLeftOnCooldown -= DeltaTime * (MoveComp->IsMovingOnGround() ? 3 : 1);
		if (TimeLeftOnCooldown <= 0.f)
		{
			bIsOnCooldown = false;
			TimeLeftOnCooldown = 0;
		}
	}
	if (bIsDashing || bIsDoubleJumping)
	{
		float TotalProgress = FMath::Clamp(DashProgress + DoubleJumpProgress, 0.f, 1.f);
		TransitionProgress = UKismetMathLibrary::Ease(0.f, 1.f, TotalProgress, EEasingFunc::EaseOut, 12);
		UpdatePostProcess(TransitionProgress);
	}
	else if (TransitionProgress > 0.f)
	{
		TransitionProgress = FMath::Clamp(TransitionProgress - (DeltaTime * 1), 0.f, 1.f);
		float EasedTransition = UKismetMathLibrary::Ease(0.f, 1.f, TransitionProgress, EEasingFunc::EaseIn, 4);
		UpdatePostProcess(EasedTransition);
	}
	if (!bDoubleJumpAvailable && MoveComp->IsMovingOnGround())
	{
		bDoubleJumpAvailable = true;
	}
}
void UDashComponent::StartDashing(FVector DashDirection)
{
	if (bIsOnCooldown) return;

	if (bIsDoubleJumping) StopDoubleJump();

	CurrentDashDirection = DashDirection;
	DashProgress = 0.f;
	ElapsedDashTime = 0.f;
	bIsDashing = true;
	Owner->GetWorldTimerManager().UnPauseTimer(DashTimer);
	TimeLeftOnCooldown = CooldownDuration;
	bIsOnCooldown = true;
	OnDashStart();
	
	// adds function call to sub-stepping of scene tick
	if (FPhysScene* PhysScene = GetWorld()->GetPhysicsScene())
			DashSubstepHandle = PhysScene->OnPhysSceneStep.AddUObject(this, &UDashComponent::SubStepDash);

	MoveComp->SetComponentTickEnabled(false);

	if (MoveComp->Velocity.Z > 0)
		MoveComp->Velocity.Z = 0;
}

void UDashComponent::StopDashing()
{
	if (!bIsDashing) return;
	bIsDashing = false;
	MoveComp->SetComponentTickEnabled(true); // TODO: Add conditions: Isdoublejumping? isglitching?
	FActorComponentTickFunction CustomTick;
	CustomTick.Target = this;
	MoveComp->TickComponent(0.0001f, LEVELTICK_All, &CustomTick); // to update groundcheck
	if (!MoveComp->IsMovingOnGround())
	{
	 MoveComp->Velocity *= ExitDashVelocityMultiplier;
	}

	// adds function call to sub-stepping of scene tick
	if (FPhysScene* PhysScene = GetWorld()->GetPhysicsScene())
		if (DashSubstepHandle.IsValid() && PhysScene->OnPhysSceneStep.IsBoundToObject(this))
			PhysScene->OnPhysSceneStep.Remove(DashSubstepHandle);


	
	OnDashEnd();
}

void UDashComponent::DashUpdate()
{
	if (!bIsDashing || ElapsedDashTime > (DashDuration * DurationMultiplier) || !IsComponentTickEnabled())
	{
		StopDashing();
		return;
	}
	float Alpha = FMath::Clamp(UKismetMathLibrary::NormalizeToRange(AverageFPS, 15, 120), 0.f, 1.f);;

	float FPSMultiplier = UKismetMathLibrary::Ease(0.974f, 1.f, Alpha, EEasingFunc::ExpoOut, 2);
	DashProgress = ElapsedDashTime / (DashDuration * DashLengthMultiplier);
	float EasedAlpha = DashMovementCurve->GetFloatValue(DashProgress);
	float ZVelocity = MoveComp->Velocity.Z;

	FVector DashingVelocity = ((CurrentDashDirection * FPSMultiplier) * (DashSpeed * DashLengthMultiplier) * EasedAlpha);
	FVector NewVelocity = FVector(DashingVelocity.X, DashingVelocity.Y, ZVelocity);
	MoveComp->Velocity = NewVelocity;
	
}

void UDashComponent::StartDoubleJump()
{
	if (!bDoubleJumpAvailable) return;
	if (bIsDashing) StopDashing();

	bIsDoubleJumping = true;
	bDoubleJumpAvailable = false;
	//Owner->CustomTimeDilation = 1.f;
	ElapsedJumpTime = 0.f;
	DoubleJumpProgress = 0.f;
	MoveComp->Velocity *= JumpKeepVelocity;
	if (FPhysScene* PhysScene = GetWorld()->GetPhysicsScene())
		JumpSubstepHandle = PhysScene->OnPhysSceneStep.AddUObject(this, &UDashComponent::SubStepJump);

	OnDoubleJumpStart();
}

void UDashComponent::StopDoubleJump()
{
	if (!bIsDoubleJumping) return;

	Owner->GetWorldTimerManager().PauseTimer(DoubleJumpTimer);
	bIsDoubleJumping = false;

	if (FPhysScene* PhysScene = GetWorld()->GetPhysicsScene())
		if (JumpSubstepHandle.IsValid() && PhysScene->OnPhysSceneStep.IsBoundToObject(this))
			PhysScene->OnPhysSceneStep.Remove(JumpSubstepHandle);

	OnDoubleJumpEnd();	
}

void UDashComponent::DoubleJumpUpdate()
{
	if (ElapsedJumpTime > (DoubleJumpDuration * JumpLengthMultiplier) || !IsComponentTickEnabled())
	{
		StopDoubleJump();
		return;
	}
	if (MoveComp->IsMovingOnGround())
		MoveComp->DoJump(false);

	DoubleJumpProgress = ElapsedJumpTime / (DoubleJumpDuration * JumpLengthMultiplier);
	float EasedAlpha = DoubleJumpCurve->GetFloatValue(DoubleJumpProgress);
	FVector PreVelocity = MoveComp->Velocity;
	FVector NewVelocity = FVector(PreVelocity.X, PreVelocity.Y, (JumpForce *JumpLengthMultiplier) * EasedAlpha);
	MoveComp->Velocity = NewVelocity;
}


void UDashComponent::OnDashEnd_Implementation()
{
	// BP event
}
void UDashComponent::OnDashStart_Implementation()
{
	// BP event
}
void UDashComponent::OnDoubleJumpStart_Implementation()
{
	// BP event
}

void UDashComponent::OnDoubleJumpEnd_Implementation()
{
	// BP event
}
void UDashComponent::UpdatePostProcess_Implementation(float Value)
{
	// BP event
}