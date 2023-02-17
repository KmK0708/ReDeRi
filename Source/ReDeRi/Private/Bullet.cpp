// Fill out your copyright notice in the Description page of Project Settings.


#include "Bullet.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "RedPlayer.h"
#include "Kismet/GameplayStatics.h"


// Sets default values
ABullet::ABullet()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	sphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("sphereComp"));
	SetRootComponent(sphereComp);
	sphereComp->SetSphereRadius(5);
	sphereComp->SetCollisionProfileName(TEXT("OverlapAllDynamic"));

	meshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("meshComp"));
	meshComp->SetupAttachment(sphereComp);
	meshComp->SetRelativeScale3D(FVector(0.5f));
	meshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	ConstructorHelpers::FObjectFinder<UStaticMesh> tempMesh(TEXT("/Script/Engine.StaticMesh'/Game/Asset/prop/Bullet.Bullet'"));
	if (tempMesh.Succeeded())
	{
		meshComp->SetStaticMesh(tempMesh.Object);

		meshComp->SetRelativeRotation(FRotator(0, -90.0f, 0));
	}

	movementComp = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("movementComp"));
	movementComp->SetUpdatedComponent(sphereComp);
	movementComp->InitialSpeed = 2000.0f;
	movementComp->MaxSpeed = 2000.0f;
	movementComp->ProjectileGravityScale = 0.2f;
}

// Called when the game starts or when spawned
void ABullet::BeginPlay()
{
	Super::BeginPlay();

}

// Called every frame
void ABullet::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	FVector NormalImpulse, const FHitResult& Hit)
{
	// �Ѿ��� �÷��̾�� �¾��� ��
	if (OtherActor->IsA(ARedPlayer::StaticClass()))
	{
		// �÷��̾� Ÿ������ ĳ����
		ARedPlayer* player = Cast<ARedPlayer>(OtherActor);

		// ���� ������ player��� OnDamage�Լ��� �����ϰ� �������� �Ѱ��ش�.
		if (player)
		{
			player->OnDamage(Damage);
			UE_LOG(LogTemp, Warning, TEXT("Hittest"));
		}
		// �Ѿ� �ı�
		Destroy();
	}
	else
	{
		// �Ѿ� ���� ���
		UGameplayStatics::PlaySoundAtLocation(this, BulletWhipSound, GetActorLocation());
	}
}

