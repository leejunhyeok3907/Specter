// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCustomEnums.h"
#include "PDA_WeaponData.generated.h"


/**
 * 
 */
UCLASS()
class SPECTER_API UPDA_WeaponData : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	UPDA_WeaponData();

	UPROPERTY(EditAnywhere, Category = "GunValue")
	FName WeaponName;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class USkeletalMesh* WeaponMesh;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EWeaponType WeaponType;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	EWeaponSlot WeaponSlot;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	int AmmoIndex;
	
	UPROPERTY(EditAnywhere, Category = "GunValue")
	int MaxClip;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float ImpactSoundRadius;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float FireSoundRadius;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class USoundBase* GunReloadSound;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class UAnimationAsset* GunReloadAnim;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class USoundBase* GunFireSound;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class UAnimationAsset* GunFireAnim;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class UNiagaraSystem* GunFireParticle;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	class UTexture2D* WeaponHUD;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	TSubclassOf<class AShellEjectionBase> ShellEjectionObj;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	TSubclassOf<class AActor> MagazineObj;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float WeaponSpread;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	FVector2D WeaponDamage;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UCurveFloat* GunRecoilCurveYaw;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UCurveFloat* GunRecoilCurvePitch;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	float GunRecoilTimelineLength;

	UPROPERTY(EditAnywhere, Category = "GunValue")
	UNiagaraSystem* TrailParticle;

public:
	UPROPERTY(EditAnywhere, Category = "CharValue")
	UAnimationAsset* CharacterFireAnim;

	UPROPERTY(EditAnywhere, Category = "CharValue")
	UAnimationAsset* CharacterReloadAnim;
};
