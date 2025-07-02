// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComp/ClipEjectionBase.h"
#include "Components/SphereComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundCue.h"

// Sets default values
AClipEjectionBase::AClipEjectionBase()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));



	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("Sphere"));
	Sphere->SetupAttachment(DefaultSceneComponent);
	Sphere->SetCollisionProfileName("NoCollision");

	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/Model/Weapon/Weapon_Pack/M4AssaultRifle/Mesh/StaticMesh/SM_M4_m4_030.SM_M4_m4_030"));

	if (SM.Succeeded())
	{
		Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
		Mesh->SetupAttachment(Sphere);
		Mesh->SetStaticMesh(SM.Object);
		Mesh->SetSimulatePhysics(true);
		Mesh->SetNotifyRigidBodyCollision(true);
		Mesh->SetCollisionProfileName("PhysicsActor");
	}

	static ConstructorHelpers::FObjectFinder<USoundCue> GroundSound(TEXT("/Game/Sound/Sound_FX/Character/TPS_Audio/TPSKitOrginals/weapons/pistol_bounce_clip_cue.pistol_bounce_clip_cue"));

	if (GroundSound.Succeeded())
	{
		GroundHitSound = GroundSound.Object;
	}
}

// Called when the game starts or when spawned
void AClipEjectionBase::BeginPlay()
{
	Super::BeginPlay();
	
	Mesh->OnComponentHit.AddDynamic(this, &AClipEjectionBase::OnHit);
}

// Called every frame
void AClipEjectionBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AClipEjectionBase::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (SoundOnce.Execute())
	{
		FVector SoundLocation = GetActorLocation();

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), GroundHitSound, SoundLocation);
	}
}

