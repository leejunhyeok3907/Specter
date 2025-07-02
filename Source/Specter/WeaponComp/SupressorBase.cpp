// Fill out your copyright notice in the Description page of Project Settings.


#include "SupressorBase.h"
#include "Sound/SoundBase.h"

ASupressorBase::ASupressorBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/Model/Character/TSam_Agent/Weapons/Mesh/Pistol/SM_Mesh/SM_Pistol_Silencer.SM_Pistol_Silencer"));

	if (SM.Succeeded())
	{
		AttachmentMesh->SetStaticMesh(SM.Object);
	}
}

USoundBase* ASupressorBase::GetSupressorFireSound(EWeaponType _type)
{
	if (EWeaponType::Pistol == _type) return Pistol_FireSound;
	else if (EWeaponType::AssultRifle == _type) return AR_FireSound;
	else if (EWeaponType::SniperRifle == _type) return SR_FireSound;

	return nullptr;
}

void ASupressorBase::Init_OnCreate()
{
	AttachmentMesh->SetCollisionProfileName("NoCollision");
}
