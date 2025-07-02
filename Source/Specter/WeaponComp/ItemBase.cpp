// Fill out your copyright notice in the Description page of Project Settings.

#include "ItemBase.h"
#include "MyCharacter.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Components/SphereComponent.h"
#include "InteractionWidget.h"
#include "Train_Between.h"
#include "Train_Rail.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AItemBase::AItemBase()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	DefaultSceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("DefaultSceneComponent"));
	SetRootComponent(DefaultSceneComponent);

	// Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	// Sphere->SetupAttachment(DefaultSceneComponent);
	// Sphere->SetSphereRadius(64.f);
	//
	// TrainComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TrainComponent"));
	// TrainComponent->SetupAttachment(Sphere);
	//
	// SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	// SkeletalMesh->SetupAttachment(TrainComponent);
		
	TrainComponent = CreateDefaultSubobject<USceneComponent>(TEXT("TrainComponent"));
	TrainComponent->SetupAttachment(DefaultSceneComponent);
	
	SkeletalMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("SkeletalMesh"));
	SkeletalMesh->SetupAttachment(TrainComponent);

	Sphere = CreateDefaultSubobject<USphereComponent>(TEXT("SphereCollision"));
	Sphere->SetupAttachment(SkeletalMesh);
	Sphere->SetSphereRadius(64.f);
	
	WidgetComp = CreateDefaultSubobject<UWidgetComponent>(TEXT("INTERACTWIDGET"));

	static ConstructorHelpers::FClassFinder<UUserWidget> UW(TEXT("/Game/WeaponSystem/Interaction/WBP_Interaction_2.WBP_Interaction_2_C"));

	if (UW.Succeeded())
	{
		WidgetComp->SetWidgetClass(UW.Class);
		WidgetComp->SetWidgetSpace(EWidgetSpace::Screen);
		WidgetComp->SetDrawAtDesiredSize(true);
		WidgetComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WidgetComp->InitWidget();
		WidgetComp->SetupAttachment(SkeletalMesh);
	}
}

// Called when the game starts or when spawned
void AItemBase::BeginPlay()
{
	Super::BeginPlay();

	WidgetComp->SetVisibility(false);

	UInteractionWidget* WO = Cast<UInteractionWidget>(WidgetComp->GetUserWidgetObject());
	if (WO != nullptr)
	{
		WO->SetItemName(ItemName);
	}
}

// Called every frame
void AItemBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*if (IsValid(UGameplayStatics::GetPlayerPawn(GetWorld(), 0)))
	{
		float Range = (UGameplayStatics::GetPlayerPawn(GetWorld(), 0)->GetActorLocation() - this->GetActorLocation()).Size();
		if (Range > 3000) {
			return;
		}*/
		Calc(DeltaTime);
	//}
}

void AItemBase::Calc(float DeltaTime) {}

void AItemBase::OnInteract_Implementation(AMyCharacter* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Interact"));
}

void AItemBase::OnInteractRangeEntered_Implementation(AMyCharacter* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Entered"));

	WidgetComp->SetVisibility(true);
}

void AItemBase::OnInteractRangeExited_Implementation(AMyCharacter* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Exited"));

	WidgetComp->SetVisibility(false);
}

void AItemBase::SetItemName(FName _Name)
{
	ItemName = _Name;

	UInteractionWidget* WO = Cast<UInteractionWidget>(WidgetComp->GetUserWidgetObject());
	if (WO != nullptr)
	{
		WO->SetItemName(_Name);
	}
}
