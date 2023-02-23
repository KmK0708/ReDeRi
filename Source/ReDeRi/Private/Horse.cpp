// Fill out your copyright notice in the Description page of Project Settings.


#include "Horse.h"
#include "DeadEyeSpawn.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "FireBottle.h"
#include "HorseAnim.h"
#include "HorsePlayerAnim.h"
#include "WeaponWidget.h"
#include "RedPlayer.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ReDeRi/ReDeRiGameModeBase.h"

// Sets default values
AHorse::AHorse()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/HorseAnimsetPro/Meshes/Horses/SK_Horse_PA.SK_Horse_PA'"));
	if (tempMesh.Succeeded())
	{
		GetMesh()->SetSkeletalMesh(tempMesh.Object);

		GetMesh()->SetRelativeScale3D(FVector(1.1f));

		GetMesh()->SetRelativeLocationAndRotation(FVector(-45.0f, 0, -90.0f), FRotator(0, -90.0f, 0));
	}
	//GetMesh()->SetAnimationMode(EAnimationMode::AnimationSingleNode);

	boxComp = CreateDefaultSubobject<UBoxComponent>(TEXT("boxComp"));
	boxComp->SetupAttachment(RootComponent);
	boxComp->SetBoxExtent(FVector(100.0f, 110.0f, 32.0f));

	springComp = CreateDefaultSubobject<USpringArmComponent>(TEXT("springComp"));
	springComp->SetupAttachment(RootComponent);
	springComp->SetRelativeLocationAndRotation(FVector(0, 0, 250.0f), FRotator(-30.0f, 0, 0));
	springComp->TargetArmLength = 500.0f;

	cameraComp = CreateDefaultSubobject<UCameraComponent>(TEXT("cameraComp"));
	cameraComp->SetupAttachment(springComp);

	bUseControllerRotationYaw = false;
	springComp->bUsePawnControlRotation = true;
	cameraComp->bUsePawnControlRotation = true;
	GetCharacterMovement()->bOrientRotationToMovement = true;

	attachComp = CreateDefaultSubobject<USceneComponent>(TEXT("attachComp"));
	attachComp->SetupAttachment(GetMesh());
	detachComp = CreateDefaultSubobject<USceneComponent>(TEXT("detachComp"));
	detachComp->SetupAttachment(GetMesh());
	detachComp->SetRelativeLocation(FVector(-80.0f, 45.0f, 40.0f));

	playerMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("playerMesh"));
	playerMesh->SetupAttachment(attachComp);
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempPlayerMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Characters/SK_Character_Cowboy_01.SK_Character_Cowboy_01'"));
	if (tempPlayerMesh.Succeeded())
	{
		playerMesh->SetSkeletalMesh(tempPlayerMesh.Object);

		playerMesh->SetRelativeLocationAndRotation(FVector(0, 50.0f, 160.0f), FRotator(0, 0, 0));
	}

	gunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("gunMeshComp"));
	gunMeshComp->SetupAttachment(playerMesh, TEXT("Hand_RSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Rifle_01.SK_Wep_Rifle_01'"));
	if (tempGunMesh.Succeeded())
	{
		gunMeshComp->SetSkeletalMesh(tempGunMesh.Object);

		gunMeshComp->SetRelativeLocationAndRotation(FVector(-11.0f, -2.6f, -8.0f), FRotator(-6.2f, -75.0f, 165.0f));
	}
	revolMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("revolMeshComp"));
	revolMeshComp->SetupAttachment(playerMesh, TEXT("Hand_RSocket"));
	ConstructorHelpers::FObjectFinder<USkeletalMesh> tempRevolMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Revolver_01.SK_Wep_Revolver_01'"));
	if (tempRevolMesh.Succeeded())
	{
		revolMeshComp->SetSkeletalMesh(tempRevolMesh.Object);

		revolMeshComp->SetRelativeLocationAndRotation(FVector(-12.4f, -4.3f, -6.2f), FRotator(5.0f, -80.0f, 165.0f));
	}
	bottleMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bottleMeshComp"));
	bottleMeshComp->SetupAttachment(playerMesh, TEXT("Hand_RSocket"));
	bottleFireMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("bottleFireMeshComp"));
	bottleFireMeshComp->SetupAttachment(bottleMeshComp);

	ConstructorHelpers::FClassFinder<UAnimInstance> tempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/player/ABP_Horse.ABP_Horse_C'"));
	if (tempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(tempAnim.Class);
	}

	ConstructorHelpers::FClassFinder<UAnimInstance> tempRiderAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/player/ABP_HorsePlayer.ABP_HorsePlayer_C'"));
	if (tempRiderAnim.Succeeded())
	{
		playerMesh->SetAnimInstanceClass(tempRiderAnim.Class);
	}
}

