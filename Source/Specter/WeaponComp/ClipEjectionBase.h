// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MySequenceNodes.h"
#include "ClipEjectionBase.generated.h"

UCLASS()
class SPECTER_API AClipEjectionBase : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AClipEjectionBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	class USceneComponent* DefaultSceneComponent;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	class USphereComponent* Sphere;

	UPROPERTY(EditAnyWhere, BlueprintReadWrite)
	class UStaticMeshComponent* Mesh;

public:
	FDoOnce SoundOnce;

	UPROPERTY(EditAnyWhere)
	class USoundCue* GroundHitSound;

	float SoundRadius = 100.f;

public:
	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

};
