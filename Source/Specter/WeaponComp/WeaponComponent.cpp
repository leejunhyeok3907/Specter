// Fill out your copyright notice in the Description page of Project Settings.


#include "WeaponComponent.h"
#include "Camera/CameraComponent.h"
#include "Math/RandomStream.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"
#include "Components/DecalComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "WeaponToOwner.h"
#include "ADS_Camera.h"
#include "ShellEjectionBase.h"
#include "ClipEjectionBase.h"

#include "NiagaraFunctionLibrary.h"
#include "NiagaraComponent.h"

#include "LaserSightBase.h"

#include "SRBase.h"
#include "ARBase.h"
#include "DamageUI.h"
#include "MyCharacter.h"
#include "MyCharacterAI.h"
#include "MyCharacterAI_Controller.h"
#include "PDA_WeaponImpactDefinition.h"
#include "PistolBase.h"
#include "Perception/AISense_Damage.h"
#include "Perception/AISense_Hearing.h"
#include "UIGameMode.h"
#include "UI/PauseUI.h"
#include "UIMainHUD.h"
#include "CrossHairUI.h"
#include "MyCharacterAI_Train.h"
#include "Sound/SoundCue.h"

// Sets default values for this component's properties
UWeaponComponent::UWeaponComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	GunSlot.SetNum((int)EWeaponSlot::End);
	Ammo.SetNum((int)EWeaponSlot::End);
	//text.SetNum((int)EWeaponSlot::End);
	Ammo[0] = 120;
	Ammo[1] = 360;
	Ammo[2] = 120;

	static ConstructorHelpers::FObjectFinder<UDataAsset> DataAsset(TEXT("/Game/WeaponSystem/Table/DA_WeaponImpactData.DA_WeaponImpactData"));

	if (DataAsset.Succeeded())
	{
		WeaponImpactEffect = Cast<UPDA_WeaponImpactDefinition>(DataAsset.Object);
	}
	// ...

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj(TEXT("/Game/UI/Sprite/Handcrafted_Weapon_Icons/White_Icon_Set/Modern/Pistols/modern_pistol__2_.modern_pistol__2_"));

	if (TextureObj.Succeeded())
	{
		WeaponText1 = Cast<UTexture2D>(TextureObj.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj2(TEXT("/Game/UI/Sprite/Handcrafted_Weapon_Icons/White_Icon_Set/Modern/Rifles/modern_rifle__12_.modern_rifle__12_"));

	if (TextureObj2.Succeeded())
	{
		WeaponText2 = Cast<UTexture2D>(TextureObj2.Object);
	}

	static ConstructorHelpers::FObjectFinder<UTexture2D> TextureObj3(TEXT("/Game/UI/Sprite/Xenon_Star/Sliced/FPS_Aspect/Weapon_Slot/weapon_example.weapon_example"));

	if (TextureObj3.Succeeded())
	{
		WeaponText3 = Cast<UTexture2D>(TextureObj3.Object);
	}

	CurrentGunSlot = EWeaponSlot::WeaponNone;

	TArray<FString> GunClassesPath;
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_AK47.BP_AK47_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_AK74.BP_AK74_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_ASVAL.BP_ASVAL_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_M4.BP_M4_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_M4_Silner.BP_M4_Silner_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_Pistol.BP_Pistol_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_SVD.BP_SVD_C"));
	GunClassesPath.Add(TEXT("/Game/WeaponSystem/Item/Gun/BP_VSK.BP_VSK_C"));

	GunClassData.SetNum(static_cast<int>(EWeaponLoadType::End));

	for (int i = 0; i < static_cast<int>(EWeaponLoadType::End); i++)
	{
		ConstructorHelpers::FClassFinder<AGunBase> GunData(*GunClassesPath[i]);

		if (GunData.Succeeded() && GunData.Class != nullptr)
		{
			GunClassData[i] = GunData.Class;
		}
	}
}


// Called when the game starts
void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	AUIGameMode* GameMode = Cast<AUIGameMode>(GetWorld()->GetAuthGameMode());
	MainHUDWidget = GameMode->GetMainHUDWidget();
	DamageHUDWidget = GameMode->GetDamageHUDWidget();
	CrossHairWidget = GameMode->GetCrossHairWidget();
	PauseHUDWidget = GameMode->GetPauseHUDWidget();
	CrossHairWidget->Hit->SetRenderOpacity(0.f);

	if (IsPlayer)
	{
		SetLaserSightVisual(false);
	}
}

void UWeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (EndPlayReason == EEndPlayReason::LevelTransition)
	{
		if (GetWorld()->GetTimerManager().TimerExists(TH))
		{
			GetWorld()->GetTimerManager().ClearTimer(TH);
		}
		if (GetWorld()->GetTimerManager().TimerExists(TH2))
		{
			GetWorld()->GetTimerManager().ClearTimer(TH2);
		}
	}

}


