// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MySequenceNodes.generated.h"

USTRUCT(BlueprintType)
struct FDoOnce
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite)
	bool DoOnceFlag;

public:
	FORCEINLINE void Reset() { DoOnceFlag = true; }
	FORCEINLINE bool Execute()
	{
		if (DoOnceFlag)
		{
			DoOnceFlag = false;
			return true;
		}

		return false;
	}

public:
	FORCEINLINE FDoOnce();
	explicit FORCEINLINE FDoOnce(bool _StartClosed);
};

FORCEINLINE FDoOnce::FDoOnce() : DoOnceFlag(true)
{
}

FORCEINLINE FDoOnce::FDoOnce(bool _StartClosed) : DoOnceFlag(!_StartClosed)
{
}

USTRUCT(BlueprintType)
struct FGate
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere)
	bool GateOpen;

	FORCEINLINE void Open() { GateOpen = true; }
	FORCEINLINE void Close() { GateOpen = false; }
	FORCEINLINE void Toggle() { GateOpen = !GateOpen; }

	FORCEINLINE bool IsOpen() { return GateOpen; }


public:
	FORCEINLINE FGate();
	explicit FORCEINLINE FGate(bool _StartClosed);
};

FORCEINLINE FGate::FGate() : GateOpen(false)
{

}

FORCEINLINE FGate::FGate(bool _StartClosed) : GateOpen(!_StartClosed)
{

}