// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "SubtitleWidget.h"
#include "GameFramework/Actor.h"
#include "SubtitleActor.generated.h"

UCLASS()
class GP3_2022_TEAM06_API ASubtitleActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ASubtitleActor();
	UPROPERTY(BlueprintReadOnly)
	USubtitleWidget* PlayerHud;
	UPROPERTY(EditDefaultsOnly, Category = "Settings | Classes") // shout out to Nikolai Nyqvist
	TSubclassOf<USubtitleWidget> PlayerHudClass;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
public:	
	// Called every frame
	//virtual void Tick(float DeltaTime) override;

};
