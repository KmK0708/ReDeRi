// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraComponent.h"
#include "FireBottleFloor.generated.h"

UCLASS()
class REDERI_API AFireBottleFloor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFireBottleFloor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* floorEffect;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* fireEffect;

	UPROPERTY(EditAnywhere)
	class USphereComponent* sphereComp;

	UFUNCTION()
	void OnOverlap();

	void StartFloor();

	void StartFire();

	void DestroySelf();
	
	UPROPERTY()
	UNiagaraComponent* niagaraFloorComp;

	UPROPERTY()
	UNiagaraComponent* niagaraFireComp;

	FTimerHandle destroyTime;
};