// Called when the game starts or when spawned
void AHorse::BeginPlay()
{
	Super::BeginPlay();

	boxComp->OnComponentBeginOverlap.AddDynamic(this, &AHorse::OverlapRide);
	boxComp->OnComponentEndOverlap.AddDynamic(this, &AHorse::EndRide);
	boxComp->SetGenerateOverlapEvents(true);

	playerMesh->SetVisibility(false);

	player = Cast<ARedPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ARedPlayer::StaticClass()));

	weapon_UI = CreateWidget<UWeaponWidget>(GetWorld(), weaponWidget);

	horseAnim = Cast<UHorseAnim>(GetMesh()->GetAnimInstance());

	horsePlayerAnim = Cast<UHorsePlayerAnim>(playerMesh->GetAnimInstance());

	gmH = Cast<AReDeRiGameModeBase>(GetWorld()->GetAuthGameMode());

	ChooseWeapon(EWeaponArm::FIST);

	GetCharacterMovement()->MaxWalkSpeed = 1200 * accel;
}

// Called every frame
void AHorse::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	FRotator cam = cameraComp->GetComponentRotation();
	if (cam.Yaw < 145.0f && cam.Yaw > -120.0f)
	{
		horsePlayerAnim->horseYaw = cam.Yaw;
		horsePlayerAnim->horsePitch = cam.Pitch;
	}

	accel = FMath::Clamp(accel, 0, maxAccel);
	if (h == 0 && v == 0)
	{
		// 손놓으면 감속
		accel = FMath::Lerp(accel, 0, breakValue);

		FTransform transform(GetControlRotation());
		FVector resultDirection = transform.TransformVector(breakDirection);
		resultDirection.Z = 0;
		resultDirection.Normalize();
		AddMovementInput(resultDirection);
	}
	else
	{
		breakDirection = direction;

		FTransform transform(GetControlRotation());
		FVector resultDirection = transform.TransformVector(direction);
		resultDirection.Z = 0;
		resultDirection.Normalize();
		AddMovementInput(resultDirection);
		direction = FVector::ZeroVector;
	}

	GetCharacterMovement()->MaxWalkSpeed = 1200 * accel;
}

// Called to bind functionality to input
void AHorse::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis(TEXT("Horse Horizontal"), this, &AHorse::Horizontal);
	PlayerInputComponent->BindAxis(TEXT("Horse Vertical"), this, &AHorse::Vertical);
	PlayerInputComponent->BindAxis(TEXT("Horse Look Up"), this, &AHorse::LookUp);
	PlayerInputComponent->BindAxis(TEXT("Horse Turn Right"), this, &AHorse::TurnRight);
	PlayerInputComponent->BindAction(TEXT("HorseRide"), IE_Pressed, this, &AHorse::HorseRide);
	PlayerInputComponent->BindAction(TEXT("FireBullet"), IE_Pressed, this, &AHorse::FirePressed);
	PlayerInputComponent->BindAction(TEXT("HorseWeaponChange"), IE_Pressed, this, &AHorse::WeaponChangePressed);
	PlayerInputComponent->BindAction(TEXT("Jump"), IE_Pressed, this, &AHorse::ActionJump);
	PlayerInputComponent->BindAction(TEXT("Reload"), IE_Pressed, this, &AHorse::Reload);
	PlayerInputComponent->BindAction(TEXT("DeadEye"), IE_Pressed, this, &AHorse::OnHorseDeadEye);
	PlayerInputComponent->BindAction(TEXT("DeadEye"), IE_Released, this, &AHorse::OffHorseDeadEye);
}

