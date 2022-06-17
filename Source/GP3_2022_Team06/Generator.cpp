// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator.h"

#include "Components/BoxComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Player/Pickup/PickupComponent.h"


// Sets default values
AGenerator::AGenerator()
{
	GeneratorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("GenMesh"));
	AnimationEndPos = CreateDefaultSubobject<USceneComponent>(TEXT("EndPos"));
	
	TextBox = CreateDefaultSubobject<UTextRenderComponent>(TEXT("Text>Box"));
	TextBox->SetVisibility(false);
	TextBox->SetupAttachment(RootComponent);
	
	InteractArea = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxComp"));
	InteractArea->SetBoxExtent(FVector(128, 128, 64));
	InteractArea->SetVisibility(true);
}

void AGenerator::AudioEvent_Implementation()
{}

// Called when the game starts or when spawned
void AGenerator::BeginPlay()
{
	Super::BeginPlay();
	AnimationEndPos->SetRelativeLocation(FVector(0,123,153)); // Why does unreal not store blueprint local positions
	TextBox->SetVisibility(false);
	GetWorldTimerManager().SetTimer(AnimationTimer, this, &AGenerator::AnimUpdate, AnimationUpdateRate, true, 0.f);
	GetWorldTimerManager().PauseTimer(AnimationTimer);
	PlayerPawn = UGameplayStatics::GetPlayerPawn(this, 0);
	PickupComponent = PlayerPawn->FindComponentByClass<UPickupComponent>();
	bKeyConfirmed = false;
	InputComponent = PlayerPawn->InputComponent;

	if (InputComponent)
	{
		InputComponent->BindAction(TEXT("Interact"), IE_Pressed, this, &AGenerator::KeyDropped);
	}
	
	TargetRotation = GeneratorMesh->GetComponentRotation();
}
void AGenerator::AnimUpdate()
{
	TimerTimeElapsed = TimerTimeElapsed + AnimationUpdateRate; // Emil's scuffed DeltaTime
	if (TimerTimeElapsed > AnimationTime)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("Animation Ended"));
		TimerTimeElapsed = AnimationTime;
		GetWorldTimerManager().PauseTimer(AnimationTimer);
		LastKey->SetActorRelativeLocation(AnimationEndPos->GetComponentLocation());
	}
	AnimationCurrentPos = FMath::Lerp(AnimationStartPos,AnimationEndPos->GetComponentLocation(), TimerTimeElapsed / AnimationTime);
	LastKey->SetActorRotation(FMath::Lerp(StartRotation, TargetRotation, TimerTimeElapsed));
	LastKey->SetActorLocation(AnimationCurrentPos);
}

void AGenerator::KeyDropped()
{
	if (bKeyConfirmed && !bKeyEntered)
	{
		if (PickupComponent->bHoldingItem == true)
		{
			PickupComponent->ToggleItemPickup();
		}
		//TextBox->SetText(FText::FromString("Opening door")); no longer used
		//TextBox->SetVisibility(true);
		
		LastKey->MyMesh->SetEnableGravity(false);
		LastKey->MyMesh->ComponentVelocity.Set(0,0,0);
		AnimationStartPos = LastKey->GetActorLocation();
		LastKey->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		StartRotation = KeyActor->GetActorRotation();
		LastKey->SetActorRotation(TargetRotation);
		LastKey->MyMesh->ComponentVelocity.Set(0,0,0);
		LastKey->MyMesh->SetSimulatePhysics(false);
		LastKey->SetActorEnableCollision(false);
		GetWorldTimerManager().UnPauseTimer(AnimationTimer);
		AudioEvent();
		EnterGenArea.ExecuteIfBound(SentID);
		bKeyEntered = true;
	}
}

void AGenerator::NotifyActorBeginOverlap(AActor* OtherActor)
{
	if (KeyActor)
	{
		/*if (OtherActor == PlayerPawn)
		{

				if (PickupComponent->CurrentItem == nullptr)
				{
					TextBox->SetText(FText::FromString("Bring me a key"));
					TextBox->SetVisibility(true);
				}
				else if (PickupComponent->CurrentItem == KeyActor)
				{
					bKeyConfirmed = true;
					TextBox->SetText(FText::FromString("Drop Key to open"));
					LastKey = PickupComponent->CurrentItem;
					TextBox->SetVisibility(true);
				}
				else
				{
					TextBox->SetText(FText::FromString("Wrong key"));
					TextBox->SetVisibility(true);
				}
		}*/
		if (OtherActor == KeyActor) // needs fixing
			{
			bKeyConfirmed = true;
			LastKey = KeyActor;// This will probably break in a later update
			AGenerator::KeyDropped();
			}
	}
	else
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Red, TEXT("WHOOPS, NullPtr in Generator " + GetName() +
											 " Have you entered a keyActor?"));
	}
}

void AGenerator::NotifyActorEndOverlap(AActor* OtherActor)
{
	if (OtherActor == PlayerPawn || OtherActor == KeyActor)
	{
		TextBox->SetVisibility(false);
		bKeyConfirmed = false;
		//LastKey = nullptr;
		//ExitGenArea.ExecuteIfBound(SentID);
	}
}
