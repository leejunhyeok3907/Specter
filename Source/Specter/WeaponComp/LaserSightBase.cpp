// Fill out your copyright notice in the Description page of Project Settings.


#include "LaserSightBase.h"
#include "Components/ArrowComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"

ALaserSightBase::ALaserSightBase()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/Default/ThirdPersonKit/Meshes/TSPKitAttachments/SM_Laser_04.SM_Laser_04"));
	static ConstructorHelpers::FObjectFinder<UParticleSystem> FX(TEXT("/Game/WeaponSystem/Item/Attachments/M_Laser.M_Laser"));

	if (SM.Succeeded())
	{
		AttachmentMesh->SetStaticMesh(SM.Object);
		AttachmentMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	}

	if (FX.Succeeded())
	{
		WeaponLaserSightFX = FX.Object;
	}

	Arrow = CreateDefaultSubobject<UArrowComponent>(TEXT("Arrow"));
	Arrow->SetupAttachment(AttachmentMesh);
	Arrow->SetRelativeRotation(FRotator(0.f, 90.f, 0.f));

	LaserStartSocket = LaserStartSocket;
}

FVector ALaserSightBase::GetLaserStartSocket()
{
	FVector Location= AttachmentMesh->GetSocketLocation(LaserStartSocket);

	return Location;
}

FVector ALaserSightBase::GetLaserForwardVector()
{
	FVector Forward = Arrow->GetForwardVector();

	return Forward;
}

void ALaserSightBase::SetLaserSightVisual(bool _flag)
{
	if (IsValid(LaserSightComponent))
	{
		LaserSightComponent->SetVisibility(_flag);
	}
}

TTuple<FVector, bool> ALaserSightBase::GetLaserForwardTrace()
{
	FVector LaserStartLocation = GetLaserStartSocket();
	FVector TraceEndLocation = LaserStartLocation + (GetLaserForwardVector() * 10000.f);

	FHitResult HitResult;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(GetOwner());
	CollisionParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(HitResult, LaserStartLocation, TraceEndLocation, ECollisionChannel::ECC_Visibility, CollisionParams);

	FVector ResultVector = HitResult.bBlockingHit ? HitResult.Location : HitResult.TraceEnd;
	bool ResultHit = HitResult.bBlockingHit;

	return MakeTuple(ResultVector, ResultHit);
}

void ALaserSightBase::SpawnLaserEmitter()
{
	LaserSightComponent = UGameplayStatics::SpawnEmitterAttached
	(WeaponLaserSightFX, AttachmentMesh, LaserStartSocket, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::KeepRelativeOffset);
}

void ALaserSightBase::Init_OnCreate()
{
	AttachmentMesh->SetCollisionProfileName("NoCollision");
	SpawnLaserEmitter();
}