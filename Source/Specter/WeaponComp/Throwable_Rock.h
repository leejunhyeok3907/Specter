// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "WeaponComp/ThrowableObject.h"
#include "Throwable_Rock.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API AThrowable_Rock : public AThrowableObject
{
	GENERATED_BODY()

public:
	AThrowable_Rock();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	FDoOnce SoundOnce;

	float SoundRadius=500.f;
	
public:
	virtual void AfterThrowEvent() override;

	UFUNCTION()
	void OnHit(class UPrimitiveComponent* HitComp, class AActor* OtherActor, class UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
};
