// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyCustomEnums.h"
#include "WeaponToOwner.h"
#include "WeaponComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SPECTER_API UWeaponComponent : public UActorComponent, public IWeaponToOwner
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UWeaponComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	TArray<class AGunBase*> GunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponSlot ChangeGunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	EWeaponSlot CurrentGunSlot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	class AGunBase* CurrentEquipWeapon;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UUIMainHUD* MainHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UPauseUI* PauseHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UDamageUI* DamageHUDWidget;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
	class UCrossHairUI* CrossHairWidget;

	UPROPERTY(VisibleAnywhere)
		int32 ClipNum;
	UPROPERTY()
		int32 ClipAllNum;
	UPROPERTY(VisibleAnywhere)
		int32 ClipNum2;
	UPROPERTY()
		int32 ClipAllNum2;
	UPROPERTY(VisibleAnywhere)
		int32 ClipNum3;
	UPROPERTY()
		int32 ClipAllNum3;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon1 = false;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon2 = false;
	UPROPERTY(VisibleAnywhere)
		bool SwapWeapon3 = false;

	UPROPERTY()
		int32 RandomNumber;

	UPROPERTY()
		float HPopacity;

	UPROPERTY()
	FTimerHandle TH;

	UPROPERTY()
	FTimerHandle TH2;

	bool IsDamage = false;
	bool CanShoot;

	bool IsAiming;

	TArray<int> Ammo;
	//TArray<UTexture2D*> text;

	bool IsPlayer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	bool IsReloading;

	bool IsShooting;

	bool IsOwnerPlayFireAnimation = false;

	bool CanUseBurstShot = true;

	float CurFireCoolTime;

	UPROPERTY()
	class UPDA_WeaponImpactDefinition* WeaponImpactEffect;

	float UserFov;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "GunValue", Meta = (AllowPrivateAccess = true))
	bool ADSMode;

	bool KeepADSMode;

	void SetAimPointCamera(class UCameraComponent* _NewCamera);

	TObjectPtr<AMyCharacterAI> AIOwner;

public:
	UPROPERTY()
	class UCameraComponent* AimPointCamera;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool InfinityAmmo;

	bool LaserToFront = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool LaserSightOn = true;
	
public:
	UPROPERTY()
	class UTexture2D* WeaponText1;
	UPROPERTY()
	class UTexture2D* WeaponText2;
	UPROPERTY()
	class UTexture2D* WeaponText3;	
	
	UPROPERTY()
	FName Text1;
	UPROPERTY()
	FName Text2;
	UPROPERTY()
	FName Text3;

	TArray<TSubclassOf<AGunBase>> GunClassData;

public:
	UFUNCTION()
	void PickUpGun(class AGunBase* _NewGun);

	UFUNCTION(BlueprintCallAble)
	void SetInfinityAmmo(bool _flag = false);

	UFUNCTION(BlueprintCallAble)
	void SetLaserSightVisual(bool _flag = true);

	UFUNCTION(BlueprintCallAble)
	void SetLaserSightToFront(bool _flag = true);

	bool IsOwnerRotationToFront();

	void AddCrossHairSpread(float _amount);

	void UpdateCurrentGun();
	//void UpdateTexture();

	FName GetGunHoldSocketName(class AGunBase* _Gun);

	FName GetGunHoldSocketName();

	void UpdateGunAttachment();

	TTuple<FVector, bool> GetLookat();
	
	void InitalizeWeaponComponent(class UCameraComponent* _AimCamera, TSubclassOf<class AGunBase> _GunSlot1, TSubclassOf<class AGunBase> _GunSlot2, TSubclassOf<class AGunBase> _GunSlot3, bool _bPlayer = false);

	void LoadWeapons(TArray<EWeaponLoadType> _GunSlots);

	bool CheckEnoughAmmo();

	void ReloadGun();

	bool ReloadCheck();

	bool CheckCanShoot();

	void UpdateUI();

	TTuple<FVector, FRotator> GetFirePosition();

	void SpawnBulletTrail(FHitResult _HitData, FVector _TraceEnd, float _RayLength);

	void HitImpulse(FHitResult _HitData);

	void ImpactEffects(FHitResult _HitData);

	class UCameraComponent* GetADSCamera();

	FVector SpreadTrace(FVector _InputTrace);

public:
	UFUNCTION(BlueprintCallable)
	void EquipWeapon(EWeaponSlot _Slot);

	void WeaponShellEjection();

	UFUNCTION(BlueprintCallAble)
	void WeaponClipEjection();

	UFUNCTION(BlueprintCallAble)
	void PlayWeaponReloadAnim();

	UFUNCTION(BlueprintCallAble)
	void PlayWeaponFireAnim();

	void SpawnBullet();

	void PlayWeaponFireFx();

	void WeaponReload();

	void WeaponSelectorChange();

	void BurstFire();

	void GunFire();

	void FullAutoFire();

	void FullAutoFireRetrigger();

	void ReleaseFire();

	void WeaponSequence();

	void Texture();
	void SwapTexture();
	
	UFUNCTION(BlueprintCallable)
	void ReloadFail();
	
public://�������̽�
	UFUNCTION(BlueprintCallable)
	virtual void ReloadAnimationComplete() override;

	UFUNCTION(BlueprintCallable)
	virtual void SlotChangeAnimationComplete() override;

	UFUNCTION(BlueprintCallable)
	virtual void OwnerFireAnimationComplete() override;
};
