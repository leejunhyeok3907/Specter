// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "WeaponComp/MySequenceNodes.h"
#include "ThrowableObject.generated.h"

UCLASS()
class SPECTER_API AThrowableObject : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AThrowableObject();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ATrain_Between> Actor_Train;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> PreActor;

protected:
	bool bThrow = false;
	
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovement;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* StaticMesh;

	UPROPERTY(VisibleAnywhere)
	class UStaticMeshComponent* TrainMesh;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Sphere;

	FDoOnce ThrowOnce;

public:
	void Throw(FVector _Velocity);

	virtual void AfterThrowEvent();

	void SimulatePhysics();

};
