// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Missile.generated.h"

class UHealthComponent;
class USphereComponent;
class UProjectileMovementComponent;

UCLASS()
class MYPROJECTTEST_API AMissile : public AActor
{
	GENERATED_BODY()
	
private:

	AActor* Target;

public:
	// Sets default values for this actor's properties
	AMissile();

	FTimerHandle FTimeBeforeHoming;

	UPROPERTY(EditDefaultsOnly, Category = Components, meta = (ClampMin = 0.0, ClampMax = 5))
	float TimeBeforHoming;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Components)
	UHealthComponent* HealthComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	USphereComponent* SphereComponent;

	UPROPERTY(VisibleDefaultsOnly, Category = Components)
	UProjectileMovementComponent* MovementComponent;

	UFUNCTION()
	void OnHealthChanged(UHealthComponent* HealthComp, float Health, float HealthDelta, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);

	UFUNCTION()
	void HandleSphereOverlap(AActor* OverlappedActor, AActor* OtherActor);

	AActor* FindEnemyInFront();

	UFUNCTION()
	void SetHoming();

	UFUNCTION(BlueprintCallable, Category = Missile)
	void SelfDestruct();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
