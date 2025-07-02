// Fill out your copyright notice in the Description page of Project Settings.


#include "GunBase.h"
#include "Sound/SoundBase.h"
#include "GameFramework/Actor.h"
#include "Camera/CameraComponent.h"
#include "Components/SphereComponent.h"
#include "Components/ChildActorComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "ADS_Camera.h"
#include "SupressorBase.h"
#include "OpticsBase.h"
#include "LaserSightBase.h"
#include "MyCharacter.h"
#include "Particles/ParticleSystem.h"
#include "Particles/ParticleSystemComponent.h"
#include "WeaponComponent.h"

#include "ShellEjectionBase.h"
#include "ClipEjectionBase.h"
#include "Train_Between.h"
#include "Train_Rail.h"


AGunBase::AGunBase()
{
	static ConstructorHelpers::FClassFinder<AADS_Camera> ADS(TEXT("/Script/CoreUObject.Class'/Script/Specter.ADS_Camera'"));
	static ConstructorHelpers::FObjectFinder<UMaterial> RenderMat(TEXT("/Script/Engine.Material'/Game/WeaponSystem/Item/M_HighLightMat.M_HighLightMat'"));
	
	if (ADS.Succeeded())
	{
		ADSCameraSystem = CreateDefaultSubobject<UChildActorComponent>(TEXT("ADSCamera"));
		ADSCameraSystem->SetupAttachment(SkeletalMesh);
		ADSCameraSystem->SetChildActorClass(ADS.Class);
	}

	if (RenderMat.Succeeded())
	{
		RenderingMat = RenderMat.Object;
	}

	GunRecoilCurvePitch = nullptr;
	GunRecoilCurveYaw = nullptr;
	GunRecoilTimelineLength = 3.5f;

	MaxClip = 30;
	CurrClip = 0;

	Attachments.Init(nullptr, (int)EAttachmentType::End);

	RecoilMaxThresHold = 25.0f;
	RecoilRate = 5.0f;

	//static ConstructorHelpers::FObjectFinder<UParticleSystem> Particle(TEXT("/Game/ThirdPersonKit/Particles/P_BulletTracer.P_BulletTracer"));

	//if (Particle.Succeeded())
	//{
	//	TrailParticle = Particle.Object;
	//}
	
	ShellEjectionObj = AShellEjectionBase::StaticClass();

	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve1(TEXT("/Game/WeaponSystem/Item/Guns/GunRecoil_PitchCurve.GunRecoil_PitchCurve"));
	static ConstructorHelpers::FObjectFinder<UCurveFloat> Curve2(TEXT("/Game/WeaponSystem/Item/Guns/GunRecoil_YawCurve.GunRecoil_YawCurve"));

	if (Curve1.Succeeded())
	{
		GunRecoilCurvePitch = Curve1.Object;
	}

	if (Curve2.Succeeded())
	{
		GunRecoilCurveYaw = Curve2.Object;
	}

	static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeData1(TEXT("/Game/WeaponSystem/Component/BP_FiringShake.BP_FiringShake_C"));
	static ConstructorHelpers::FClassFinder<UCameraShakeBase> ShakeData2(TEXT("/Game/WeaponSystem/Component/BP_RecoilShake.BP_RecoilShake_C"));

	if (ShakeData1.Succeeded())
	{
		FiringShake = ShakeData1.Class;
	}

	if (ShakeData2.Succeeded())
	{
		RecoilShake = ShakeData2.Class;
	}

	LaserClass = ALaserSightBase::StaticClass();
	OpticsClass = AOpticsBase::StaticClass();
	SupressorClass = ASupressorBase::StaticClass();
}

void AGunBase::BeginPlay()
{
	AItemBase::BeginPlay();

	if (GunRecoilCurvePitch != nullptr && GunRecoilCurveYaw != nullptr)
	{
		FOnTimelineFloat CurvePitchCallback;
		FOnTimelineFloat CurveYawCallback;
		FOnTimelineEvent TimelineFinishedCallback;

		CurvePitchCallback.BindUFunction(this, FName("GunRecoilUpdate"));
		CurveYawCallback.BindUFunction(this, FName("GunRecoilUpdate"));
		TimelineFinishedCallback.BindUFunction(this, FName("GunRecoilFinished"));

		GunRecoilTimeline.SetDirectionPropertyName(FName("TimelineDirection"));

		GunRecoilTimeline.AddInterpFloat(GunRecoilCurveYaw, CurvePitchCallback);
		GunRecoilTimeline.AddInterpFloat(GunRecoilCurvePitch, CurveYawCallback);
		GunRecoilTimeline.SetTimelineFinishedFunc(TimelineFinishedCallback);
		GunRecoilTimeline.SetTimelineLength(GunRecoilTimelineLength);
	}

	DropGun();

	CurrClip = MaxClip;

	WeaponDataInit(WeaponData);
	SocketDataInit(SocketData);

	Init_OnCreate();
}

void AGunBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	GunRecoilTimeline.TickTimeline(DeltaTime);
	if (IsValid(Actor_Train) == true) {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%s %s"), *this->GetName(), *Actor_Train->GetName()));
	}
	else {
		//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%s No Actor Train"), *this->GetName()));
	}
}

void AGunBase::Calc(float DeltaTime) {
	if (UseTrain == true) {
		if (UseGun == true) {
			//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Red, FString::Printf(TEXT("%s"), *this->GetName()));
			SetActorRelativeLocation(FVector::Zero(), false, nullptr, ETeleportType::None);
			TrainComponent->SetRelativeLocation(FVector::Zero(), false, nullptr, ETeleportType::None);
		}
		else {
			FVector CompLocation = GetActorLocation();
			FHitResult Hit;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			CollisionParams.AddIgnoredActor(GetOwner());
	
			if (IsValid(Actor_Train) == true) {
				FRotator tVecRot = Actor_Train->Rail_Train->GetActorRotation() + FRotator(0, 90, 0);
				FVector tVec = Actor_Train->Rail_Train->GetActorLocation() - tVecRot.RotateVector(Actor_Train->GetActorLocation() - GetActorLocation());
				FRotator tRot;
				tRot.Roll = GetActorRotation().Roll;
				tRot.Pitch = GetActorRotation().Pitch;
				tRot.Yaw = Actor_Train->Rail_Train->GetActorRotation().Yaw + GetActorRotation().Yaw + 90;
				TrainComponent->SetWorldLocation(tVec, false, nullptr, ETeleportType::TeleportPhysics);
				TrainComponent->SetWorldRotation(tRot, false, nullptr, ETeleportType::TeleportPhysics);
				CompLocation = TrainComponent->GetComponentLocation();
				
				FRotator RotTrain = Actor_Train->Rail_Train->GetActorRotation() + FRotator(0, 90, 0);
				//RotTrain *= -1;
				FVector Reposition = GetActorLocation() - RotTrain.RotateVector(TrainComponent->GetComponentLocation() - SkeletalMesh->GetComponentLocation());
				Sphere->SetWorldLocation(Reposition, false, nullptr, ETeleportType::None);
				//GEngine->AddOnScreenDebugMessage(-1, 50, FColor::Black, FString::Printf(TEXT("%s %s %s"), *Reposition.ToString(), *(TrainComponent->GetComponentLocation() - SkeletalMesh->GetComponentLocation()).ToString(), *GetActorLocation().ToString()));
				
				// FRotator tVecRot2 = Actor_Train->Rail_Train->GetActorRotation() + FRotator(0, 90, 0);
				// FVector tVec2 = Actor_Train->Rail_Train->GetActorLocation() - tVecRot2.RotateVector(Actor_Train->GetActorLocation() - GetActorLocation());
				// Sphere->SetRelativeLocation(tVec2, false, nullptr, ETeleportType::None);
			}
			else {
				TrainComponent->SetWorldLocation(GetActorLocation());
				Sphere->SetRelativeLocation(FVector::Zero(),false, nullptr, ETeleportType::None);
			}
	
			if (GetWorld()->LineTraceSingleByChannel(Hit, CompLocation, CompLocation - FVector(0, 0, 100), ECollisionChannel::ECC_GameTraceChannel2, CollisionParams)) {
				//GEngine->AddOnScreenDebugMessage(-1, DeltaTime, FColor::Blue, FString::Printf(TEXT("%s %s"), *this->GetName(), *Hit.GetActor()->GetName()));
				if (Hit.GetActor()->IsA(ATrain_Between::StaticClass()) == true) {
					if (Actor_Train != Hit.GetActor()) {
						Actor_Train = Cast<ATrain_Between>(Hit.GetActor());
					}
				}
				else if (Hit.GetActor()->IsA(ATrain_Rail::StaticClass()) == true) {
					TObjectPtr<ATrain_Rail> TR = Cast<ATrain_Rail>(Hit.GetActor());
					if (Actor_Train != TR->Train_Between) {
						FRotator RotTrain = TR->GetActorRotation() + FRotator(0, 90, 0);
						RotTrain *= -1;
						FVector Reposition = TR->Train_Between->GetActorLocation() - RotTrain.RotateVector(TR->GetActorLocation() - TrainComponent->GetComponentLocation());
						SetActorLocation(Reposition, false, nullptr, ETeleportType::None);
						Actor_Train = TR->Train_Between;
						//DrawDebugCircle(GetWorld(), Reposition, 1000, 20, FColor::Green, false, 100, 0, 10);
	
						FRotator tVecRot = Actor_Train->Rail_Train->GetActorRotation() + FRotator(0, 90, 0);
						FVector tVec = Actor_Train->Rail_Train->GetActorLocation() - tVecRot.RotateVector(Actor_Train->GetActorLocation() - GetActorLocation());
						FRotator tRot;
						tRot.Roll = GetActorRotation().Roll;
						tRot.Pitch = GetActorRotation().Pitch;
						tRot.Yaw = Actor_Train->Rail_Train->GetActorRotation().Yaw + GetActorRotation().Yaw + 90;
						TrainComponent->SetWorldLocation(tVec, false, nullptr, ETeleportType::TeleportPhysics);
						TrainComponent->SetWorldRotation(tRot, false, nullptr, ETeleportType::TeleportPhysics);
					}
				}
			}
	
			if (IsValid(PreActor) == false) {
				if (IsValid(Actor_Train) == true) {
					TObjectPtr<ATrain_Rail> PreTick = Actor_Train->Rail_Train;
					while(true) {
						if (IsValid(PreTick) == true) {
							if (IsValid(PreTick->Back_Train) == true) {
								PreTick = PreTick->Back_Train;
							}
							else {
								break;
							}
						}
						else {
							break;
						}
					}
					PreActor = PreTick;
					AddTickPrerequisiteActor(PreActor);
				}
			}
		}
	}
	
}

