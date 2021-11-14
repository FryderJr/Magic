// Fill out your copyright notice in the Description page of Project Settings.


#include "Missile.h"
#include "HealthComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Components/SphereComponent.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values
AMissile::AMissile()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SphereComponent = CreateDefaultSubobject<USphereComponent>(FName("SphereComp"));
	SphereComponent->SetSphereRadius(200);
	SphereComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComponent->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	RootComponent = SphereComponent;

	HealthComponent = CreateDefaultSubobject<UHealthComponent>(FName("HealthComponent"));
	HealthComponent->OnHealthChanged.AddDynamic(this, &AMissile::OnHealthChanged);

	MovementComponent = CreateDefaultSubobject<UProjectileMovementComponent>(FName("MovementComponent"));
	MovementComponent->UpdatedComponent = SphereComponent;
	MovementComponent->InitialSpeed = 200.0f;
	MovementComponent->MaxSpeed = 300.0f;
	MovementComponent->bRotationFollowsVelocity = true;
	MovementComponent->bInitialVelocityInLocalSpace = true;
	MovementComponent->bShouldBounce = false;
	MovementComponent->bIsHomingProjectile = false;
	MovementComponent->ProjectileGravityScale = 0.f;
	MovementComponent->Velocity = FVector(0.f, 0.f, 0.f);

	TimeBeforHoming = 0.75;
}

void AMissile::OnHealthChanged(UHealthComponent* HealthComp, float Health, float HealthDelta, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{

}

void AMissile::HandleSphereOverlap(AActor* OverlappedActor, AActor* OtherActor)
{

}

AActor* AMissile::FindEnemyInFront()
{
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APawn::StaticClass(), OutActors);

	OutActors = OutActors.FilterByPredicate([&](AActor* Single) {
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		//CollisionQueryParams.AddIgnoredActor(Single); //Igrone collision with base object
		CollisionQueryParams.AddIgnoredActor(this);	   //Ignore collision with projectile
		CollisionQueryParams.bTraceComplex = true;	   //Trace with each individual triangle of complex mesh
		CollisionQueryParams.bReturnPhysicalMaterial = true;   //Return physical material

		FVector ToEnemy = Single->GetActorLocation() - GetActorLocation();
		ToEnemy.Normalize();
		float Angle = FMath::Acos(FVector::DotProduct(GetActorForwardVector(), ToEnemy)) * 57.2958;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, Single->GetActorLocation(), GetActorLocation(), ECC_Visibility, CollisionQueryParams))
		{
			return false;
		}

		if (Angle < 45.0 && !HealthComponent->IsFriendly(this, Single))
		{
			return true;
		}
		else
		{
			return false;
		}
		});
	if (OutActors.Num() > 0)
	{
		OutActors.Sort([&](const AActor& First, const AActor& Second) {
			return (FVector::Distance(GetActorLocation(), First.GetActorLocation()) < FVector::Distance(GetActorLocation(), Second.GetActorLocation()));
			});
		return OutActors[0];
	}
	else
	{
		return nullptr;
	}
}

void AMissile::SetHoming()
{
	FTransform CreateTransform;
	UActorComponent* TempComponent = Target->AddComponentByClass(UStaticMeshComponent::StaticClass(), false, CreateTransform, false);
	MovementComponent->HomingTargetComponent = Cast<USceneComponent>(TempComponent);
	MovementComponent->bIsHomingProjectile = true;
}

void AMissile::SelfDestruct()
{
	TArray<AActor*> IgnoredActors;
	IgnoredActors.Add(this);
	UGameplayStatics::ApplyRadialDamage(this, 100.0f, GetActorLocation(), 200.0f, nullptr, IgnoredActors, this, GetInstigatorController(), true);
	SetLifeSpan(0.2f);
}

// Called when the game starts or when spawned
void AMissile::BeginPlay()
{
	Super::BeginPlay();
	
	OnActorBeginOverlap.AddDynamic(this, &AMissile::HandleSphereOverlap);

	Target = FindEnemyInFront();
	if (Target == nullptr)
	{
		MovementComponent->bIsHomingProjectile = false;
	}
	else
	{
		FTimerDelegate CustomDelegate;
		CustomDelegate.BindUFunction(this, FName("SetHoming"));
		GetWorldTimerManager().SetTimer(FTimeBeforeHoming, CustomDelegate, TimeBeforHoming, false);
	}
}

// Called every frame
void AMissile::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (!MovementComponent->bIsHomingProjectile)
	{
		MovementComponent->Velocity += GetActorForwardVector() * MovementComponent->InitialSpeed * DeltaTime;
	}
}