void AHorse::OverlapRide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	auto owner = Cast<ARedPlayer>(OtherActor);

	if (owner != nullptr)
	{
		player->bPressed = true;
	}
}

void AHorse::EndRide(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	auto owner = Cast<ARedPlayer>(OtherActor);

	if (owner != nullptr)
	{
		player->bPressed = false;
	}
}

void AHorse::Horizontal(float value)
{
	h = value;
	accel += FMath::Abs(value) * accelRate * GetWorld()->GetDeltaSeconds();
	if (value != 0)
	{
		direction.Y = value;
	}
}

void AHorse::Vertical(float value)
{
	v = value;
	accel += FMath::Abs(value) * accelRate * GetWorld()->GetDeltaSeconds();
	if (value != 0)
	{
		direction.X = value;
	}
}

void AHorse::LookUp(float value)
{
	AddControllerPitchInput(value);
}

void AHorse::TurnRight(float value)
{
	AddControllerYawInput(value);
}

void AHorse::ActionJump()
{
	Jump();
}

void AHorse::HorseRide()
{
	if (weaponArm == EWeaponArm::FIREBOTTLE)
	{
		player->ChooseWeapon(EWeaponState::FIREBOTTLE);
	}
	else if (weaponArm == EWeaponArm::FIST)
	{
		player->ChooseWeapon(EWeaponState::FIST);
	}
	else if (weaponArm == EWeaponArm::PISTOL)
	{
		player->ChooseWeapon(EWeaponState::PISTOL);
	}
	else if (weaponArm == EWeaponArm::RIFLE)
	{
		player->ChooseWeapon(EWeaponState::RIFLE);
	}

	ChooseWeapon(EWeaponArm::FIST);
	player->SetActorLocation(FVector(GetActorLocation().X, GetActorLocation().Y, GetActorLocation().Z + 50));
	player->SetActorRotation(GetActorRotation());
	playerMesh->SetVisibility(false);
	player->GetMesh()->SetVisibility(true);
	player->HorseUnRide();
}

void AHorse::ChangeMesh(bool bChange)
{
	if (bChange != true)
	{
		playerMesh->SetVisibility(true);
	}
}

void AHorse::UnRide()
{
	player->SetActorTransform(detachComp->GetComponentTransform());

	UGameplayStatics::GetPlayerController(this, 0)->Possess(player);

	player->bIsRide = false;
	//플레이어 콜리젼 켜기
	player->SetActorEnableCollision(true);

	GetMovementComponent()->StopMovementImmediately();

	GetCharacterMovement()->MaxWalkSpeed = 0;
}

void AHorse::Reload()
{
	if (horsePlayerAnim != nullptr)
	{
		horsePlayerAnim->ReloadAnim();
	}
	if (weaponArm == EWeaponArm::RIFLE)
	{
		player->PlaySound(player->rifleReloadSound, GetActorLocation());
	}
	else if (weaponArm == EWeaponArm::PISTOL)
	{
		player->PlaySound(player->pistolReloadSound, GetActorLocation());
	}
}

void AHorse::OnHorseDeadEye()
{
	if (weaponArm == EWeaponArm::PISTOL || weaponArm == EWeaponArm::RIFLE)
	{
		if (gmH->deadCount > 0)
		{
			bHorseDeadEye = true;

			UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);

			gmH->deadCount -= 1;
		}
	}
}

void AHorse::OffHorseDeadEye()
{
	HDestroyEnemy();

	bHorseDeadEye = false;

	UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
}