void AGunBase::WeaponDataInit(UPDA_WeaponData* _Data)
{
	if (IsValid(_Data))
	{
		ItemName = _Data->WeaponName;
		SkeletalMesh->SetSkeletalMesh(_Data->WeaponMesh);
		WeaponType = _Data->WeaponType;
		WeaponSlot = _Data->WeaponSlot;
		WeaponHUD = _Data->WeaponHUD;
		AmmoIndex = _Data->AmmoIndex;
		MaxClip = _Data->MaxClip;

		GunReloadSound = _Data->GunReloadSound;
		//GunReloadAnim = _Data->GunReloadAnim;
		GunFireSound = _Data->GunFireSound;
		//GunFireAnim = _Data->GunFireAnim;
		GunFireParticle = _Data->GunFireParticle;
		ShellEjectionObj = _Data->ShellEjectionObj;
		MagazineObj = _Data->MagazineObj;

		WeaponSpread = _Data->WeaponSpread;
		WeaponMinDamage = _Data->WeaponDamage.X;
		WeaponMaxDamage = _Data->WeaponDamage.Y;
		ImpactSoundRadius = _Data->ImpactSoundRadius;
		FireSoundRadius = _Data->FireSoundRadius;

		GunRecoilCurvePitch = _Data->GunRecoilCurvePitch;
		GunRecoilCurveYaw = _Data->GunRecoilCurveYaw;
		GunRecoilTimelineLength = _Data->GunRecoilTimelineLength;

		TrailParticle = _Data->TrailParticle;
	}
}

void AGunBase::SocketDataInit(UPDA_GunSocketData* _Data)
{
	if (IsValid(_Data))
	{
		MuzzleFlashName = _Data->MuzzleFlashName;
		SupressorMuzzleFlashName = _Data->SupressorMuzzleFlashName;
		AimingCameraLocation = _Data->AimingCameraLocation;
		AttachmentSocketLaser = _Data->AttachmentSocketLaser;
		AttachmentSocketSupressor = _Data->AttachmentSocketSupressor;
		AttachmentSocketScope = _Data->AttachmentSocketScope;
		BaseSocketScope = _Data->BaseSocketScope;
		BulletShellSpawnLocation = _Data->BulletShellSpawnLocation;
		HandGripTransform = _Data->HandGripTransform;
		Magazine = _Data->Magazine;

	}
}