// Called every frame
void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...

	srand(time(NULL));
	RandomNumber = rand() % 2;
	if (IsValid(CurrentEquipWeapon))
	{
		ALaserSightBase* Laser = CurrentEquipWeapon->IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);
		TTuple<FVector, bool> Value;

		if (IsValid(Laser))
		{
			if (LaserSightOn)
			{
				Value = Laser->GetLaserForwardTrace();
				//if (!LaserToFront && IsOwnerRotationToFront())
				if (!LaserToFront)
				{
					if (IsValid(AIOwner) == true && IsValid(AIOwner->AI_Controller) == true) {
						if (AIOwner->AI_Controller->GetCurrentState() == E_EnemyState::E_StateAttack) {
							Value = GetLookat();
						}
					}
				}

				FVector _Loc = Value.Get<0>();
				bool _hit = Value.Get<1>();

				CurrentEquipWeapon->UpdateLaserSight(_hit, _Loc);
			}
		}
	}

	if (IsShooting == false && IsPlayer)
	{
		AddCrossHairSpread(-0.9f);
	}

	if (CurFireCoolTime > 0.f)
	{
		CurFireCoolTime -= DeltaTime;

		if (CurFireCoolTime <= 0.f)
			CurFireCoolTime = 0.f;
	}

	float temp_v = CrossHairWidget->Hit->GetRenderOpacity();

	if (temp_v > 0.f)
	{
		CrossHairWidget->Hit->SetRenderOpacity(FMath::Clamp(temp_v - 0.01f, 0.f, 1.f));
	}

	FString ClipText;
	FString ClipAllText;

	if (SwapWeapon1)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
		MainHUDWidget->RifleOff();
	}
	else if (SwapWeapon2)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum2);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum2);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
	}
	else if (SwapWeapon3)
	{
		ClipText = FString::Printf(TEXT("%d"), ClipNum3);
		ClipAllText = FString::Printf(TEXT("%d"), ClipAllNum3);
		MainHUDWidget->SetBullet(FText::FromString(ClipText));
		MainHUDWidget->SetBulletAll(FText::FromString(ClipAllText));
		MainHUDWidget->RifleOff();
	}
}
	

void UWeaponComponent::SetAimPointCamera(UCameraComponent* _NewCamera)
{
	AimPointCamera = _NewCamera;
}

void UWeaponComponent::PickUpGun(AGunBase* _NewGun)
{
	AGunBase* CurrentSlotGun = GunSlot[(int)_NewGun->WeaponSlot];

	FString ClipText;
	FString ClipAllText;

	if (IsValid(CurrentSlotGun))
	{
		CurrentSlotGun->DropGun();
		GunSlot[(int)_NewGun->WeaponSlot] = nullptr;
	}

	//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, FString::Printf(TEXT("Owner :%s"), *GetOwner()->GetName()));

	GunSlot[(int)_NewGun->WeaponSlot] = _NewGun;
	_NewGun->ChangeState(EGunState::Equipped);

	if (IsValid(_NewGun->PickUpSound))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), _NewGun->PickUpSound, GetOwner()->GetActorLocation());
	}

	UpdateCurrentGun();
	UpdateGunAttachment();

	if (IsValid(CurrentEquipWeapon))
	{
		if (_NewGun->AmmoIndex == 0)
		{
			ClipNum = _NewGun->CurrClip;
			ClipAllNum = Ammo[_NewGun->AmmoIndex];

			if (SwapWeapon1)
				Texture();
		}
		else if (_NewGun->AmmoIndex == 1)
		{
			ClipNum2 = _NewGun->CurrClip;
			ClipAllNum2 = Ammo[_NewGun->AmmoIndex];

			if (SwapWeapon2)
			{
				if (CurrentEquipWeapon->ARType == ESelectorType::Single)
					MainHUDWidget->RifleSingle();
				else if (CurrentEquipWeapon->ARType == ESelectorType::Burst)
					MainHUDWidget->RifleBurst();
				else if (CurrentEquipWeapon->ARType == ESelectorType::FullAuto)
					MainHUDWidget->RifleFullAuto();

				Texture();
			}
		}
		else if (_NewGun->AmmoIndex == 2)
		{
			ClipNum3 = _NewGun->CurrClip;
			ClipAllNum3 = Ammo[_NewGun->AmmoIndex];

			if(SwapWeapon3)
				Texture();
		}
	}
}

void UWeaponComponent::SetInfinityAmmo(bool _flag)
{
	InfinityAmmo = _flag;
}

void UWeaponComponent::SetLaserSightVisual(bool _flag)
{
	if (IsValid(CurrentEquipWeapon))
	{
		LaserSightOn = _flag;
		CurrentEquipWeapon->SetVisualLaserSight(_flag);
	}
}

void UWeaponComponent::SetLaserSightToFront(bool _flag)
{
	LaserToFront = _flag;
}

bool UWeaponComponent::IsOwnerRotationToFront()
{
	FRotator LookRotation = AimPointCamera->GetComponentRotation();
	FRotator OwnerRotation = GetOwner()->GetActorRotation();
	float Range = 30.f;

	float Value = UKismetMathLibrary::MapRangeClamped(OwnerRotation.Yaw, LookRotation.Yaw - Range, LookRotation.Yaw + Range, -1.f, 1.f);

	return (Value <= 0.1f && Value >= -0.1f);
}

void UWeaponComponent::AddCrossHairSpread(float _amount)
{
	CrossHairWidget->Spread = FMath::Clamp(CrossHairWidget->Spread + _amount, CrossHairWidget->MinSpread, CrossHairWidget->MaxSpread);
}

void UWeaponComponent::UpdateCurrentGun()
{
	CurrentEquipWeapon = GunSlot[(int)CurrentGunSlot];
}


FName UWeaponComponent::GetGunHoldSocketName(AGunBase* _Gun)
{
	FName SocketName = "None";

	if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot1)
	{
		SocketName = "GunStowedSocket1";
	}
	else if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot2)
	{
		SocketName = "GunStowedSocket2";
	}
	else if (_Gun->WeaponSlot == EWeaponSlot::WeaponSlot3)
	{
		SocketName = "GunStowedSocket3";
	}

	return SocketName;
}

FName UWeaponComponent::GetGunHoldSocketName()
{
	FName SocketName = "None";

	if (IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
		{
			SocketName = "PistolHoldSocket";
		}
		else
		{
			SocketName = "RifleHoldSocket";
		}
	}
	

	return SocketName;
}

