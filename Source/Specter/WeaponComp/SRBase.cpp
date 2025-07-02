// Fill out your copyright notice in the Description page of Project Settings.


#include "SRBase.h"
#include "Components/WidgetComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"

ASRBase::ASRBase() 
{
	WeaponSlot = EWeaponSlot::WeaponSlot2;
	WeaponType = EWeaponType::SniperRifle;
}

void ASRBase::Init_OnCreate()
{
	Super::Init_OnCreate();
}
