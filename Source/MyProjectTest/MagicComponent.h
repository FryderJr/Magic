// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagicComponent.generated.h"


class USkeletalMeshComponent;
class UNiagaraComponent;
class USphereComponent;
class UNiagaraSystem;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class MYPROJECTTEST_API UMagicComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMagicComponent();

protected:

	FTimerHandle FTimeBeforeAttack;

	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(VisibleAnywhere, Category = Components)
		USphereComponent* MagicSphere;

	UPROPERTY(EditAnywhere, Category = Magic)
		FName RCastSocketName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		TSubclassOf<AActor> FireballClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		TSubclassOf<AActor> SeekingballClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		TSubclassOf<AActor> Shield1Class;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		TSubclassOf<AActor> FlockClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		UNiagaraSystem* TransformationEffect;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Magic)
		UNiagaraSystem* TransformationEffectStatic;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
		TSubclassOf<AActor> DummyClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Components)
		TSubclassOf<AActor> TrollClass;

	AActor* CreatedFlock;

	AActor* CreatedShield;

	AActor* FindEnemyInFront();

	UFUNCTION(Server, Reliable)
		void ServerThrowFireball();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void ThrowFireball();

	UFUNCTION(Server, Reliable)
		void ServerThrowSeekingball();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void ThrowSeekingball();

	UFUNCTION(BlueprintImplementableEvent, Category = Magic)
		void ThrownFireball();

	UFUNCTION(Server, Reliable)
		void ServerCreateShield1();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void CreateShield1();

	UFUNCTION(Server, Reliable)
		void ServerCreateFlock();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void CreateFlock();

	TArray<AActor*> FindClosestActorsWithComponent(TSubclassOf<UActorComponent> ComponentClass, float Radius);

	UFUNCTION(Server, Reliable)
		void ServerTransformObject();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void TransformObject();

	UFUNCTION(BlueprintCallable, Category = Magic)
		void TransformObjects();

	UFUNCTION()
		void TransformObjectEnd(AActor* SM_Target, TArray<AActor*> SM_Sources);

	UFUNCTION()
		void OnFlockDestroyed(AActor* DestroyedActor);

	UFUNCTION()
		void OnShieldDestroyed(AActor* DestroyedActor);
};