void UWeaponComponent::UpdateGunAttachment()
{
	ACharacter* OwnChar = Cast<ACharacter>(GetOwner());

	if (IsValid(OwnChar))
	{
		for (AGunBase* Cur : GunSlot)
		{
			if (IsValid(Cur))
			{
				Cur->Owner = this->GetOwner();
				Cur->UseGun = true;
				TObjectPtr<AMyCharacterAI_Train> AI = Cast<AMyCharacterAI_Train>(GetOwner());
				TObjectPtr<AMyCharacter> Player = Cast<AMyCharacter>(GetOwner());
				if (Player) {
					Cur->Actor_Train = Player->Actor_Train;
				}
				else if (AI) {
					Cur->Actor_Train = AI->Between;
				}
				
				Cur->AttachToComponent(OwnChar->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetGunHoldSocketName(Cur));
			}
		}

		if (IsValid(GunSlot[(int)CurrentGunSlot]) && CurrentGunSlot!=EWeaponSlot::WeaponNone)
		{
			GunSlot[(int)CurrentGunSlot]->AttachToComponent(OwnChar->GetMesh(), FAttachmentTransformRules::SnapToTargetIncludingScale, GetGunHoldSocketName());
		}
	}
}

TTuple<FVector, bool> UWeaponComponent::GetLookat()
{
	FVector Start = AimPointCamera->GetComponentLocation();
	FVector End = (AimPointCamera->GetComponentLocation() + (AimPointCamera->GetForwardVector() * 100000.f));
	//FVector Start = CurrentEquipWeapon->GetMuzzlePosition().Get<0>();
	//FVector End = Start + CurrentEquipWeapon->GetMuzzlePosition().Get<1>().Vector() * 100000.f;


	FVector ResultVector;
	bool ResultHit;

	FHitResult HitData;

	FCollisionQueryParams Params;

	if(GetWorld()->LineTraceSingleByChannel(HitData, Start, End, ECollisionChannel::ECC_GameTraceChannel2))
	{
		ResultVector = HitData.Location;
		ResultHit = true;
	}
	else
	{
		ResultVector = HitData.TraceEnd;
		ResultHit = false;
	}
	

	return MakeTuple(ResultVector, ResultHit);
}

