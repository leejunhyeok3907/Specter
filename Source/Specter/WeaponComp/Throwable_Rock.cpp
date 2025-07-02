// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComp/Throwable_Rock.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Perception/AISense_Hearing.h"

AThrowable_Rock::AThrowable_Rock()
{
}

void AThrowable_Rock::BeginPlay()
{
	Super::BeginPlay();

	StaticMesh->OnComponentHit.AddDynamic(this, &AThrowable_Rock::OnHit);
}

void AThrowable_Rock::AfterThrowEvent()
{
	Super::AfterThrowEvent();


}

void AThrowable_Rock::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	if (SoundOnce.Execute())
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Grenade Hit"));

		FVector SoundLocation = GetActorLocation();
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, SoundLocation.ToString());

		UAISense_Hearing::ReportNoiseEvent(GetWorld(), SoundLocation, 1.0f, this, SoundRadius);

		//DrawDebugSphere(GetWorld(), SoundLocation, SoundRadius, 26, FColor::Red, true);
		//UGameplayStatics::PlaySoundAtLocation(GetWorld(), , SoundLocation);
	}
}