void AHorse::HDestroyEnemy()
{
	for (int i = 0; i < player->enemies.Num(); i++)
	{
		gmH->deadeyes[i]->Destroy();

		UEnemyFSM* fsm = Cast<UEnemyFSM>(player->enemies[i]->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

		fsm->OnDamageProcess(100);

		FVector locf = GetActorLocation();

		if (weaponArm == EWeaponArm::PISTOL)
		{
			player->PlaySound(pistolFireSound, locf);
		}
		else if (weaponArm == EWeaponArm::RIFLE)
		{
			player->PlaySound(gunFireSound, locf);
		}
	}

	player->enemies.Empty();

	gmH->deadeyes.Empty();
}

void AHorse::HorseDeadEyeTarget()
{
	FHitResult deadInfo;
	FVector start = cameraComp->GetComponentLocation();
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(deadInfo, start, end, objParams);
	if (bEnemyHit)
	{
		if (weaponArm == EWeaponArm::PISTOL)
		{
			gmH->pistolAmmo -= 1;
		}
		else if (weaponArm == EWeaponArm::RIFLE)
		{
			gmH->rifleAmmo -= 1;
		}
		auto enemy = Cast<AEnemy>(deadInfo.GetActor());
		player->enemies.Add(enemy);
		FVector loc = (deadInfo.GetActor()->GetActorLocation());
		FVector finLoc = loc + FVector(0, 0, 70.0f);
		GetWorld()->SpawnActor<ADeadEyeSpawn>(player->deadFactory, finLoc, FRotator::ZeroRotator);
	}
}

void AHorse::FirePressed()
{
	if (bHorseDeadEye != true)
	{
		FVector loch = GetActorLocation();
		switch (weaponArm)
		{
		case EWeaponState::FIST:
			break;

		case EWeaponState::PISTOL:
			if (gmH->pistolAmmo > 0)
			{
				if (horsePlayerAnim != nullptr)
				{
					horsePlayerAnim->FireAnim();
				}
				player->PlaySound(pistolFireSound, loch);
				FirePistol();
				gmH->pistolAmmo -= 1;
			}
			else
			{
				player->PlaySound(player->noBulletSound, loch);
			}
			break;

		case EWeaponState::RIFLE:
			if (gmH->rifleAmmo > 0)
			{
				if (horsePlayerAnim != nullptr)
				{
					horsePlayerAnim->FireAnim();
				}
				player->PlaySound(gunFireSound, loch);
				FireRifle();
				gmH->rifleAmmo -= 1;
			}
			else
			{
				player->PlaySound(player->noBulletSound, loch);
			}
			break;

		case EWeaponState::FIREBOTTLE:
			FireBottle();
			break;

		default:
			break;
		}
	}
	else if (bHorseDeadEye != false)
	{
		switch (weaponArm)
		{
		case EWeaponState::PISTOL:
			if (gmH->pistolAmmo >= 1)
			{
				HorseDeadEyeTarget();
			}
			break;

		case EWeaponState::RIFLE:
			if (gmH->rifleAmmo >= 1)
			{
				HorseDeadEyeTarget();
			}
			break;

		default:
			break;
		}
	}
}

void AHorse::WeaponChangePressed()
{
	if (weapon_UI && false == weapon_UI->IsInViewport() && player->bGetWeapon != false)
	{
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 0.1);
		weapon_UI->AddToViewport();
		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->AController::SetIgnoreLookInput(true);
		UWidgetBlueprintLibrary::SetInputMode_GameAndUIEx(GetWorld()->GetFirstPlayerController(), weapon_UI);
	}
}

void AHorse::ControllerWidget()
{
	UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(false);
	GetWorld()->GetFirstPlayerController()->AController::SetIgnoreLookInput(false);
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(GetWorld()->GetFirstPlayerController());
}

void AHorse::FirePistol()
{
	FHitResult hitInfo;
	FVector start = cameraComp->GetComponentLocation() + cameraLoc;
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	FCollisionObjectQueryParams objParams1;
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);
	objParams1.AddObjectTypesToQuery(ECC_GameTraceChannel4);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams);
	if (bEnemyHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		player->SpawnEmitter(pistolEnemyImpactFactory, trans);
		player->PlaySound(enemyImpactSound, loc);

		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

			fsm->OnDamageProcess(10);
		}
	}

	bool bGround = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams1);
	if (bGround)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		player->SpawnEmitter(GroundImpactFactory, trans);
		player->PlaySound(groundImpactSound, loc);
	}
}