void AGunBase::OnInteract_Implementation(AMyCharacter* Target)
{
	SetOwner(Target);
	Target->GetWeaponComponent()->PickUpGun(this);
}

void AGunBase::ChangeState(EGunState newGunState)
{
	GunState = newGunState;

	if (GunState == EGunState::Dropped)
	{
		Sphere->SetCollisionProfileName(FName(TEXT("OverlapAllDynamic")), true);
		SkeletalMesh->SetSimulatePhysics(true);
		SkeletalMesh->SetCollisionProfileName(FName(TEXT("DroppedGun")), true);
		HighLightOn();

		if (UseTrain == true) {
			SkeletalMesh->SetSimulatePhysics(false);
		}
	}
	else if (GunState == EGunState::Equipped)
	{
		Sphere->SetCollisionProfileName(FName(TEXT("NoCollision")), false);
		SkeletalMesh->SetSimulatePhysics(false);
		SkeletalMesh->SetCollisionProfileName(FName(TEXT("NoCollision")), false);
		SkeletalMesh->ResetRelativeTransform();
		HighLightOff();
	}
}

void AGunBase::DropGun() {
	if (IsValid(GetAttachParentActor()) == true) {
		//GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%s"), *GetAttachParentActor()->GetName()));
	}
	if (IsValid(Owner) == true) {
		SetActorLocation(Owner->GetActorLocation(), false,  nullptr, ETeleportType::None);
	}
	
	if (UseTrain == true) {
		FHitResult Hit;
		FCollisionQueryParams CollisionParams;
		CollisionParams.AddIgnoredActor(this);
		CollisionParams.AddIgnoredActor(GetOwner());
		
		if (GetWorld()->LineTraceSingleByChannel(Hit, GetActorLocation(), GetActorLocation() - FVector(0, 0, 1000), ECollisionChannel::ECC_GameTraceChannel2, CollisionParams)) {
			SetActorLocation(Hit.Location, true, nullptr, ETeleportType::None);
		}
	}
	
	UseGun = false;
	ChangeState(EGunState::Dropped);
	SetOwner(nullptr);
	DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	SetVisualLaserSight(false);

	if (IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight))
	{
		SetFlashLight(IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight));
	}
}

void AGunBase::DeleteGun()
{
	for (AAttachmentBase* Actor : Attachments)
	{
		if (IsValid(Actor))
		{
			Actor->Destroy();
		}
	}

	Destroy();
}

void AGunBase::HideGun(bool _flag)
{
	SkeletalMesh->SetVisibility(_flag);

	for (AAttachmentBase* Actor : Attachments)
	{
		if (IsValid(Actor))
		{
			Actor->AttachmentMesh->SetVisibility(_flag);
		}
	}
}

TTuple<FVector, FRotator> AGunBase::GetMuzzlePosition()
{
	FVector MuzzleLocation;
	FRotator MuzzleRotator;

	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		MuzzleLocation = SkeletalMesh->GetSocketLocation(SupressorMuzzleFlashName);
		MuzzleRotator = SkeletalMesh->GetSocketRotation(SupressorMuzzleFlashName);
	}
	else
	{
		MuzzleLocation = SkeletalMesh->GetSocketLocation(MuzzleFlashName);
		MuzzleRotator = SkeletalMesh->GetSocketRotation(MuzzleFlashName);
	}

	return MakeTuple(MuzzleLocation, MuzzleRotator);
}

FName AGunBase::GetMuzzleSocketName()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return SupressorMuzzleFlashName;
	}

	return MuzzleFlashName;
}

void AGunBase::DecreaseAmmo()
{
	CurrClip = FMath::Clamp(CurrClip - 1, 0, MaxClip);
}

void AGunBase::SetVisualMagazine(bool to_hide)
{
	if (to_hide)
	{
		SkeletalMesh->HideBoneByName(Magazine, EPhysBodyOp::PBO_None);
	}
	else
	{
		SkeletalMesh->UnHideBoneByName(Magazine);
	}
}

float AGunBase::GetImpactSoundRadius()
{
	return ImpactSoundRadius;
}

float AGunBase::GetFireSoundRadius()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return temp->FireSoundRadius;
	}

	return FireSoundRadius;
}

int AGunBase::GetMaxClip()
{
	return MaxClip;
}

int AGunBase::GetCurrClip()
{
	return CurrClip;
}

