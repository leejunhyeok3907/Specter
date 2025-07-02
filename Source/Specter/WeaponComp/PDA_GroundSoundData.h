// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDA_GroundSoundData.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API UPDA_GroundSoundData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, class USoundCue*> FootSound;
	
};
