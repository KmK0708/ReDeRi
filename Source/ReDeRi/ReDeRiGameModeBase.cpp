// Copyright Epic Games, Inc. All Rights Reserved.


#include "ReDeRiGameModeBase.h"
#include "GamePlayWidget.h"
#include "DeadEyeSpawn.h"
#include "TargetCrossWidget.h"
#include "DefaultCrossWidget.h"
#include "BloodWidget.h"
#include "PistolBulletWidget.h"
#include "RifleBulletWidget.h"
#include "GameOverWidget.h"
#include "DeadEyeWidget.h"
#include "StartWidget.h"
#include "Horse.h"
#include "RedPlayer.h"
#include "Components/AudioComponent.h"
#include "Kismet/GameplayStatics.h"

AReDeRiGameModeBase::AReDeRiGameModeBase()
{
	PrimaryActorTick.bCanEverTick = true;
}

void AReDeRiGameModeBase::BeginPlay()
{
	Super::BeginPlay();

	CastFun();

	GameStart();

	HP = MaxHP;

	RP = MaxRP;
}

void AReDeRiGameModeBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnBlood();

	HPRPCharge();

	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ADeadEyeSpawn::StaticClass(), deadeyes);
}

void AReDeRiGameModeBase::CastFun()
{
	horse = Cast<AHorse>(UGameplayStatics::GetActorOfClass(GetWorld(), AHorse::StaticClass()));

	player = Cast<ARedPlayer>(UGameplayStatics::GetActorOfClass(GetWorld(), ARedPlayer::StaticClass()));

	play_UI = CreateWidget<UGamePlayWidget>(GetWorld(), gamePlayWidget);

	target_UI = CreateWidget<UTargetCrossWidget>(GetWorld(), targetCrossWidget);

	default_UI = CreateWidget<UDefaultCrossWidget>(GetWorld(), defaultCrossWidget);

	blood_UI = CreateWidget<UBloodWidget>(GetWorld(), bloodWidget);

	pBullet_UI = CreateWidget<UPistolBulletWidget>(GetWorld(), pistolBulletWidget);

	rBullet_UI = CreateWidget<URifleBulletWidget>(GetWorld(), rifleBulletWidget);

	gameover_UI = CreateWidget<UGameOverWidget>(GetWorld(), gameOverWidget);

	start_UI = CreateWidget<UStartWidget>(GetWorld(), startWidget);

	deadEye_UI = CreateWidget<UDeadEyeWidget>(GetWorld(), deadEyeWidget);
}

void AReDeRiGameModeBase::OnGamePlayWidget()
{
	if (play_UI != nullptr)
	{
		auto BGM = Cast<UAudioComponent>(UGameplayStatics::SpawnSound2D(GetWorld(), gameBGM));

		BGM->Activate();

		play_UI->AddToViewport();
	}
}

void AReDeRiGameModeBase::PlayerBulletSet()
{
	if (pBullet_UI != nullptr && player->armWeapon == EWeaponState::PISTOL)
	{
		pBullet_UI->AddToViewport();
	}
	else if (rBullet_UI != nullptr && player->armWeapon == EWeaponState::RIFLE)
	{
		rBullet_UI->AddToViewport();
	}
}

void AReDeRiGameModeBase::PlayerBulletOff()
{
	if (pBullet_UI != nullptr && player->armWeapon == EWeaponState::PISTOL)
	{
		pBullet_UI->RemoveFromParent();
	}
	else if (rBullet_UI != nullptr && player->armWeapon == EWeaponState::RIFLE)
	{
		rBullet_UI->RemoveFromParent();
	}
}

void AReDeRiGameModeBase::HorseBulletSet()
{
	if (pBullet_UI != nullptr && horse->weaponArm == EWeaponArm::PISTOL)
	{
		pBullet_UI->AddToViewport();
	}
	else if (rBullet_UI != nullptr && horse->weaponArm == EWeaponArm::RIFLE)
	{
		rBullet_UI->AddToViewport();
	}
}

void AReDeRiGameModeBase::HorseBulletOff()
{
	if (pBullet_UI != nullptr && horse->weaponArm == EWeaponArm::PISTOL)
	{
		pBullet_UI->RemoveFromParent();
	}
	else if (rBullet_UI != nullptr && horse->weaponArm == EWeaponArm::RIFLE)
	{
		rBullet_UI->RemoveFromParent();
	}
}

void AReDeRiGameModeBase::CrossHairOnOff()
{
	if (default_UI != nullptr)
	{
		default_UI->AddToViewport();
	}
}

void AReDeRiGameModeBase::CrossHairchange()
{
	if (default_UI != nullptr && target_UI != nullptr)
	{
		if (player->bTarget != false)
		{
			default_UI->RemoveFromParent();

			target_UI->AddToViewport();
		}
		else
		{
			default_UI->AddToViewport();

			target_UI->RemoveFromParent();
		}
	}
}

void AReDeRiGameModeBase::OnBlood()
{
	bool bInView = blood_UI->IsInViewport();
	if (blood_UI != nullptr)
	{
		if (HP < 50.0f && bInView != true)
		{
			blood_UI->AddToViewport();
		}
		else if (HP > 50.0f)
		{
			blood_UI->RemoveFromParent();
		}
	}
}

void AReDeRiGameModeBase::HPRPCharge()
{
	if (HP <= 100)
	{
		HP += 0.05;
	}
	if (RP <= 100)
	{
		RP += 0.05;
	}

	play_UI->playerHP = HP;

	play_UI->playerRP = RP;

	play_UI->dCount = deadCount;

	play_UI->bottleAmmo = holdBottleAmmo;

	pBullet_UI->pisAmmo = pistolAmmo;

	pBullet_UI->pisHoldAmmo = holdPistolAmmo;

	rBullet_UI->rifAmmo = rifleAmmo;

	rBullet_UI->rifHoldAmmo = holdRifleAmmo;
}

void AReDeRiGameModeBase::GameOver()
{
	if (gameover_UI != nullptr)
	{
		auto BGM = Cast<UAudioComponent>(UGameplayStatics::SpawnSound2D(GetWorld(), gameBGM));
		BGM->Stop();

		UGameplayStatics::GetPlayerController(GetWorld(), 0)->SetShowMouseCursor(true);
		GetWorld()->GetFirstPlayerController()->AController::SetIgnoreLookInput(true);
		if(gameover_UI->IsInViewport() != true)
		{
			gameover_UI->AddToViewport();
		}
	}
}

void AReDeRiGameModeBase::GameStart()
{
	if (start_UI != nullptr)
	{
		start_UI->AddToViewport();
	}

	UGameplayStatics::SetGamePaused(GetWorld(), true);

	GetWorld()->GetFirstPlayerController()->SetShowMouseCursor(true);
}
