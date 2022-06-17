// Fill out your copyright notice in the Description page of Project Settings.


#include "VideoActor.h"
#include "Components/BoxComponent.h"
#include "Components/SceneComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AVideoActor::AVideoActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapCollision"));
	BoxCollision->SetBoxExtent(FVector(100, 100, 100));
	BoxCollision->CanCharacterStepUpOn = ECanBeCharacterBase::ECB_No;

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &AVideoActor::OnOverlapPlayerBegin);
	
}

// Called when the game starts or when spawned
void AVideoActor::BeginPlay()
{
	Super::BeginPlay();

	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	
}

// Called every frame
void AVideoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AVideoActor::OnOverlapPlayerBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (!PlayerPawn)
	{
		PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	}
	if (OtherActor == PlayerPawn)
	{
		bPlayVideo = true;
	}
}


