// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Bullet.generated.h"

UCLASS()
class REDERI_API ABullet : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABullet();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class USphereComponent* sphereComp;

	UPROPERTY(EditAnywhere)
	class UStaticMeshComponent* meshComp;

	UPROPERTY(EditAnywhere)
	class UProjectileMovementComponent* movementComp;

	// �Ѿ� ������
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "BulletSettings")
	float Damage = 15.0f;

	// �Ѿ� ������ �Լ�
	UFUNCTION()
	void OnHit(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	void DestroyMyself();

	// �Ѿ� ��ġ�� ����
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "BulletSettings")
	USoundBase* BulletWhipSound;
};