void UWeaponComponent::InitalizeWeaponComponent(UCameraComponent* _AimCamera, TSubclassOf<AGunBase> _GunSlot1, TSubclassOf<AGunBase> _GunSlot2, TSubclassOf<AGunBase> _GunSlot3, bool _bPlayer)
{
	AimPointCamera = _AimCamera;

	
	
	FTransform Location;
	Location.SetLocation(FVector(0.f, 0.f, 0.f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	IsPlayer = _bPlayer;

	TArray<TSubclassOf<AGunBase>> CreateArray;

	CreateArray.Add(_GunSlot1);
	CreateArray.Add(_GunSlot2);
	CreateArray.Add(_GunSlot3);

	for (TSubclassOf<AGunBase> GunClass : CreateArray)
	{
		if (GunClass)
		{
			TObjectPtr<AMyCharacterBase> Owner = Cast<AMyCharacterBase>(GetOwner());
			AGunBase* CreateGun = GetWorld()->SpawnActor<AGunBase>(GunClass, Location, SpawnParams);
			CreateGun->UseTrain = Owner->TrainMap;
			CreateGun->Init_OnCreate();
			PickUpGun(CreateGun);
		}
	}

}

void UWeaponComponent::LoadWeapons(TArray<EWeaponLoadType> _GunSlots)
{
	FTransform Location;
	Location.SetLocation(FVector(0.f, 0.f, 0.f));

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	for (auto Gun : GunSlot)
	{
		if (IsValid(Gun))
		{
			Gun->DropGun();
			Gun->DeleteGun();
		}
	}

	for (EWeaponLoadType GunClass : _GunSlots)
	{
		if (GunClassData[static_cast<int>(GunClass)])
		{
			TObjectPtr<AMyCharacterBase> Owner = Cast<AMyCharacterBase>(GetOwner());
			AGunBase* CreateGun = GetWorld()->SpawnActor<AGunBase>(GunClassData[static_cast<int>(GunClass)], Location, SpawnParams);
			CreateGun->UseTrain = Owner->TrainMap;
			CreateGun->Init_OnCreate();
			PickUpGun(CreateGun);
		}
	}
}

bool UWeaponComponent::CheckEnoughAmmo()
{
	if (IsValid(CurrentEquipWeapon))
	{
		return (CurrentEquipWeapon->CurrClip > 0);
	}

	return false;
}

void UWeaponComponent::ReloadGun()
{
	if (IsValid(CurrentEquipWeapon))
	{
		int NeedAmmo = CurrentEquipWeapon->GetMaxClip() - CurrentEquipWeapon->GetCurrClip();

		if (InfinityAmmo == false)
		{
			if (Ammo[CurrentEquipWeapon->AmmoIndex] >= NeedAmmo)
			{
				Ammo[CurrentEquipWeapon->AmmoIndex] -= NeedAmmo;
				CurrentEquipWeapon->CurrClip = CurrentEquipWeapon->MaxClip;
			}
			else
			{
				CurrentEquipWeapon->CurrClip += Ammo[CurrentEquipWeapon->AmmoIndex];

				Ammo[CurrentEquipWeapon->AmmoIndex] = 0;
			}

			//?�중??지?�것!
			if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
			{
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
			{
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::SniperRifle)
			{
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
		}
		else if(InfinityAmmo == true)
		{
			CurrentEquipWeapon->CurrClip = CurrentEquipWeapon->MaxClip;
		}

		//나중에 지울것!

		CheckCanShoot();

		IWeaponToOwner* Interface = Cast<IWeaponToOwner>(GetOwner());

		//IsReloading = false;

		if (Interface)
		{
			Interface->PlayReloadAnimation();
		}
		else
		{
			IsReloading = false;
		}
	}
}

bool UWeaponComponent::ReloadCheck()
{
	if (IsValid(CurrentEquipWeapon))
	{
		//R
		return (Ammo[CurrentEquipWeapon->AmmoIndex] > 0 && CurrentEquipWeapon->MaxClip != CurrentEquipWeapon->CurrClip);
	}

	return false;
}

bool UWeaponComponent::CheckCanShoot()
{
	//R
	bool check = (CheckEnoughAmmo() && !IsReloading && CurFireCoolTime <= 0.f);

	CanShoot = check;

	return check;
}

void UWeaponComponent::UpdateUI()
{
}

TTuple<FVector, FRotator> UWeaponComponent::GetFirePosition()
{
	FVector FirePosition;
	FRotator FireRotator;

	if (IsValid(CurrentEquipWeapon))
	{
		TTuple<FVector, FRotator> MuzzlePosition = CurrentEquipWeapon->GetMuzzlePosition();
		
		FirePosition = MuzzlePosition.Get<0>() + (UKismetMathLibrary::GetForwardVector(MuzzlePosition.Get<1>()) * 5.f);
		FireRotator = MuzzlePosition.Get<1>();
	}

	return MakeTuple(FirePosition, FireRotator);
}

void UWeaponComponent::SpawnBulletTrail(FHitResult _HitData, FVector _TraceEnd, float _RayLength)
{
	UNiagaraSystem* Particle = CurrentEquipWeapon->TrailParticle;

	FVector StartLocation = GetFirePosition().Get<0>();
	FVector VectorParam = _HitData.bBlockingHit ? _HitData.Location : _TraceEnd;

	UNiagaraComponent* Effect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(CurrentEquipWeapon->SkeletalMesh, Particle, StartLocation,
		UKismetMathLibrary::FindLookAtRotation(StartLocation, _TraceEnd), FVector::OneVector, true, true, ENCPoolMethod::AutoRelease, true);
	
	float Range = (_HitData.Distance / _RayLength + 1.0f) + (_HitData.bBlockingHit * -1);

	if (IsValid(Effect))
	{
		Effect->SetFloatParameter(FName("TracerLifeTime"), Range);
		Effect->SetVectorParameter(FName("StartPoint"), StartLocation);
		Effect->SetVectorParameter(FName("EndPoint"), VectorParam);
	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("SpawnBulletTrail::Can't Create Particle"));
	}

	//Effect->SetFloatParameter(FName("TracerLifetime"), Range);
	//Effect->SetVectorParameter(FName("ShockBeamEnd"), VectorParam);
	// 
	//UParticleSystemComponent* CreateParticle = UGameplayStatics::SpawnEmitterAtLocation
	//(GetWorld(), Particle, StartLocation, UKismetMathLibrary::FindLookAtRotation(StartLocation, _TraceEnd), false);

	//float Range = (_HitData.Distance / _RayLength + 1.0f) + (_HitData.bBlockingHit * -1);
	//
	//CreateParticle->SetFloatParameter(FName("TracerLifetime"), Range);
	//CreateParticle->SetVectorParameter(FName("ShockBeamEnd"), VectorParam);
}

void UWeaponComponent::HitImpulse(FHitResult _HitData)
{
	//UStaticMeshComponent* HitObject = Cast<UStaticMeshComponent>(_HitData.Component);

	//if (IsValid(HitObject))
	//{
	//	if (HitObject->IsSimulatingPhysics())
	//	{
	//		if (HitObject->Mobility == EComponentMobility::Movable)
	//		{
	//			FVector Impulse;
	//			HitObject->AddImpulseAtLocation(Impulse, _HitData.Location);
	//		}
	//	}
	//}
}

void UWeaponComponent::ImpactEffects(FHitResult _HitData)
{
	if(IsValid(WeaponImpactEffect))
	{
		EPhysicalSurface Surface = UGameplayStatics::GetSurfaceType(_HitData);

		//FX
		{
			UParticleSystem* Particle = *(WeaponImpactEffect->ImpactVFX.Find(Surface));

			if ((WeaponImpactEffect->ImpactVFX.Find(Surface)!=nullptr))
			{
				FVector temp = _HitData.ImpactNormal;
				float Rand_f = UKismetMathLibrary::RandomFloatInRange(-180.f, 180.f);

				FRotator Rotation = UKismetMathLibrary::MakeRotator(temp.X + Rand_f, temp.Y, temp.Z);

				FVector Scale = UKismetMathLibrary::MakeVector(1.f, 1.f, 1.f) * UKismetMathLibrary::RandomFloatInRange(0.9f, 1.1f);

				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), Particle, UKismetMathLibrary::MakeTransform(_HitData.Location, Rotation, Scale));

				UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), CurrentEquipWeapon->GunFireParticle, _HitData.Location, FRotator::ZeroRotator, FVector(1), true, true, ENCPoolMethod::AutoRelease, true);
			}
		}
		//Decal
		{
			UMaterialInterface* ImpactDecal = *(WeaponImpactEffect->ImpactDecal.Find(Surface));
			FVector2D ImpactDecalScaleMultiplier = *(WeaponImpactEffect->ImpactDecalScaleMultiplier.Find(Surface));
			FVector ImpactDecalSize = *(WeaponImpactEffect->ImpactDecalSize.Find(Surface));
			int DecalMaxNumberOfVariation = *(WeaponImpactEffect->DecalMaxNumberOfVariations.Find(Surface));

			if ((WeaponImpactEffect->ImpactDecal.Find(Surface))!=nullptr)
			{
				FVector temp = _HitData.ImpactNormal * -1;

				FVector Size = ImpactDecalSize * UKismetMathLibrary::RandomFloatInRange(ImpactDecalScaleMultiplier.X, ImpactDecalScaleMultiplier.Y);

				float Life = 10.0f;

				UDecalComponent* Decal = UGameplayStatics::SpawnDecalAttached(ImpactDecal, Size, _HitData.Component.Get(), _HitData.BoneName, _HitData.Location, temp.Rotation(), EAttachLocation::KeepWorldPosition, Life);

				if (IsValid(Decal))
				{
					//찾�? 못한경우 1
					int maxValue = (WeaponImpactEffect->DecalMaxNumberOfVariations.Find(Surface) == nullptr) ? 1 : DecalMaxNumberOfVariation;

					Decal->CreateDynamicMaterialInstance()->SetScalarParameterValue(FName("Frame"), UKismetMathLibrary::RandomIntegerInRange(1, maxValue));
				}
			}
		}
		//Sound
		{
			USoundCue* ImpactSound = *(WeaponImpactEffect->ImpactSound.Find(Surface));

			if ((WeaponImpactEffect->ImpactSound.Find(Surface)) != nullptr)
			{
				UGameplayStatics::PlaySoundAtLocation(GetWorld(), ImpactSound, _HitData.Location);

				if (IsPlayer)
				{
					if (IsValid(CurrentEquipWeapon))
					{
						if (CurrentEquipWeapon->GetImpactSoundRadius() > 0)
						{
							FVector temp = _HitData.Location;

							UAISense_Hearing::ReportNoiseEvent(GetWorld(), temp, 1, GetOwner(), CurrentEquipWeapon->GetImpactSoundRadius());
							//DrawDebugSphere(GetWorld(), temp, CurrentEquipWeapon->GetImpactSoundRadius(), 16, FColor::Red, true, 5.0f);
						}
					}
				}
			}
		}
	}
}