void AHorse::FireRifle()
{
	FHitResult hitInfo;
	FVector start = cameraComp->GetComponentLocation() + cameraLoc;
	FVector end = start + cameraComp->GetForwardVector() * 200000.0f;
	FCollisionObjectQueryParams objParams;
	FCollisionObjectQueryParams objParams1;
	//채널3 - enemy
	objParams.AddObjectTypesToQuery(ECC_GameTraceChannel3);
	//채널4 - ground
	objParams1.AddObjectTypesToQuery(ECC_GameTraceChannel4);

	bool bEnemyHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams);
	if (bEnemyHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		player->SpawnEmitter(gunEnemyImpactFactory, trans);
		player->PlaySound(enemyImpactSound, loc);

		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

			fsm->OnDamageProcess(25);
		}
	}

	bool bGroundHit = GetWorld()->LineTraceSingleByObjectType(hitInfo, start, end, objParams1);
	if (bGroundHit)
	{
		FTransform trans(hitInfo.ImpactPoint);
		FVector loc(hitInfo.ImpactPoint);
		player->SpawnEmitter(GroundImpactFactory, trans);
		player->PlaySound(groundImpactSound, loc);
	}
}

void AHorse::FireBottle()
{
	if (horsePlayerAnim != nullptr && gmH->holdBottleAmmo > 0)
	{
		horsePlayerAnim->OnRiderAnim();
		gmH->holdBottleAmmo -= 1;
	}
}

void AHorse::SpawnHorseBottle()
{
	GetWorld()->SpawnActor<AFireBottle>(fireBottleFactory, GetActorLocation() + (GetActorUpVector() * 100.0f) + (GetActorForwardVector() * 100.0f), GetControlRotation());
}

void AHorse::ChangeFist()
{
	if (weapon_UI && true == weapon_UI->IsInViewport())
	{
		if (weaponArm == EWeaponArm::RIFLE || weaponArm == EWeaponArm::PISTOL)
		{
			gmH->HorseBulletOff();
		}
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponArm::FIST);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
}

void AHorse::ChangeRifle()
{
	if (weapon_UI && true == weapon_UI->IsInViewport())
	{
		if (weaponArm == EWeaponArm::RIFLE || weaponArm == EWeaponArm::PISTOL)
		{
			gmH->HorseBulletOff();
		}
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponArm::RIFLE);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		gmH->HorseBulletSet();
	}
}

void AHorse::ChangePistol()
{
	if (weapon_UI && true == weapon_UI->IsInViewport())
	{
		if (weaponArm == EWeaponArm::RIFLE || weaponArm == EWeaponArm::PISTOL)
		{
			gmH->HorseBulletOff();
		}
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponArm::PISTOL);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
		gmH->HorseBulletSet();
	}
}

void AHorse::ChangeBottle()
{
	if (weapon_UI && true == weapon_UI->IsInViewport())
	{
		if (weaponArm == EWeaponArm::RIFLE || weaponArm == EWeaponArm::PISTOL)
		{
			gmH->HorseBulletOff();
		}
		weapon_UI->RemoveFromParent();
		ControllerWidget();
		ChooseWeapon(EWeaponArm::FIREBOTTLE);
		UGameplayStatics::SetGlobalTimeDilation(GetWorld(), 1);
	}
}

void AHorse::ChooseWeapon(EWeaponArm val)
{
	switch (val)
	{
	case EWeaponArm::FIST:
		gunMeshComp->SetVisibility(false);
		revolMeshComp->SetVisibility(false);
		bottleMeshComp->SetVisibility(false);
		bottleFireMeshComp->SetVisibility(false);
		weaponArm = val;

		break;

	case EWeaponArm::PISTOL:
		gunMeshComp->SetVisibility(false);
		revolMeshComp->SetVisibility(true);
		bottleMeshComp->SetVisibility(false);
		bottleFireMeshComp->SetVisibility(false);
		weaponArm = val;

		break;

	case EWeaponArm::RIFLE:
		gunMeshComp->SetVisibility(true);
		revolMeshComp->SetVisibility(false);
		bottleMeshComp->SetVisibility(false);
		bottleFireMeshComp->SetVisibility(false);
		weaponArm = val;

		break;

	case EWeaponArm::FIREBOTTLE:
		gunMeshComp->SetVisibility(false);
		revolMeshComp->SetVisibility(false);
		bottleMeshComp->SetVisibility(true);
		bottleFireMeshComp->SetVisibility(true);
		weaponArm = val;

	default:
		break;
	}
}

