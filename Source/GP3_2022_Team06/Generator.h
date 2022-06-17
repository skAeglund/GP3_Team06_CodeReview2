// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextRenderActor.h"
#include "GameFramework/Actor.h"
#include "Player/Pickup/PickupComponent.h"
#include "Generator.generated.h"
DECLARE_DELEGATE_OneParam(GeneratorDelegate, int id);
UCLASS()
class GP3_2022_TEAM06_API AGenerator : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AGenerator();
	UFUNCTION(BlueprintNativeEvent)
	void AudioEvent();
	GeneratorDelegate EnterGenArea;
	GeneratorDelegate ExitGenArea; // currently unused, here in case we want to be able to close doors

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* InteractArea;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	APickup* KeyActor; // This is needed, otherwise causes issues

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	int SentID;

protected:
	virtual void BeginPlay() override;
	virtual void KeyDropped();
	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);
	virtual void AnimUpdate();

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UStaticMeshComponent* GeneratorMesh;

	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	UTextRenderComponent* TextBox;
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	USceneComponent* AnimationEndPos;

	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.1f, ClampMax = 10, UIMin = 0.1f, UIMax = 10))
	float AnimationTime = 0.5f;
	UPROPERTY(EditAnywhere, meta = (ClampMin = 0.001f, ClampMax = 0.5f, UIMin = 0.01f, UIMax = 0.5f))
	float AnimationUpdateRate = 0.001f;
	
	APawn* PlayerPawn;
	FTimerHandle AnimationTimer;
	FRotator StartRotation;
	FRotator TargetRotation;
	float TimerTimeElapsed;
	bool bKeyConfirmed;
	bool bKeyEntered;
	UInputComponent* InputComponent;
	UPickupComponent* PickupComponent;
	FVector AnimationStartPos;
	FVector AnimationCurrentPos;
	APickup* LastKey;
};
