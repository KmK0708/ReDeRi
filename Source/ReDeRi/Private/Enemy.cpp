// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemy.h"
#include <Components/SkeletalMeshComponent.h>
#include "EnemyFSM.h"
#include "EnemyAnim.h"
#include "Bullet.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Engine/SkeletalMesh.h"
// Sets default values
AEnemy::AEnemy()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// enemy FSM 컴포넌트 추가
	myEnemyFSM = CreateDefaultSubobject<UEnemyFSM>(TEXT("EnemyFSM"));
	// enemy mesh
		// constructorhelpers를 이용해서 캐릭터의 메쉬를 가져온다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempEnemyMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Characters/SK_Character_Badguy_01.SK_Character_Badguy_01'"));
	// 불러오는데 성공했다면
	if (TempEnemyMesh.Succeeded())
	{
		// 메쉬를 GetMesh로 적용한다.
		GetMesh()->SetSkeletalMesh(TempEnemyMesh.Object);
		// transform을 설정한다.
		GetMesh()->SetRelativeLocationAndRotation(FVector(0.0f, 0.0f, -90.0f), FRotator(0.0f, -90.0f, 0.0f));
		// scale 수정
		GetMesh()->SetRelativeScale3D(FVector(1.0f));
	}

	// 라이플의 컴포넌트를 만들고 싶다.
	GunMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("GunMeshComp"));
	GunMeshComp->SetRelativeScale3D(FVector(1.0f));
	GunMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	// 라이플의 에셋을 읽어서 컴포넌트에 적용한다.
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempGunMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Rifle_01.SK_Wep_Rifle_01'"));
	// 만약 적용이 성공했다면
	if (TempGunMesh.Succeeded())
	{
		// 라이플의 메쉬를 GunMeshComp에 적용한다.
		GunMeshComp->SetSkeletalMesh(TempGunMesh.Object);
		// transform을 설정한다.
		GunMeshComp->SetRelativeLocationAndRotation(FVector(-10.304565f, -4.293165f, -3.691982f), FRotator(20.551983f, -76.14f, 159.704f));

	}

	// 리볼버 컴포넌트
	RevolverMeshComp = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("RevolverMeshComp"));
	RevolverMeshComp->SetRelativeScale3D(FVector(1.0f));
	RevolverMeshComp->SetupAttachment(GetMesh(), TEXT("Hand_RSocket"));
	// 리볼버 에셋 읽어서 컴포넌트에 적용
	ConstructorHelpers::FObjectFinder<USkeletalMesh> TempRevolverMesh(TEXT("/Script/Engine.SkeletalMesh'/Game/PolygonWestern/Meshes/Weapons/SK_Wep_Revolver_01.SK_Wep_Revolver_01'"));
	if (TempRevolverMesh.Succeeded())
	{
		RevolverMeshComp->SetSkeletalMesh(TempRevolverMesh.Object);
		RevolverMeshComp->SetRelativeLocationAndRotation(FVector(-10.304565f, -4.293165f, -3.691982f), FRotator(20.551983f, -76.14f, 159.704f));
	}

	// 애너미에님 컨스트럭터헬퍼스
	ConstructorHelpers::FClassFinder<UAnimInstance> TempAnim(TEXT("/Script/Engine.AnimBlueprint'/Game/Blueprint/enemy/ABP_Enemy.ABP_Enemy_C'"));
	if (TempAnim.Succeeded())
	{
		GetMesh()->SetAnimInstanceClass(TempAnim.Class);
	}
	
	GetCharacterMovement()->bOrientRotationToMovement = true;

	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}


void AEnemy::OnDeath()
{
	GetMesh()->SetCollisionProfileName(TEXT("Ragdoll"));
	GetMesh()->SetSimulatePhysics(true);
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RevolverMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GunMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

// Called when the game starts or when spawned
void AEnemy::BeginPlay()
{
	Super::BeginPlay();

	// 애너미애님 겟매쉬 캐스트
	enemyAnim = Cast<UEnemyAnim>(GetMesh()->GetAnimInstance());
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

//void AEnemy::OnFire()
//{
//	enemyAnim->OnMyAttack(TEXT("GunShoot"));
//}
