// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDA_WeaponImpactDefinition.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API UPDA_WeaponImpactDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, class UParticleSystem*> ImpactVFX;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, class USoundCue*> ImpactSound;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, class UMaterialInterface*> ImpactDecal;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, FVector> ImpactDecalSize;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, FVector2D> ImpactDecalScaleMultiplier;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, bool> ShouldImpactMakeNoise;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, float> ImpactNoiseRange;

	UPROPERTY(EditAnywhere, Category = "BulletImpactSettings")
	TMap<TEnumAsByte<EPhysicalSurface>, int> DecalMaxNumberOfVariations;

};
