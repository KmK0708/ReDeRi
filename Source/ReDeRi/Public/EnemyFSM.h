// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EnemyFSM.generated.h"

// ����� ���� ����
UENUM(BlueprintType)
enum class EEnemyState : uint8
{
	Idle,
	Move,
	Attack,
	Damage,
	Dead
};

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class REDERI_API UEnemyFSM : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UEnemyFSM();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FSM")
		EEnemyState mState = EEnemyState::Idle;

	// ��� ����
	void IdleState();
	// �̵� ����
	void MoveState();
	// ���� ����
	void AttackState();
	// �ǰ� ����
	void DamageState();
	// ��� ����
	void DeadState();

	// ��� �ð�
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "FSM")
		float IdleDelayTime = 2.0f;
	// ��� �ð�
	float currentTime = 0.0f;
	bool bAttackPlay = false;
	// Ÿ��
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FSM")
		class APawn* target;

	// ���� ����
	UPROPERTY()
		class AEnemy* me;

	// ���� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float AttackRange = 300.0f;

	// ���� ������
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float AttackDelayTime = 1.5f;

	// �� �ȴ� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float EnemyWalkSpeed = 300.0f;

	// �� �ٴ� �ӵ�
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
		float EnemyRunSpeed = 600.0f;

	// �� ������ �Ҹ� ����
	UPROPERTY(EditAnywhere, Category = "FSM")
		TSubclassOf<class ABullet> EnemyRifleBulletFactory;

	// �� �ǽ��� �Ҹ� ���忡
	UPROPERTY(EditAnywhere, Category = "FSM")
		TSubclassOf<class ABullet> EnemyPistolBulletFactory;


	// �ǰ� �˸� �̺�Ʈ �Լ�
	void OnDamageProcess(int32 damage);

	// AI ��Ʈ�ѷ�
	UPROPERTY()
		class AAIController* AI;

	// ���� �׾��°�
	UPROPERTY(EditAnyWhere, BlueprintReadWrite, Category = "FSM")
		bool bIsDead = false;
	// spawntokill�� �� ����
	UPROPERTY(EditAnyWhere, BlueprintReadOnly, Category = "FSM")
		int NumEnemies;
	//// ���� ������, spawner�� numEnemy�� ������Ű�� �Լ�
	//UFUNCTION()
	//	void MinusNumEnemies();
	//UPROPERTY()
	//	TArray<class AEnemySpawnToKill*> enemySpawns;

	UPROPERTY()
		TArray<AActor*> bpEnemySpawner;

public:
	// ü���� ǥ�� �ϰ�ʹ�.
	int32 EnemyHealth;
	int32 EnemyMaxHealth = 50;
	// ����Ƚ��
	int32 EnemyAttackCount = 0;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
		bool IsCanPlayerShoot = false;
};
