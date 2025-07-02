// Fill out your copyright notice in the Description page of Project Settings.


#include "OpticsBase.h"

AOpticsBase::AOpticsBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/WeaponSystem/FPS_Weapon_Bundle/Weapons/Meshes/Accessories/SM_T4_Sight.SM_T4_Sight"));

	if (SM.Succeeded())
	{
		AttachmentMesh->SetStaticMesh(SM.Object);
		AttachmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	AimingCameraSocket = FName("AimingSocket");
	SniperFOV = 80.f;
}

void AOpticsBase::Init_OnCreate()
{
	Super::Init_OnCreate();
	AttachmentMesh->SetCollisionProfileName("NoCollision");
	SetActorRelativeLocation(ObjectOffset);
}

FVector AOpticsBase::GetAimingCameraSocketLocation()
{
	return AttachmentMesh->GetSocketLocation(AimingCameraSocket);
}