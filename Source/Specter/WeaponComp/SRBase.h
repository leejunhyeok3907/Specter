// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GunBase.h"
#include "SRBase.generated.h"

/**
 * 
 */
UCLASS()
class SPECTER_API ASRBase : public AGunBase
{
	GENERATED_BODY()

public:
	ASRBase();

public:
	virtual void Init_OnCreate();
};