UCameraComponent* UWeaponComponent::GetADSCamera()
{
	AADS_Camera* AdsCamera = Cast<AADS_Camera>(CurrentEquipWeapon->ADSCameraSystem->GetChildActor());

	if (AdsCamera)
	{
		return AdsCamera->Camera;
	}

	return nullptr;
}

FVector UWeaponComponent::SpreadTrace(FVector _InputTrace)
{
	float SpreadValue = UKismetMathLibrary::MapRangeClamped(CrossHairWidget->Spread,
		CrossHairWidget->MaxSpread, CrossHairWidget->MinSpread, 0.0f, 1.0f);

	float ClampedSpread = UKismetMathLibrary::MapRangeClamped(SpreadValue, 0.0f, 1.0f, CurrentEquipWeapon->WeaponSpread, 0.f);
	float RandMax = ClampedSpread;
	float RandMin = -ClampedSpread;

	static FRandomStream Random;
	static int32 BeforeSeed = Random.GetCurrentSeed();
	if (BeforeSeed == Random.GetCurrentSeed()) Random.GenerateNewSeed();

	BeforeSeed = Random.GetCurrentSeed();

	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("Seed %d"), BeforeSeed));

	FVector OutputTrace;
	OutputTrace.X = _InputTrace.X + Random.FRandRange(RandMin, RandMax);
	OutputTrace.Y = _InputTrace.Y + Random.FRandRange(RandMin, RandMax);
	OutputTrace.Z = _InputTrace.Z + Random.FRandRange(RandMin, RandMax);

	//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("x : %f y : %f z : %f"), OutputTrace.X, OutputTrace.Y, OutputTrace.Z));


	return OutputTrace;
}

void UWeaponComponent::EquipWeapon(EWeaponSlot _Slot)
{
	//?�재 총을 ?�고?�는 ?�롯�?같�??�을경우�??�행
	if (CurrentGunSlot != _Slot)
	{
		//if (IsValid(GunSlot[(int)_Slot]))
		//{
			CanShoot = true;
			CurrentGunSlot = _Slot;
			UpdateCurrentGun();
			CurrentEquipWeapon = GunSlot[(int)CurrentGunSlot];
			//UpdateGunAttachment();
			//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Changed Slot"));
			if (IsValid(GunSlot[(int)_Slot]))
			{
				Texture();
			}

			if (CurrentGunSlot == EWeaponSlot::WeaponSlot1)
			{
				SwapWeapon1 = true;
				SwapWeapon2 = false;
				SwapWeapon3 = false;
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
				MainHUDWidget->RifleOff();
			}

			else if (CurrentGunSlot == EWeaponSlot::WeaponSlot2)
			{
				SwapWeapon1 = false;
				SwapWeapon2 = true;
				SwapWeapon3 = false;
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];

				if (CurrentEquipWeapon->ARType == ESelectorType::Single)
					MainHUDWidget->RifleSingle();
				else if (CurrentEquipWeapon->ARType == ESelectorType::Burst)
					MainHUDWidget->RifleBurst();
				else if (CurrentEquipWeapon->ARType == ESelectorType::FullAuto)
					MainHUDWidget->RifleFullAuto();
			}

			else if (CurrentGunSlot == EWeaponSlot::WeaponSlot3)
			{
				SwapWeapon1 = false;
				SwapWeapon2 = false;
				SwapWeapon3 = true;
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
				MainHUDWidget->RifleOff();
			}
	/*	}*/

		UpdateGunAttachment();

		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Black, TEXT("Changed Slot"));
	}
}

void UWeaponComponent::WeaponShellEjection()
{
	if (CurrentEquipWeapon->ShellEjectionObj)
	{
		FTransform ShellSpawnTransform = CurrentEquipWeapon->SkeletalMesh->GetSocketTransform(CurrentEquipWeapon->ShellEjectionSocket);
		ShellSpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AShellEjectionBase* SpawnedShell = GetWorld()->SpawnActor<AShellEjectionBase>(CurrentEquipWeapon->ShellEjectionObj, ShellSpawnTransform.GetLocation(), ShellSpawnTransform.Rotator(), SpawnParams);

		SpawnedShell->Mesh->SetSimulatePhysics(true);
		SpawnedShell->Mesh->SetCollisionProfileName(FName("DroppedGun"));
		SpawnedShell->SetLifeSpan(10.0f);

		FVector Impulse = 
			(SpawnedShell->Mesh->GetForwardVector() * UKismetMathLibrary::RandomFloatInRange(1.f, 10.f))
			+ (SpawnedShell->Mesh->GetRightVector() * UKismetMathLibrary::RandomFloatInRange(-20.f, -40.f));

		SpawnedShell->Mesh->AddImpulse(Impulse);
	}
}

