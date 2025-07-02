// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachmentBase.h"
#include "LaserSightBase.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API ALaserSightBase : public AAttachmentBase
{
	GENERATED_BODY()

public:
	ALaserSightBase();
	
public:
	UPROPERTY(VisibleAnywhere)
	class UArrowComponent* Arrow;

	UPROPERTY(VisibleAnywhere)
	FName LaserStartSocket;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	class UParticleSystem* WeaponLaserSightFX;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	FLinearColor LaserSightColor;

	UPROPERTY(EditAnywhere, Category = "LaserSight")
	class UParticleSystemComponent* LaserSightComponent;

public:
	UFUNCTION()
	FVector GetLaserStartSocket();

	UFUNCTION()
	FVector GetLaserForwardVector();

	void SetLaserSightVisual(bool _flag);

	TTuple<FVector, bool> GetLaserForwardTrace();

	UFUNCTION()
	void SpawnLaserEmitter();

	virtual void Init_OnCreate() override;
};
