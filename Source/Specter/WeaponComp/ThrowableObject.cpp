// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComp/ThrowableObject.h"

#include "Train_Between.h"
#include "Train_Rail.h"
#include "UObject/ConstructorHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Components/SphereComponent.h"

// Sets default values
AThrowableObject::AThrowableObject()
{
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SM(TEXT("/Game/Default/ThirdPersonKit/Meshes/TPSKitProps/SciFiGrenade/SM_SciFi_Grenade_01_01.SM_SciFi_Grenade_01_01"));

 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	//DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("DefaultSceneComponent");
	//SetRootComponent(DefaultSceneComponent);

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>("Root");
	SetRootComponent(DefaultSceneComponent);
	
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>("StaticMesh");
	StaticMesh->SetNotifyRigidBodyCollision(true);
	StaticMesh->SetupAttachment(DefaultSceneComponent);

	TrainMesh = CreateDefaultSubobject<UStaticMeshComponent>("TrainMesh");
	TrainMesh->SetupAttachment(StaticMesh);

	if (SM.Succeeded())
	{
		StaticMesh->SetStaticMesh(SM.Object);
		StaticMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));

		TrainMesh->SetStaticMesh(SM.Object);
		TrainMesh->SetRelativeScale3D(FVector(1.5f, 1.5f, 1.5f));
	}

	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>("ProjectileMovement");

	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called when the game starts or when spawned
void AThrowableObject::BeginPlay()
{
	Super::BeginPlay();

	ProjectileMovement->SetActive(false);
	StaticMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	TrainMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	ProjectileMovement->ProjectileGravityScale = 0.f;
}

// Called every frame
void AThrowableObject::Tick(float DeltaTime) {
	Super::Tick(DeltaTime);

	FVector CompLocation = GetActorLocation();
	FHitResult Hit;
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);


	if (bThrow == true) {
		if (IsValid(Actor_Train) == true) {
			FRotator tVecRot = Actor_Train->Rail_Train->GetActorRotation() + FRotator(0, 90, 0);
			FVector tVec = Actor_Train->Rail_Train->GetActorLocation() - tVecRot.RotateVector(Actor_Train->GetActorLocation() - GetActorLocation());
			FRotator tRot;
			tRot.Roll = GetActorRotation().Roll;
			tRot.Pitch = GetActorRotation().Pitch;
			tRot.Yaw = Actor_Train->Rail_Train->GetActorRotation().Yaw + GetActorRotation().Yaw + 90;
			TrainMesh->SetWorldLocation(tVec, false, nullptr, ETeleportType::TeleportPhysics);
			TrainMesh->SetWorldRotation(tRot, false, nullptr, ETeleportType::TeleportPhysics);
			CompLocation = TrainMesh->GetComponentLocation();
		}
	}
	
	if (IsValid(PreActor) == false) {
		if (IsValid(Actor_Train) == true) {
			TObjectPtr<ATrain_Rail> PreTick = Actor_Train->Rail_Train;
			while(true) {
				if (IsValid(PreTick) == true) {
					if (IsValid(PreTick->Back_Train) == true) {
						PreTick = PreTick->Back_Train;
					}
					else {
						break;
					}
				}
				else {
					break;
				}
			}
			PreActor = PreTick;
			AddTickPrerequisiteActor(PreActor);
		}
	}
}

void AThrowableObject::Throw(FVector _Velocity)
{
	if (ThrowOnce.Execute())
	{
		bThrow = true;
		ProjectileMovement->SetActive(true);
		ProjectileMovement->Velocity = _Velocity;

		DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
		
		FRotator RandRot = UKismetMathLibrary::RandomRotator();
		FLatentActionInfo Info;

		//UKismetSystemLibrary::MoveComponentTo(StaticMesh, FVector::ZeroVector, RandRot, false, false, 0.5f, false, EMoveComponentAction::Move, Info);

		float InDelayTime = 0.1f;
		FTimerHandle TH;
		GetWorld()->GetTimerManager().SetTimer(TH, FTimerDelegate::CreateLambda([&]()
			{
		 		AfterThrowEvent();
			}
		), InDelayTime, false);
	}
}

void AThrowableObject::AfterThrowEvent()
{
	SimulatePhysics();
}

void AThrowableObject::SimulatePhysics()
{
	FVector Velocity = GetVelocity();

	StaticMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	StaticMesh->SetSimulatePhysics(true);
	StaticMesh->SetPhysicsLinearVelocity(Velocity);
}
