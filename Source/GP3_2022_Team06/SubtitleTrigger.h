// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubtitleActor.h"
#include "SubtitleWidget.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Actor.h"
#include "SubtitleTrigger.generated.h"


UCLASS()
class GP3_2022_TEAM06_API ASubtitleTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubtitleTrigger();
	virtual void UpdateSubtitles();
	UPROPERTY(EditAnywhere,BlueprintReadWrite)
	ASubtitleActor* SubtitleActor;
	USubtitleWidget* SubtitleUI; // can not be set in Editor, this is a workaround
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FString> Lines; // The exact line displayed
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<float> TimeStamps; // The exact time the display is at

	FTimerHandle VideoTimer;
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void NotifyActorBeginOverlap(AActor* OtherActor);
	virtual void NotifyActorEndOverlap(AActor* OtherActor);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UBoxComponent* BoxComp;
	bool bPlayerInArea; // also unused
	bool bHasStarted;
	float TimeElapsed; // can probably be replaced by link to media

	float DisplayTime;
	UBoxComponent* BeginArea;
	APawn* PlayerPawn;

};
