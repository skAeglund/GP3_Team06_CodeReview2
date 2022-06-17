// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "VideoActor.generated.h"

class UBoxComponent;
class USceneComponent;

UENUM()
enum VideoStatus
{
	NotStarted		UMETA(DisplayName = "NotStarted"),
	Playing			UMETA(DisplayName = "Playing"),
	Done			UMETA(DisplayName = "Done")
};

UCLASS()
class GP3_2022_TEAM06_API AVideoActor : public AActor
{
	GENERATED_BODY()
	
public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxCollision;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	APawn* PlayerPawn;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	bool bPlayVideo = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadWrite)
	TEnumAsByte<VideoStatus> VideoStatus = NotStarted;

	// Sets default values for this actor's properties
	AVideoActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnOverlapPlayerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
