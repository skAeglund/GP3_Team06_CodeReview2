// Fill out your copyright notice in the Description page of Project Settings.


#include "SubtitleWidget.h"
#include "Components/TextBlock.h"
void USubtitleWidget::NativeConstruct()
{
	UpdateSubtitle("Default Subtitle, I am a bug");
	SetVisibility(ESlateVisibility::Hidden);
	Super::NativeConstruct();
}
void USubtitleWidget::UpdateSubtitle(FString Input)
{
	GEngine->AddOnScreenDebugMessage(1, 2.f, FColor::Purple,
	GetNameSafe(SubtitleTextBlock));
	if (SubtitleTextBlock)
	{
		SubtitleTextBlock->SetText(FText::FromString(Input));
	}
}