void UWeaponComponent::WeaponClipEjection()
{
	if (CurrentEquipWeapon->ClipEjectionObj)
	{
		FTransform ClipSpawnTransform = CurrentEquipWeapon->SkeletalMesh->GetSocketTransform(CurrentEquipWeapon->ClipEjectionSocket);
		ClipSpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));

		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();

		AClipEjectionBase* SpawnedClip = GetWorld()->SpawnActor<AClipEjectionBase>(CurrentEquipWeapon->ClipEjectionObj, ClipSpawnTransform.GetLocation(), ClipSpawnTransform.Rotator(), SpawnParams);

		SpawnedClip->Mesh->SetSimulatePhysics(true);
		SpawnedClip->Mesh->SetCollisionProfileName(FName("DroppedGun"));
		SpawnedClip->SetLifeSpan(10.0f);
	}
}

void UWeaponComponent::SpawnBullet()
{
	if (IsValid(CurrentEquipWeapon))
	{
		//GEngine->AddOnScreenDebugMessage(-1, 5, FColor::Red, FString::Printf(TEXT("Bullet")));
		
		float RayLength = 10000.0f;
		PlayWeaponFireFx();
		WeaponSequence();
		
		FHitResult PreResult;
		FVector PreStart = AimPointCamera->GetComponentLocation();
		FVector PreEnd = PreStart + AimPointCamera->GetForwardVector() * RayLength;
		
		if (IsPlayer == false) {
			float tmp = 10.0f * RayLength / 300;
			float x = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			float y = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			float z = UKismetMathLibrary::RandomFloatInRange(-tmp, tmp);
			PreEnd += FVector(x, y, z) + FVector(0, 0, -80.0f);

			AMyCharacter* tChar = Cast<AMyCharacter>(UGameplayStatics::GetPlayerCharacter(GetWorld(), 0));
			if (IsValid(tChar) == true) {
				if (tChar->Get_crocuhState() == false) {
					PreResult.Location += FVector(0, 0, 40.0f);
					// PreEnd += FVector(0, 0, 40.0f);
				}
			}
		}
		GetWorld()->LineTraceSingleByChannel(PreResult, PreStart, PreEnd, ECollisionChannel::ECC_GameTraceChannel2);
		//DrawDebugLine(GetWorld(), PreStart, PreEnd, FColor::Green, false, 10, 0 , 3);

		FHitResult HitResult;
		FVector TraceStart = GetFirePosition().Get<0>();
		FVector TraceEnd = (PreResult.bBlockingHit == false) ? AimPointCamera->GetComponentLocation() + (SpreadTrace(AimPointCamera->GetForwardVector() * RayLength))
			: PreResult.Location + (SpreadTrace(AimPointCamera->GetForwardVector() * (PreResult.Distance / RayLength)));

		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("x : %f y : %f z : %f"), TraceEnd.X, TraceEnd.Y, TraceEnd.Z));
		//DrawDebugLine(GetWorld(), TraceStart, TraceEnd, FColor::Red, false, 10, 0 , 2);
		
		if (GetWorld()->LineTraceSingleByChannel(HitResult, TraceStart, TraceEnd, ECollisionChannel::ECC_GameTraceChannel2))
		{
			ImpactEffects(HitResult);
			HitImpulse(HitResult);
			
			float WeaponBaseDamage = UKismetMathLibrary::RandomFloatInRange(CurrentEquipWeapon->WeaponMinDamage, CurrentEquipWeapon->WeaponMaxDamage);

			if (IsPlayer == true) {
				AMyCharacterAI* Character = Cast<AMyCharacterAI>(HitResult.GetActor());
				if (Character) {
					if (HitResult.BoneName == FName("head")) {
						Character->GetDamage(Character->HP_Max);
						CrossHairWidget->Hit->SetContentColorAndOpacity(FLinearColor::Red);
					}
					else {
						Character->GetDamage(WeaponBaseDamage);
						CrossHairWidget->Hit->SetContentColorAndOpacity(FLinearColor(1.0f, 0.25f, 0.25f));
						UAISense_Damage::ReportDamageEvent(GetWorld(), Character, UGameplayStatics::GetPlayerPawn(GetWorld(), 0), 0.0f, HitResult.TraceStart, HitResult.Location);
					}
					//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s %f"), *Character->GetName(), Character->HP));
					
					CrossHairWidget->Hit->SetRenderOpacity(1.0f);
				}
			}
			else {
				AMyCharacter* Player = Cast<AMyCharacter>(HitResult.GetActor());
				if (Player) {
					IsDamage = true;
					Player->GetDamage(WeaponBaseDamage);

					HPopacity = Player->HP / Player->HP_Max;

					if (IsValid(MainHUDWidget))
					{
						MainHUDWidget->SetHP(Player->HP / Player->HP_Max);
					}
					if (IsValid(DamageHUDWidget))
					{
						if (IsDamage)
						{
							/*if (RandomNumber == 0)
								DamageHUDWidget->SetVisi();
							else if (RandomNumber == 1)
								DamageHUDWidget->SetVisi2();*/

							//DamageHUDWidget->SetOp(Player->HP / Player->HP_Max);
							DamageHUDWidget->SetOp(1.0f- HPopacity);
						}
					}

					/*if (IsValid(PauseHUDWidget))
					{
						if (Player->HP == 0.0f)
							PauseHUDWidget->DeadPauseOn();
					}*/
					IsDamage = false;
					//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s %f"), *Character->GetName(), Character->HP));
				}
				
				
			}
			//UGameplayStatics::ApplyDamage(HitResult.GetActor(), WeaponBaseDamage, GetOwner()->GetInstigatorController(), CurrentEquipWeapon, nullptr);
		}
		
		static FRandomStream Random;
		static int32 BeforeSeed = Random.GetCurrentSeed();
		if (BeforeSeed == Random.GetCurrentSeed()) Random.GenerateNewSeed();

		BeforeSeed = Random.GetCurrentSeed();

		bool Check=UKismetMathLibrary::RandomBoolFromStream(Random);
		
		if(Check==true) SpawnBulletTrail(HitResult, TraceEnd, RayLength);
	}
}

