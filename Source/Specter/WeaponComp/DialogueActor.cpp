// Fill out your copyright notice in the Description page of Project Settings.


#include "DialogueActor.h"

#include "../Event/DialogueInstance.h"
#include "UIGameMode.h"
#include "../Event/DialogueTable.h"
#include "Components/BoxComponent.h"

// Sets default values
ADialogueActor::ADialogueActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	Collider = CreateDefaultSubobject<UBoxComponent>(TEXT("Interaction Collider"));
	SetRootComponent(Collider);
	IsInteractable = false;
	IsUsed = false;
	
}

// Called when the game starts or when spawned
void ADialogueActor::BeginPlay()
{
	Super::BeginPlay();
	/*Collider->OnComponentBeginOverlap.AddDynamic(this, &ADialogueActor::OnOverlapBegin);
	Collider->OnComponentEndOverlap.AddDynamic(this, &ADialogueActor::OnOverlapEnd);*/
}

// Called every frame
void ADialogueActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ADialogueActor::OnInteract_Implementation(AMyCharacter* Target)
{
	UE_LOG(LogTemp, Log, TEXT("Inst Interaction OnInteract Called"));
}

void ADialogueActor::OnInteractRangeEntered_Implementation(AMyCharacter* Target)
{
	if (false == IsUsed) return;
	UE_LOG(LogTemp, Log, TEXT("Inst Interaction Entered"));
	IsInteractable = true;
	FDialogueData* Row = GetWorld()->GetGameInstance<UDialogueInstance>()->GetDialogueData(DialogueId);
	if (nullptr == Row)
	{
		UE_LOG(LogTemp, Log, TEXT("Failed to find target row, Dialogue Id = %d"), DialogueId);
	}
	else
	{
		//UE_LOG(LogTemp, Log, TEXT("Found dialogue : %s"), *Row->text);
		//GetWorld()->GetAuthGameMode<AUIGameMode>()->UpdateDialogueWidget(Row->text);
	}
}

void ADialogueActor::OnInteractRangeExited_Implementation(AMyCharacter* Target)
{
	if (false == IsUsed) return;
	UE_LOG(LogTemp, Log, TEXT("Inst Interaction Exited"));
	IsInteractable = false;
}

void ADialogueActor::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Collision Enter"));
}

void ADialogueActor::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	UE_LOG(LogTemp, Log, TEXT("Collision Exited"));
}
