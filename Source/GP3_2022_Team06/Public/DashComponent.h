#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DashComponent.generated.h"


class UCapsuleComponent;
class UCharacterMovementComponent;
UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class GP3_2022_TEAM06_API UDashComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UDashComponent();
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	UFUNCTION(BlueprintCallable)
	void StartDashing(FVector DashDirection);
	UFUNCTION(BlueprintCallable)
	void StopDashing();
	UFUNCTION(BlueprintCallable)
	void StartDoubleJump();
	UFUNCTION(BlueprintCallable)
	void StopDoubleJump();

protected:
	virtual void BeginPlay() override;
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnDashStart();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnDashEnd();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnDoubleJumpStart();
	UFUNCTION(BlueprintNativeEvent, Category = "Events")
	void OnDoubleJumpEnd();
	UFUNCTION(BlueprintNativeEvent)
	void UpdatePostProcess(float Value);


private:
	void DashUpdate();
	void DoubleJumpUpdate();

	
	void SubStepDash(FPhysScene_PhysX* PhysScene, float DeltaTime);
	void SubStepJump(FPhysScene_PhysX* PhysScene, float DeltaTime);

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsDashing = false;
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
		bool bIsDoubleJumping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cooldown")
		bool bIsOnCooldown = false;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Cooldown")
		float CooldownDuration = 1.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Cooldown")
		float TimeLeftOnCooldown = 0.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "Cooldown")
		bool bDoubleJumpAvailable = true;


	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.875f, ClampMax = 1.1f, UIMin = 0.875f, UIMax = 1.1f), Category = "Dash")
		float DashLengthMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Dash")
		float ExitDashVelocityMultiplier = 0.4f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.5f, ClampMax = 2.f, UIMin = 0.5f, UIMax = 2.f), Category = "Double jump")
		float JumpLengthMultiplier = 1.f;
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 0.f, ClampMax = 1.f, UIMin = 0.f, UIMax = 1.f), Category = "Double jump")
		float JumpKeepVelocity = 0.5f;


protected:
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DashMovementCurve;
	UPROPERTY(EditDefaultsOnly)
	UCurveFloat* DoubleJumpCurve;

	UPROPERTY(BlueprintReadOnly)
	UCapsuleComponent* Collider;
	UPROPERTY(BlueprintReadOnly)
	USceneComponent* CameraComponent;

	float DashSpeed = 3000; // 3000
	float DashDuration = 0.3f;
	float JumpForce = 2000; // 2500
	float DoubleJumpDuration = 0.15f;
	float TransitionProgress = 0.f;
	float GroundedCooldownMultiplier = 0.5f;

private:
	UPROPERTY()
		AActor* Owner;
	UCharacterMovementComponent* MoveComp;
	FTimerHandle DashTimer;
	FTimerHandle DoubleJumpTimer;
	FVector CurrentDashDirection;
	float LastDashTickTime = 0.f;
	float DashTickRate = 0.00833f;
	float ElapsedDashTime = 0.f;
	float DashProgress = 0.f;
	float ElapsedJumpTime = 0.f;
	float DoubleJumpProgress = 0.f;
	float DurationMultiplier = 1.f;
	float SpeedMultiplier = 1.f;

	FDelegateHandle DashSubstepHandle;
	FDelegateHandle JumpSubstepHandle;

	FActorComponentTickFunction* SubstepTickFunction;
};
