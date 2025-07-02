// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

/**
 * 
 */

UENUM(BlueprintType)
enum class EGunState : uint8
{
	Dropped = 0 UMETA(DisplayName = "Dropped"),
	Equipped UMETA(DisplayName = "Equipped"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EWeaponSlot : uint8
{
	WeaponNone = 0 UMETA(DisplayName = "WeaponNone"),
	WeaponSlot1 = 1 UMETA(DisplayName = "WeaponSlot1"),
	WeaponSlot2 = 2 UMETA(DisplayName = "WeaponSlot2"),
	WeaponSlot3 = 3 UMETA(DisplayName = "WeaponSlot3"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class ESelectorType : uint8
{
	Single = 0 UMETA(DisplayName = "Single"),
	Burst UMETA(DisplayName = "Burst"),
	FullAuto UMETA(DisplayName = "FullAuto"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EWeaponType : uint8
{
	Pistol = 0 UMETA(DisplayName = "Pistol"),
	AssultRifle UMETA(DisplayName = "AssultRifle"),
	SniperRifle UMETA(DisplayName = "SniperRifle"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EAttachmentType : uint8
{
	Optics = 0 UMETA(DisplayName = "Optics"),
	Supressor UMETA(DisplayName = "Supressor"),
	LaserSight UMETA(DisplayName = "LaserSight"),
	End UMETA(DisplayName = "End"),
};

UENUM(BlueprintType)
enum class EWeaponLoadType : uint8
{
	E_AK47 = 0 UMETA(DisplayName = "AK47"),
	E_AK74 UMETA(DisplayName = "AK74"),
	E_ASVAL UMETA(DisplayName = "ASVAL"),
	E_M4 UMETA(DisplayName = "M4"),
	E_M4_SILENSER UMETA(DisplayName = "M4_SILENSER"),
	E_PISTOL UMETA(DisplayName = "PISTOL"),
	E_SVD UMETA(DisplayName = "SVD"),
	E_VSK UMETA(DisplayName = "VSK"),
	End UMETA(DisplayName = "End"),
};

class SPECTER_API MyCustomEnums
{
public:
	MyCustomEnums();
	~MyCustomEnums();
};
