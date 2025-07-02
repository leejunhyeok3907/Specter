// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttachmentBase.h"
#include "MyCustomEnums.h"
#include "SupressorBase.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API ASupressorBase : public AAttachmentBase
{
	GENERATED_BODY()

public:
	ASupressorBase();

public:
	UPROPERTY(VisibleAnywhere)
	float FireSoundRadius;

	UPROPERTY(EditAnywhere)
	class USoundBase* Pistol_FireSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* AR_FireSound;

	UPROPERTY(EditAnywhere)
	class USoundBase* SR_FireSound;

public:
	class USoundBase* GetSupressorFireSound(EWeaponType _type);

	virtual void Init_OnCreate() override;
};
