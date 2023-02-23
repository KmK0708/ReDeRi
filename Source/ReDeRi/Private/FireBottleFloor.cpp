// Fill out your copyright notice in the Description page of Project Settings.


#include "FireBottleFloor.h"
#include "Enemy.h"
#include "EnemyFSM.h"
#include "RedPlayer.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFireBottleFloor::AFireBottleFloor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
}

// Called when the game starts or when spawned
void AFireBottleFloor::BeginPlay()
{
	Super::BeginPlay();

	StartFloor();

	StartFire();

	GetWorld()->GetTimerManager().SetTimer(destroyTime, this, &AFireBottleFloor::DestroySelf, 8.0f, false);
}

// Called every frame
void AFireBottleFloor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	OnOverlap();
}

void AFireBottleFloor::OnOverlap()
{
	TArray<FOverlapResult> hitsInfo;
	FVector centerLoc = GetActorLocation();
	FQuat centerRot = GetActorRotation().Quaternion();
	FCollisionObjectQueryParams params;
	params.AddObjectTypesToQuery(ECC_GameTraceChannel1);
	params.AddObjectTypesToQuery(ECC_GameTraceChannel2);
	params.AddObjectTypesToQuery(ECC_GameTraceChannel3);

	FCollisionShape checkShape = FCollisionShape::MakeSphere(200);
	GetWorld()->OverlapMultiByObjectType(hitsInfo, centerLoc, centerRot, params, checkShape);
	for (FOverlapResult hitInfo : hitsInfo)
	{
		auto enemy = Cast<AEnemy>(hitInfo.GetActor());
		if (enemy != nullptr)
		{
			UEnemyFSM* fsm = Cast<UEnemyFSM>(enemy->GetDefaultSubobjectByName(TEXT("EnemyFSM")));

			fsm->OnDamageProcess(1);
		}
		auto player = Cast<ARedPlayer>(hitInfo.GetActor());
		if (player != nullptr)
		{
			player->OnDamage(0.1f);
		}
	}
	DrawDebugSphere(GetWorld(), centerLoc, 200, 1, FColor::Yellow, false, 0.5);
}

void AFireBottleFloor::StartFloor()
{
	if (floorEffect != nullptr)
	{
		niagaraFloorComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), floorEffect, GetActorLocation());
	}
}

void AFireBottleFloor::StartFire()
{
	if (fireEffect != nullptr)
	{
		niagaraFireComp = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), fireEffect, GetActorLocation());
	}
}

void AFireBottleFloor::DestroySelf()
{
	this->Destroy();

	niagaraFloorComp->DestroyInstance();

	niagaraFireComp->DestroyInstance();
}