// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/TextBlock.h"
#include "SubtitleWidget.generated.h"

/**
 * 
 */
UCLASS()
class GP3_2022_TEAM06_API USubtitleWidget : public UUserWidget
{
	GENERATED_BODY()
	virtual void NativeConstruct() override;
public:
	virtual void UpdateSubtitle(FString Input);
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget)) // Sorcery
	UTextBlock* SubtitleTextBlock;
};
