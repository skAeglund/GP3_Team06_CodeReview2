// Fill out your copyright notice in the Description page of Project Settings.


#include "SubtitleTrigger.h"

#include "Kismet/GameplayStatics.h"


// Sets default values
ASubtitleTrigger::ASubtitleTrigger()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	BoxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	BoxComp->SetBoxExtent(FVector(128, 128, 64));
	BoxComp->SetVisibility(true);
	bHasStarted = false;
}

// Called when the game starts or when spawned
void ASubtitleTrigger::BeginPlay()
{
	Super::BeginPlay();
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
}
void ASubtitleTrigger::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (!SubtitleUI) // this is done late in order to make sure the SubtitleActor has gone through its BeginPlay()
	{
		if(SubtitleActor)
		{
			SubtitleUI = SubtitleActor->PlayerHud;
		}
		else
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Turquoise, TEXT("SubtitleActor not set in: ") + GetName());
		}
	}
	if (OtherActor == PlayerPawn && !bHasStarted)
	{
		SubtitleUI->UpdateSubtitle(""); // to clear any possible message
		bPlayerInArea = true;
		bHasStarted = true;
		GetWorldTimerManager().SetTimer(VideoTimer, this, &ASubtitleTrigger::UpdateSubtitles, 0.01, true, 0);
	}
}
void ASubtitleTrigger::NotifyActorEndOverlap(AActor* OtherActor) // not currently used but might want to be in future
{
	/*if (OtherActor == PlayerPawn) 
	{
		bPlayerInArea = false;
		SubtitleUI->SetVisibility(ESlateVisibility::Hidden);
	}*/
}
void ASubtitleTrigger::UpdateSubtitles()
{
	if (bPlayerInArea) // if player in area
	{
		for(int i = 0; i < TimeStamps.Num(); i++)
		{
			if (FMath::IsNearlyEqual(TimeStamps[i], TimeElapsed, 0.001f)) // tolerance seems fine, is this expensive?
				{
				SubtitleUI->SetVisibility(ESlateVisibility::Visible);
				SubtitleUI->UpdateSubtitle(GetData(Lines[i]));
				break;
				}
		}
		if (TimeStamps.Last() - TimeElapsed < 0)
		{
			SubtitleUI->SetVisibility(ESlateVisibility::Hidden);
		}
	}
	TimeElapsed += 0.01f;
}