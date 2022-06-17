// Fill out your copyright notice in the Description page of Project Settings.


#include "SubtitleActor.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ASubtitleActor::ASubtitleActor()
{}

// Called when the game starts or when spawned
void ASubtitleActor::BeginPlay()
{
	Super::BeginPlay();
	PlayerHud = CreateWidget<USubtitleWidget>(UGameplayStatics::GetPlayerController(this
		,0), PlayerHudClass);
	PlayerHud->AddToViewport();
}