void UWeaponComponent::PlayWeaponReloadAnim()
{
	if (IsValid(CurrentEquipWeapon->GunReloadAnim))
	{
		CurrentEquipWeapon->SkeletalMesh->PlayAnimation(CurrentEquipWeapon->GunReloadAnim, false);
	}
}

void UWeaponComponent::PlayWeaponFireAnim()
{
	if (IsValid(CurrentEquipWeapon->GunFireAnim))
	{
		CurrentEquipWeapon->SkeletalMesh->PlayAnimation(CurrentEquipWeapon->GunFireAnim, false);
	}
}

void UWeaponComponent::PlayWeaponFireFx()
{
	if (IsValid(CurrentEquipWeapon))
	{
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentEquipWeapon->GetFireSound(), CurrentEquipWeapon->GetMuzzlePosition().Get<0>());

		UAISense_Hearing::ReportNoiseEvent(GetWorld(), CurrentEquipWeapon->GetMuzzlePosition().Get<0>(), 1, GetOwner(), CurrentEquipWeapon->GetFireSoundRadius());
		//DrawDebugSphere(GetWorld(), CurrentEquipWeapon->GetMuzzlePosition().Get<0>(), CurrentEquipWeapon->GetFireSoundRadius(), 16, FColor::Red, true, 5.0f);
		
		UNiagaraFunctionLibrary::SpawnSystemAttached(CurrentEquipWeapon->GunFireParticle, CurrentEquipWeapon->SkeletalMesh, CurrentEquipWeapon->GetMuzzleSocketName()
			, FVector::ZeroVector, FRotator::ZeroRotator, EAttachLocation::Type::KeepRelativeOffset, true, true, ENCPoolMethod::AutoRelease, true);
	}
}

void UWeaponComponent::WeaponReload()
{
	if (ReloadCheck() && !IsReloading)
	{
		FVector SoundLocation = GetOwner()->GetActorLocation();
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), CurrentEquipWeapon->GunReloadSound, SoundLocation);

		SetLaserSightToFront(true);

		IWeaponToOwner* Interface = Cast<IWeaponToOwner>(GetOwner());
		if (Interface)
		{
			Interface->PlayReloadAnimation();
		}


		if (true == ADSMode)
		{
			KeepADSMode = true;

			AMyCharacter* Player = Cast<AMyCharacter>(GetOwner());

			if (IsValid(Player))
			{
				//Disable FPP Mode
				Player->FPP_Mode();
			}
		}

	}
	else
	{
		//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Can't Reload"));
	}
}

void UWeaponComponent::WeaponSelectorChange()
{
	if (!IsShooting && IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
		{
			AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

			Rifle->SelectorChange();

			if (Rifle->Selector == ESelectorType::Single)
			{
				CurrentEquipWeapon->ARType = ESelectorType::Single;
				MainHUDWidget->RifleSingle();
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Single"));
			}
			else if (Rifle->Selector == ESelectorType::Burst)
			{
				CurrentEquipWeapon->ARType = ESelectorType::Burst;
				MainHUDWidget->RifleBurst();
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("Burst"));
			}

			else if (Rifle->Selector == ESelectorType::FullAuto)
			{
				CurrentEquipWeapon->ARType = ESelectorType::FullAuto;
				MainHUDWidget->RifleFullAuto();
				//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("FullAuto"));
			}
			ReleaseFire();
		}
	}
}

void UWeaponComponent::BurstFire()
{
	static int n = 0;
	if (this == nullptr)
	{
		return;
	}
	if (false==IsPlayer)
	{
		if (nullptr == UGameplayStatics::GetPlayerCharacter(GetWorld(), 0))
		{
			CanUseBurstShot = true;
			IsShooting = false;
			n = 0;
			return;
		}
	}

	if (IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
		{
			AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

			if (CanShoot && n < 3)
			{
				if (Rifle->Selector == ESelectorType::Burst)
				{
					float InDelayTime = CurrentEquipWeapon->AttackSpeed;

					IsShooting = true;

					CanUseBurstShot = false;

					SpawnBullet();

					if (IsPlayer) {
						CurrentEquipWeapon->OnFire();
					}
					n++;
					
					GetWorld()->GetTimerManager().SetTimer(TH, this, &UWeaponComponent::BurstFire, InDelayTime, false);
				}
			}
			else
			{
				CurrentEquipWeapon->StopFire();
				CanUseBurstShot = true;
				IsShooting = false;
				n = 0;
			}
		}
	}
}

