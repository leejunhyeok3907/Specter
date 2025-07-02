// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PDA_GunSocketData.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API UPDA_GunSocketData : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, Category = "Gun")
	FName MuzzleFlashName;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName SupressorMuzzleFlashName;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName AimingCameraLocation;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName AttachmentSocketLaser;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName AttachmentSocketSupressor;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName AttachmentSocketScope;
	
	UPROPERTY(EditAnywhere, Category = "Gun")
	FName BaseSocketScope;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName BulletShellSpawnLocation;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName HandGripTransform;

	UPROPERTY(EditAnywhere, Category = "Gun")
	FName Magazine;

public:
	UPROPERTY(EditAnywhere, Category = "Character")
	FName HandRifleSocket;

	UPROPERTY(EditAnywhere, Category = "Character")
	FName HandPistolSocket;

	UPROPERTY(EditAnywhere, Category = "Character")
	FName GunHoldSocket1;

	UPROPERTY(EditAnywhere, Category = "Character")
	FName GunHoldSocket2;

	UPROPERTY(EditAnywhere, Category = "Character")
	FName GunHoldSocket3;
};