void AGunBase::SetCurrClip(int _num)
{
	CurrClip = _num;
}

USoundBase* AGunBase::GetFireSound()
{
	ASupressorBase* temp = IsAttachmentExist<ASupressorBase>(EAttachmentType::Supressor);

	if (IsValid(temp))
	{
		return temp->GetSupressorFireSound(WeaponType);
	}

	return GunFireSound;
}

void AGunBase::SpawnMagazine()
{
	if (ClipEjectionObj)
	{
		FTransform ClipTransform = SkeletalMesh->GetSocketTransform(ClipEjectionSocket);
		ClipTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = GetOwner();
		AClipEjectionBase* SpawnedMagazine = GetWorld()->SpawnActor<AClipEjectionBase>(ClipEjectionObj, ClipTransform.GetLocation(), ClipTransform.Rotator(), SpawnParams);
		SpawnedMagazine->Mesh->SetSimulatePhysics(true);
		SpawnedMagazine->Mesh->SetCollisionProfileName(FName("DroppedGun"));
		SpawnedMagazine->SetLifeSpan(10.0f);
	}
}

void AGunBase::AddControllerPitchAndYaw(float _pitch, float _yaw)
{
	APlayerController* PlayerController = GetWorld()->GetFirstPlayerController();

	PlayerController->AddPitchInput(_pitch);
	PlayerController->AddYawInput(_yaw);
}

void AGunBase::UpdateAdsCameraPosition()
{
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());
	ADS_Camera->Camera->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, AimingCameraLocation);

	AOpticsBase* Optic = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);

	if (IsValid(Optic))
	{
		ADS_Camera->Camera->AttachToComponent(Optic->AttachmentMesh, FAttachmentTransformRules::KeepRelativeTransform, Optic->AimingCameraSocket);
	}
}

AADS_Camera* AGunBase::GetAdsCamera()
{
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	return ADS_Camera;
}

void AGunBase::SetVisualLaserSight(bool _On)
{
	ALaserSightBase* Laser = IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);

	if (IsValid(Laser))
	{
		Laser->SetLaserSightVisual(_On);
	}
}

void AGunBase::SetFlashLight(AAttachmentBase* obj)
{
	K2_SetFlashLight(obj);
}

void AGunBase::UpdateLaserSight(bool _hit, FVector _Location)
{
	ALaserSightBase* Laser = IsAttachmentExist<ALaserSightBase>(EAttachmentType::LaserSight);
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	if (IsValid(Laser))
	{
		if (IsValid(Laser->LaserSightComponent))
		{
			FVector LaserStartLocation = Laser->GetLaserStartSocket();
			FVector TraceEndLocation = _Location;

			FHitResult HitResult;
			FCollisionQueryParams CollisionParams;
			CollisionParams.AddIgnoredActor(this);
			CollisionParams.AddIgnoredActor(Laser);

			GetWorld()->LineTraceSingleByChannel(HitResult, LaserStartLocation, TraceEndLocation, ECollisionChannel::ECC_GameTraceChannel2, CollisionParams);

			FVector LaserEndLocation = (HitResult.bBlockingHit ? HitResult.Location : _Location);

			Laser->LaserSightComponent->SetBeamSourcePoint(0, LaserStartLocation, 0);
			Laser->LaserSightComponent->SetBeamTargetPoint(0, LaserEndLocation, 0);
		}
	}
}

FVector AGunBase::GetAdsCameraOffset()
{
	FVector Offset = { 0, 0, 0 };

	AOpticsBase* Optics = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);

	if (IsValid(Optics))
	{
		Offset = Optics->CameraOffset;
	}

	return Offset;
}

void AGunBase::UpdateCameraFOV()
{
	AOpticsBase* Optics = IsAttachmentExist<AOpticsBase>(EAttachmentType::Optics);
	AADS_Camera* ADS_Camera = Cast<AADS_Camera>(ADSCameraSystem->GetChildActor());

	float Fov = DefaultCameraFOV;

	if (IsValid(Optics))
	{
		Fov = Optics->CameraFOV;
	}

	ADS_Camera->SettingCameraFOV(Fov);
}