void UWeaponComponent::GunFire()
{
	if (IsValid(CurrentEquipWeapon))
	{
		if (IsOwnerPlayFireAnimation == true) return;

		CheckCanShoot();

		if (CanShoot)
		{
			switch (CurrentEquipWeapon->WeaponType)
			{
			case EWeaponType::Pistol:
			{
				SpawnBullet();
			}
			break;
			case EWeaponType::AssultRifle:
			{
				// Rifle->Selector == ESelectorType::Burst
				// Rifle->Selector == ESelectorType::FullAuto
				AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);
				
				if (CurrentEquipWeapon->ARType == ESelectorType::Burst) {
					if (CanUseBurstShot) BurstFire();
				}
				else if (CurrentEquipWeapon->ARType == ESelectorType::FullAuto) {
					FullAutoFire();
				}
				else {
					SpawnBullet();
				}
			}
			break;
			case EWeaponType::SniperRifle:
			{
				SpawnBullet();
			}
			break;
			}
		}
		else
		{
			if (CurrentEquipWeapon->GetCurrClip() <= 0)
			{
				if (ReloadCheck())
				{
					//ReloadGun();

					WeaponReload();
				}
				else
				{
					//GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Blue, TEXT("NoBullet"));

					USoundCue* NoAmmoSound = CurrentEquipWeapon->NoAmmoSound;

					if (IsValid(NoAmmoSound))
					{
						UGameplayStatics::PlaySoundAtLocation(GetWorld(), NoAmmoSound, CurrentEquipWeapon->GetActorLocation());
					}
				}
			}
		}
	}
}

void UWeaponComponent::FullAutoFire()
{
	if (IsValid(CurrentEquipWeapon))
	{
		if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
		{
			AARBase* Rifle = Cast<AARBase>(CurrentEquipWeapon);

			if (Rifle->Selector == ESelectorType::FullAuto)
			{
				IsShooting = true;
				FullAutoFireRetrigger();
			}
		}
	}
}

void UWeaponComponent::FullAutoFireRetrigger()
{
	if (nullptr == UGameplayStatics::GetPlayerCharacter(this, 0))
	{
		return;
	}
	if (IsValid(CurrentEquipWeapon))
	{
		if (IsShooting && CanShoot)
		{
			float InDelayTime = CurrentEquipWeapon->AttackSpeed;

			SpawnBullet();

			CurrentEquipWeapon->OnFire();

			GetWorld()->GetTimerManager().SetTimer(TH2, this, &UWeaponComponent::FullAutoFireRetrigger, InDelayTime, false);
		}
		else
		{
			if (IsShooting) WeaponReload();

			IsShooting = false;

			CurrentEquipWeapon->StopFire();
		}
	}
}

void UWeaponComponent::ReleaseFire()
{
	IsShooting = false;
}

void UWeaponComponent::WeaponSequence()
{
	if (IsValid(CurrentEquipWeapon))
	{
		CurrentEquipWeapon->DecreaseAmmo();
		CheckCanShoot();
		UpdateUI();
		WeaponShellEjection();
		CurFireCoolTime = CurrentEquipWeapon->AttackSpeed;

		if (IsPlayer)
		{
			if (CurrentEquipWeapon->WeaponType == EWeaponType::Pistol)
			{
				ClipNum = CurrentEquipWeapon->CurrClip;
				ClipAllNum = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::AssultRifle)
			{
				ClipNum2 = CurrentEquipWeapon->CurrClip;
				ClipAllNum2 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}
			else if (CurrentEquipWeapon->WeaponType == EWeaponType::SniperRifle)
			{
				ClipNum3 = CurrentEquipWeapon->CurrClip;
				ClipAllNum3 = Ammo[CurrentEquipWeapon->AmmoIndex];
			}

			if (IsValid(CurrentEquipWeapon->FiringShake))
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CurrentEquipWeapon->FiringShake);
			}

			if (IsValid(CurrentEquipWeapon->RecoilShake))
			{
				GetWorld()->GetFirstPlayerController()->ClientStartCameraShake(CurrentEquipWeapon->RecoilShake);
			}

			AddCrossHairSpread(CurrentEquipWeapon->CrossSpreadAmount);
		}

		AMyCharacter* Character = Cast<AMyCharacter>(GetOwner());

		if (IsValid(Character))
		{
			if (!(CurrentEquipWeapon->WeaponType == EWeaponType::SniperRifle && ADSMode == true))
			{
				Character->PlayOwnerFireAnimation(CurrentEquipWeapon->WeaponType);
			}
		}
	}
}

void UWeaponComponent::ReloadAnimationComplete()
{
	ReloadGun();

	IsReloading = false;

	SetLaserSightToFront(false);

	if (true == IsPlayer)
	{
		if (true == KeepADSMode)
		{
			KeepADSMode = false;
			AMyCharacter* Player = Cast<AMyCharacter>(GetOwner());
			
			if (IsValid(Player))
			{
				//Resume FPP Mode
				Player->FPP_Mode();
			}
		}
	}
}

void UWeaponComponent::SlotChangeAnimationComplete()
{
	CurFireCoolTime = 0.f;
}

void UWeaponComponent::OwnerFireAnimationComplete()
{
}

void UWeaponComponent::Texture()
{
	if (CurrentGunSlot == EWeaponSlot::WeaponSlot1)
	{
		MainHUDWidget->SetWeapon(CurrentEquipWeapon->WeaponHUD, CurrentEquipWeapon->ItemName);
		Text1 = CurrentEquipWeapon->ItemName;
		//MainHUDWidget->SetWeapon
	}
	else if (CurrentGunSlot == EWeaponSlot::WeaponSlot2)
	{
		MainHUDWidget->SetWeapon2(CurrentEquipWeapon->WeaponHUD, CurrentEquipWeapon->ItemName);
		Text2 = CurrentEquipWeapon->ItemName;
	}
	else if (CurrentGunSlot == EWeaponSlot::WeaponSlot3)
	{
		MainHUDWidget->SetWeapon3(CurrentEquipWeapon->WeaponHUD, CurrentEquipWeapon->ItemName);
		Text3 = CurrentEquipWeapon->ItemName;

	}
	MainHUDWidget->ExistingWeaponOn();
}

void UWeaponComponent::SwapTexture()
{
	MainHUDWidget->BackGroundVisi(WeaponText1,WeaponText2,WeaponText3, Text1, Text2, Text3);
}

void UWeaponComponent::ReloadFail()
{
	IsReloading = false;
}
