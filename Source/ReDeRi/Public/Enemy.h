// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Enemy.generated.h"

UCLASS()
class REDERI_API AEnemy : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemy();

	// 적 메쉬
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "EnemySettings")
		class USkeletalMeshComponent* EnemyMesh;

	// 적 래그돌
	UFUNCTION(BlueprintCallable, Category = "EnemySettings")
		void OnDeath();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
public:
	// 적 AI 관리 컴포넌트 클래스
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "FMSComponent")
	class UEnemyFSM* myEnemyFSM;

	// 에너미 에니매이션
	UPROPERTY()
	class UEnemyAnim* enemyAnim;

	// 에너미 총발사 이벤트
	//UFUNCTION()
	//void OnFire();

	//공격사운드
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "FSM")
	USoundBase* EnemyAttackSound;

	// 총 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySettings")
	class USkeletalMeshComponent* GunMeshComp;

	// 리볼버 메쉬
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "EnemySettings")
	class USkeletalMeshComponent* RevolverMeshComp;
};