void AGunBase::AddAttachment(EAttachmentType _type)
{
	if (IsValid(IsAttachmentExist<AAttachmentBase>(_type)))
	{
		RemoveAttachment(_type);
	}

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = GetOwner();

	TSubclassOf<AAttachmentBase> SpawnClass;
	FName AttachSocket;

	switch (_type)
	{
	case EAttachmentType::Optics:
		SkeletalMesh->HideBoneByName(BaseSocketScope, PBO_None);
		SpawnClass = OpticsClass;
		AttachSocket = AttachmentSocketScope;
		break;
	case EAttachmentType::Supressor:
		SpawnClass = SupressorClass;
		AttachSocket = AttachmentSocketSupressor;
		break;
	case EAttachmentType::LaserSight:
		SpawnClass = LaserClass;
		AttachSocket = AttachmentSocketLaser;
		break;
	}


	if (SpawnClass != nullptr)
	{
		AAttachmentBase* NewAttachment = GetWorld()->SpawnActor<AAttachmentBase>(SpawnClass, SpawnParams);
		NewAttachment->AttachToComponent(SkeletalMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachSocket);
		Attachments[(int)_type] = NewAttachment;
		NewAttachment->Init_OnCreate();
	}
}

void AGunBase::RemoveAttachment(EAttachmentType _type)
{
	AAttachmentBase* RemoveAttachment = IsAttachmentExist<AAttachmentBase>(_type);
	
	if (IsValid(RemoveAttachment))
	{
		RemoveAttachment->Destroy();
	}
}

void AGunBase::OnFire()
{
	GunRecoilTimeline.SetPlayRate(1.f);
	TimelineDirection = ETimelineDirection::Forward;

	if (RecoilPoint1.Execute())
	{
		OriginRotation=GetWorld()->GetFirstPlayerController()->GetControlRotation();
	}

	GunRecoilTimeline.Play();
}

void AGunBase::StopFire()
{
	GunRecoilTimeline.SetPlayRate(RecoilRate);
	TimelineDirection = ETimelineDirection::Backward;
	GunRecoilTimeline.Reverse();
}

void AGunBase::ResetRecoil()
{
	RecoilPoint1.Reset();
	RecoilPoint2.Reset();
}

void AGunBase::GunRecoilUpdate()
{
	float TimelineValue = GunRecoilTimeline.GetPlaybackPosition();

	float PitchAmount = GunRecoilCurvePitch->GetFloatValue(TimelineValue);
	float YawAmount = GunRecoilCurveYaw->GetFloatValue(TimelineValue);



	if (TimelineDirection.GetValue() == ETimelineDirection::Forward)
	{
		AddControllerPitchAndYaw(PitchAmount, YawAmount);
	}
	else if (TimelineDirection.GetValue() == ETimelineDirection::Backward)
	{
		if (RecoilPoint2.Execute())
		{
			PostRecoilRotation = GetWorld()->GetFirstPlayerController()->GetControlRotation();
		}

		FRotator Rot = UKismetMathLibrary::NormalizedDeltaRotator(OriginRotation, PostRecoilRotation);

		float ClampValue = UKismetMathLibrary::MapRangeClamped(abs(Rot.Pitch), 0, RecoilMaxThresHold, 0, 1);

		PitchAmount = PitchAmount * -1 * ClampValue * RecoilRate;
		YawAmount = YawAmount * -1 * ClampValue * RecoilRate;

		AddControllerPitchAndYaw(PitchAmount, YawAmount);
	}
}

void AGunBase::GunRecoilFinished()
{
	if (TimelineDirection.GetValue() == ETimelineDirection::Backward)
	{
		ResetRecoil();
	}
}

void AGunBase::Init_OnCreate()
{
	int size = static_cast<int>(EAttachmentType::End);

	TArray<bool> OnSpawn_Attachments;
	OnSpawn_Attachments.SetNum(size);

	OnSpawn_Attachments[static_cast<int>(EAttachmentType::Optics)] = OnSpawn_Optic;
	OnSpawn_Attachments[static_cast<int>(EAttachmentType::LaserSight)] = OnSpawn_Laser;
	OnSpawn_Attachments[static_cast<int>(EAttachmentType::Supressor)] = OnSpawn_Supressor;

	for (int i = 0; i < size; i++)
	{
		bool flag = OnSpawn_Attachments[i];

		if (flag)
		{
			AddAttachment(EAttachmentType(i));
		}
	}
}

void AGunBase::HighLightOff()
{
	SkeletalMesh->SetOverlayMaterial(nullptr);
}

void AGunBase::HighLightOn()
{
	SkeletalMesh->SetOverlayMaterial(RenderingMat);
}

