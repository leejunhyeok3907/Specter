// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "InteractInterface.h"
#include "ItemBase.generated.h"

UCLASS()
class SPECTER_API AItemBase : public AActor, public IInteractInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AItemBase();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	virtual void Calc(float DeltaTime);

public:
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class ATrain_Between> Actor_Train;
	UPROPERTY(VisibleAnywhere)
	TObjectPtr<AActor> PreActor;

	//FVector Actor_Location;
	
public:
	UPROPERTY(VisibleAnywhere)
	class USceneComponent* DefaultSceneComponent;

	UPROPERTY(VisibleAnywhere)
	class USphereComponent* Sphere;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<class USceneComponent> TrainComponent;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	class USkeletalMeshComponent* SkeletalMesh;
	
	UPROPERTY(VisibleAnywhere)
	class UWidgetComponent* WidgetComp;

	UPROPERTY(EditAnywhere)
	FName ItemName;

public:
	virtual void OnInteract_Implementation(class AMyCharacter* Target) override;
	virtual void OnInteractRangeEntered_Implementation(class AMyCharacter* Target) override;
	virtual void OnInteractRangeExited_Implementation(class AMyCharacter* Target) override;

public:
	UFUNCTION(BlueprintCallable)
	void SetItemName(FName _Name);
};
