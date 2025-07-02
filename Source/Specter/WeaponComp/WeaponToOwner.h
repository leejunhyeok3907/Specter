// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCustomEnums.h"
#include "WeaponToOwner.generated.h"

// This class does not need to be modified.
UINTERFACE(BlueprintType, Blueprintable, MinimalAPI)
class UWeaponToOwner : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class SPECTER_API IWeaponToOwner
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.

		//Weapon to Owner
public:
	virtual void PlayReloadAnimation() {}
	virtual void PlayFireAnimation() {}
	virtual void PlaySlotHoldAnimation(EWeaponSlot _Slot) {}
	virtual void PlaySlotReleaseAnimation(EWeaponSlot _Slot) {}
	virtual void PlayGrenadeThrowAnimation() {}
	virtual void PlayGrenadeThrowEndAnimation() {}
	virtual void StopGrenadeThrowAnimation() {}
	virtual void PlayOwnerFireAnimation(EWeaponType _Type) {}

		//Owner to Weapon
public:
	virtual void ReloadAnimationComplete() {}
	virtual void SlotChangeAnimationComplete() {}
	virtual void OwnerFireAnimationComplete() {}
};
