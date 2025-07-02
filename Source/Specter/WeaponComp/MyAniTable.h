// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Animation/AnimInstance.h"
#include "MyAniTable.generated.h"

/**
 * 
 */
USTRUCT(BlueprintType)
struct FDataName :public FTableRowBase
{
	GENERATED_USTRUCT_BODY()
	
public :
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	UAnimMontage* Montage = nullptr;
};

UCLASS()
class SPECTER_API UMyAniTable : public UDataTable
{
	GENERATED_BODY()
};
