// Fill out your copyright notice in the Description page of Project Settings.


#include "EnemyFSM.h"
#include "Enemy.h"
#include "RedPlayer.h"
#include <Kismet/GameplayStatics.h>
#include <Components/SkeletalMeshComponent.h>
#include "Bullet.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "AIController.h"
#include "NavigationSystem.h"
#include "EnemyAnim.h"

// Sets default values for this component's properties
UEnemyFSM::UEnemyFSM()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UEnemyFSM::BeginPlay()
{
	Super::BeginPlay();

	// 월드에서 ARedPlayer 타깃 찾기

	// ARedPlayer 타입으로 캐스팅
	// 소유 객체 가져오기
	me = Cast<AEnemy>(GetOwner());

	// 태어날때 현재 체력을 최대 체력으로 설정
	EnemyHealth = EnemyMaxHealth;

	// AAIController 타입으로 캐스팅
	AI = Cast<AAIController>(me->GetController());
}


// Called every frame
void UEnemyFSM::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	{
		target = UGameplayStatics::GetPlayerController(GetWorld(), 0)->GetPawn();

		switch (mState)
		{
		case EEnemyState::Idle:
			IdleState();
			break;
		case EEnemyState::Move:
			MoveState();
			break;
		case EEnemyState::Attack:
			AttackState();
			break;
		case EEnemyState::Damage:
			DamageState();
			break;
		case EEnemyState::Dead:
			DeadState();
			break;
		}
	}
}

// 대기 상태
void UEnemyFSM::IdleState()
{
	
	
	// 적의 플레이어 인지범위 생성
	FCollisionShape shape;
	shape.SetSphere(3000.0f);
	// 적의 위치
	FVector location = me->GetActorLocation();
	// 적의 위치에서 플레이어 인지범위 생성
	TArray<FOverlapResult> Overlaps;
	// 적의 위치에서 플레이어 인지범위 생성
	GetWorld()->OverlapMultiByChannel(Overlaps, location, FQuat::Identity, ECollisionChannel::ECC_Pawn, shape);
	// 플레이어가 인지범위에 있으면

		// 플레이어가 범위 내에 없으면
		if (Overlaps.Num() == 0)
		{
			// 상태를 대기 상태로 변경
			mState = EEnemyState::Idle;
			me->enemyAnim->State = mState;
			// 반복문 종료
			return;
		}
		auto player = UGameplayStatics::GetPlayerPawn(GetWorld(), 0);
		for (auto overlap : Overlaps)
		{
			// 플레이어 타입으로 캐스팅
			// 플레이어가 존재하면
			if (overlap.GetActor() == player)
			{
				target = player;
				// 플레이어를 타깃으로 설정
				// 상태를 이동 상태로 변경
				mState = EEnemyState::Move;
				me->enemyAnim->State = mState;
				//Cast<AEnemy>(GetOwner())->enemyAnim->State = mState
				// 반복문 종료
				break;
			}
		}
	
}

// 이동 상태
void UEnemyFSM::MoveState()
{
	// 1. 타깃 목적지가 필요함.
	FVector destination = target->GetActorLocation();
	// 2. 방향이 필요함.
	FVector direction = destination - me->GetActorLocation();
	// 3. 방향으로 EnemyRunSpeed속도로 이동하고 싶다.

	me->GetCharacterMovement()->MaxWalkSpeed = EnemyRunSpeed;

	//	me->AddMovementInput(direction.GetSafeNormal());

	AI->MoveToLocation(destination, 800.0f);

	// 타깃과 가까워 지면 공격 상태로 전환하고 싶다.
	// 1. 만약 거리가 공격 범위 안에 들어오면..
	if (direction.Size() < AttackRange)
	{
		// 2. 상태를 공격 상태로 변경
		mState = EEnemyState::Attack;
		me->enemyAnim->State = mState;
	}
}

// 공격 상태
void UEnemyFSM::AttackState()
{
	// 목표 : 일정시간 마다 한번씩 공격하고싶다.
	// 1. 시간이 흘렀으니까
	currentTime += GetWorld()->DeltaTimeSeconds;
	// 2. 공격시간이 경과했는지 확인
	if (currentTime > AttackDelayTime)
	{
		// Enemy.cpp에있는 OnFire 함수를 호출
		me->enemyAnim->OnMyAttack(TEXT("GunShoot"));
		// 3. 공격소리 재생
		// Enemy에있는 GunMeshComp를 이용해서 공격
		FTransform transform = me->GunMeshComp->GetSocketTransform("SK_Wep_Rifle_01_SlideSocket");
		GetWorld()->SpawnActor<ABullet>(EnemyRifleBulletFactory, transform);

		UGameplayStatics::PlaySoundAtLocation(GetWorld(), me->EnemyAttackSound, me->GetActorLocation(), me->GetActorRotation());
		// 4. 경과 시간 초기화
		currentTime = 0.0f;

		// 목표 : 타깃이 공격 범위를 벗어나면 상태를 이동으로 전환하고 싶다.
		// 1. 타깃과의 거리가 필요하다.
		float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());
		// 2. 만약 거리가 공격 범위보다 크면
		if (distance > AttackRange)
		{
			// 3. 상태를 이동 상태로 변경
			mState = EEnemyState::Move;
			me->enemyAnim->State = mState;
		}
		else
		{
			//범위 안에있으면 계속 공격
			me->enemyAnim->bAttackPlay = true;
		}
	}
	// 플레이어와의 거리 계산
	FVector direction = target->GetActorLocation() - me->GetActorLocation();
	float distance = FVector::Distance(target->GetActorLocation(), me->GetActorLocation());

	// 만약 플레이어가 공격 범위 안에 들어오면
	if (distance <= AttackRange)
	{
		// 적이 플레이어를 바라볼 때 필요한 로테이션 값
		FRotator lookAtRotation = FRotationMatrix::MakeFromX(direction).Rotator();
		// 적이 플레이어를 바라보는 로테이션
		me->SetActorRotation(FMath::Lerp(me->GetActorRotation(), lookAtRotation, 0.1f));
	}
}

// 피격 상태
void UEnemyFSM::DamageState()
{
	currentTime += GetWorld()->GetDeltaSeconds();
	// currentTime이 1초가 넘으면
	if (currentTime > 1.0f)
	{
		// 상태를 이동 상태로 변경
		mState = EEnemyState::Move;
		me->enemyAnim->State = mState; // 경과 시간 초기화
		currentTime = 0.0f;
	}
}

// 사망 상태
void UEnemyFSM::DeadState()
{
	currentTime += GetWorld()->GetDeltaSeconds();

	// 사망
	if (currentTime > 3.0f)
	{
		/*for (int i = 0; i < bpEnemySpawner.Num(); i++)
		{
			enemySpawns[i]->NumEnemy--;

			if (enemySpawns[i]->NumEnemy <= 0)
			{
				enemySpawns[i]->isSpawn = true;
			}
		}*/
		// currentTime이 1초가 넘으면 사망
		me->Destroy();
	}
}

void UEnemyFSM::OnDamageProcess(int32 damage)
{
	EnemyHealth -= damage;

	// 체력이 0이하면
	if (EnemyHealth <= 0)
	{
		// 상태를 사망 상태로 변경
		bIsDead = true;
		me->OnDeath();
		mState = EEnemyState::Dead;
		me->enemyAnim->State = mState;
	}
	else
	{
		// 상태를 피격 상태로 변경
		mState = EEnemyState::Damage;
		me->enemyAnim->State = mState;
	}
}