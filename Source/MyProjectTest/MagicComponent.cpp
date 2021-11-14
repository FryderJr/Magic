// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicComponent.h"
#include "NiagaraComponent.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

// Sets default values for this component's properties
UMagicComponent::UMagicComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UMagicComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UMagicComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

AActor* UMagicComponent::FindEnemyInFront()
{
	AActor* SelfActor = GetOwner();
	AActor* Enemy = nullptr;
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);

	FVector Forward = SelfActor->GetActorForwardVector();
	Forward.Normalize();

	float MinAngle = 45.0f;

	for (auto& Single : OutActors)
	{
		if (Single == SelfActor)
		{
			continue;
		}
		FVector ToEnemy = Single->GetActorLocation() - SelfActor->GetActorLocation();
		ToEnemy.Normalize();
		float Angle = FMath::Acos(FVector::DotProduct(Forward, ToEnemy)) * 57.2958;
		if (Angle < MinAngle) {
			Enemy = Single;
			MinAngle = Angle;
		}
	}
	return Enemy;
}

void UMagicComponent::ServerThrowFireball_Implementation()
{
	ThrowFireball();
}

void UMagicComponent::ThrowFireball()
{
	AActor* SelfActor = GetOwner();
	if (SelfActor->GetLocalRole() < ROLE_Authority)
	{
		ServerThrowFireball();
		return;
	}
	if (FireballClass == nullptr)
	{
		return;
	}
	ThrownFireball();
	AActor* Enemy = FindEnemyInFront();
	FTransform FireballTransform;
	USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(SelfActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!MeshComponent)
	{
		return;
	}
	FVector SpawnLoc = MeshComponent->GetSocketLocation(RCastSocketName);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	//SpawnLoc.X += 100.0;
	FireballTransform.SetLocation(SpawnLoc);
	FireballTransform.SetRotation(SelfActor->GetActorRotation().Quaternion());
	if (Enemy != nullptr)
	{
		FVector forward = Enemy->GetActorLocation() - SelfActor->GetActorLocation();
		FVector up = SelfActor->GetActorUpVector();
		FRotator rotateAtEnemy = UKismetMathLibrary::MakeRotFromXZ(forward.GetSafeNormal(), up.GetSafeNormal());
		FireballTransform.SetRotation(rotateAtEnemy.Quaternion());
	}

	AActor* Fireball = GetWorld()->SpawnActor<AActor>(FireballClass, FireballTransform, SpawnParameters);
}

void UMagicComponent::ServerThrowSeekingball_Implementation()
{
	ThrowSeekingball();
}

void UMagicComponent::ThrowSeekingball()
{
	AActor* SelfActor = GetOwner();
	if (SelfActor->GetLocalRole() < ROLE_Authority)
	{
		ServerThrowSeekingball();
		return;
	}
	if (SeekingballClass == nullptr)
	{
		return;
	}
	FTransform FireballTransform;
	USkeletalMeshComponent* MeshComponent = Cast<USkeletalMeshComponent>(SelfActor->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
	if (!MeshComponent)
	{
		return;
	}
	FVector SpawnLoc = MeshComponent->GetSocketLocation(RCastSocketName);
	SpawnLoc += SelfActor->GetActorForwardVector() * 50;
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FireballTransform.SetLocation(SpawnLoc);


	FireballTransform.SetRotation(SelfActor->GetActorForwardVector().Rotation().Quaternion());

	AActor* Fireball = GetWorld()->SpawnActor<AActor>(SeekingballClass, FireballTransform, SpawnParameters);
}

void UMagicComponent::ServerCreateShield1_Implementation()
{
	CreateShield1();
}

void UMagicComponent::CreateShield1()
{
	AActor* SelfActor = GetOwner();
	if (SelfActor->GetLocalRole() < ROLE_Authority)
	{
		ServerCreateShield1();
		return;
	}
	if (Shield1Class == nullptr)
	{
		return;
	}
	if (CreatedShield != nullptr)
	{
		CreatedShield->Destroy();
	}
	FTransform ShieldTransform;
	FVector SpawnLoc = SelfActor->GetActorLocation() + SelfActor->GetActorUpVector() * (-15.0);
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	ShieldTransform.SetLocation(SpawnLoc);
	ShieldTransform.SetRotation(SelfActor->GetActorRotation().Quaternion());
	CreatedShield = GetWorld()->SpawnActor<AActor>(Shield1Class, ShieldTransform, SpawnParameters);
	CreatedShield->OnDestroyed.AddDynamic(this, &UMagicComponent::OnShieldDestroyed);
}

void UMagicComponent::ServerCreateFlock_Implementation()
{
	CreateFlock();
}

void UMagicComponent::CreateFlock()
{
	AActor* SelfActor = GetOwner();
	if (SelfActor->GetLocalRole() < ROLE_Authority)
	{
		ServerCreateFlock();
		return;
	}
	if (FlockClass == nullptr)
	{
		return;
	}
	if (CreatedFlock != nullptr)
	{
		return;
	}
	AActor* Enemy = FindEnemyInFront();
	if (Enemy == nullptr)
	{
		return;
	}
	FTransform FlockTransform;
	FVector SpawnLoc = SelfActor->GetActorLocation();
	FActorSpawnParameters SpawnParameters;
	SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	FlockTransform.SetLocation(SpawnLoc);
	FlockTransform.SetRotation(SelfActor->GetActorRotation().Quaternion());
	CreatedFlock = GetWorld()->SpawnActor<AActor>(FlockClass, FlockTransform, SpawnParameters);
	CreatedFlock->OnDestroyed.AddDynamic(this, &UMagicComponent::OnFlockDestroyed);
}

TArray<AActor*> UMagicComponent::FindClosestActorsWithComponent(TSubclassOf<UActorComponent> ComponentClass, float Radius)
{
	AActor* SelfActor = GetOwner();
	TArray<AActor*> OutActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AActor::StaticClass(), OutActors);

	OutActors = OutActors.FilterByPredicate([&](const AActor* Single) {
		if (
			Single != SelfActor &&
			FVector::Distance(SelfActor->GetActorLocation(), Single->GetActorLocation()) < Radius &&
			Single->GetComponentByClass(ComponentClass) != nullptr
			)
		{
			print(FString::Printf(TEXT("Mesh name is %s"), *Single->GetName()));
			return true;
		}
		else
		{
			return false;
		}
		});
	return OutActors;
}

void UMagicComponent::ServerTransformObject_Implementation()
{
	TransformObject();
}

void UMagicComponent::TransformObject()
{
	AActor* SelfActor = GetOwner();
	if (SelfActor->GetLocalRole() < ROLE_Authority)
	{
		ServerTransformObject();
		return;
	}
	bool bIsSkeletal = false;
	TArray<AActor*> OutActors = FindClosestActorsWithComponent(USkeletalMeshComponent::StaticClass(), 500.0f);
	print(FString::Printf(TEXT("Skeletal meshes %d"), OutActors.Num()));
	OutActors.Append(FindClosestActorsWithComponent(UStaticMeshComponent::StaticClass(), 500.0f));
	print(FString::Printf(TEXT("Static meshes %d"), OutActors.Num()));
	OutActors.Sort([&](const AActor& First, const AActor& Second) {
		return (FVector::Distance(SelfActor->GetActorLocation(), First.GetActorLocation()) < FVector::Distance(SelfActor->GetActorLocation(), Second.GetActorLocation()));
		});
	if (OutActors.Num() > 0)
	{
		print(FString::Printf(TEXT("Actors found")));
		UMeshComponent* SM_Source = Cast<USkeletalMeshComponent>(OutActors[0]->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		bIsSkeletal = true;
		if (!SM_Source) {
			SM_Source = Cast<UStaticMeshComponent>(OutActors[0]->GetComponentByClass(UStaticMeshComponent::StaticClass()));
			bIsSkeletal = false;
		}
		if (!SM_Source) {
			return;
		}

		print(FString::Printf(TEXT("Mesh found")));
		FHitResult HitResult;
		FCollisionQueryParams CollisionQueryParams;
		CollisionQueryParams.AddIgnoredActor(OutActors[0]); //Igrone collision with base object
		CollisionQueryParams.AddIgnoredActor(SelfActor);	   //Ignore collision with player
		CollisionQueryParams.bTraceComplex = true;	   //Trace with each individual triangle of complex mesh
		CollisionQueryParams.bReturnPhysicalMaterial = true;   //Return physical material

		FVector SpawnLocation = SelfActor->GetActorLocation() + SelfActor->GetActorForwardVector() * 200.0f;
		SpawnLocation.Z += 200.0f;
		FVector End = SpawnLocation;
		End.Z -= 100000.0f;

		if (GetWorld()->LineTraceSingleByChannel(HitResult, SpawnLocation, End, ECC_Visibility, CollisionQueryParams))
		{
			SpawnLocation = HitResult.ImpactPoint;
		}

		FTransform DummyTransform;
		SM_Source->SetSimulatePhysics(false);
		DummyTransform.SetLocation(SpawnLocation);
		DummyTransform.SetRotation(SelfActor->GetActorRotation().Quaternion());
		FActorSpawnParameters SpawnParameters;
		SpawnParameters.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;

		AActor* Dummy = GetWorld()->SpawnActor<AActor>(DummyClass, DummyTransform, SpawnParameters);
		if (!Dummy)
		{
			return;
		}
		UNiagaraComponent* SpawnEffect;
		if (bIsSkeletal)
		{
			SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TransformationEffect, SM_Source->GetComponentLocation(), SM_Source->GetComponentRotation());
		}
		else
		{
			SpawnEffect = UNiagaraFunctionLibrary::SpawnSystemAtLocation(GetWorld(), TransformationEffectStatic, SM_Source->GetComponentLocation(), SM_Source->GetComponentRotation());
		}
		USkeletalMeshComponent* SM_Target = Cast<USkeletalMeshComponent>(Dummy->GetComponentByClass(USkeletalMeshComponent::StaticClass()));
		float lifetime = 5.0f;
		SpawnEffect->SetNiagaraVariableFloat("Lifetime", lifetime);
		UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(SpawnEffect, "User.SM_Target", SM_Target);
		if (bIsSkeletal)
		{
			UNiagaraFunctionLibrary::OverrideSystemUserVariableSkeletalMeshComponent(SpawnEffect, "User.SM_Source", Cast<USkeletalMeshComponent>(SM_Source));
		}
		else
		{
			UNiagaraFunctionLibrary::OverrideSystemUserVariableStaticMeshComponent(SpawnEffect, "User.SM_Source", Cast<UStaticMeshComponent>(SM_Source));
		}

		OutActors[0]->SetActorTickEnabled(false);
		OutActors[0]->SetActorHiddenInGame(true);
		OutActors[0]->SetActorEnableCollision(false);

		Dummy->SetActorTickEnabled(false);
		Dummy->SetActorHiddenInGame(true);
		Dummy->SetActorEnableCollision(false);

		FTimerDelegate CustomDelegate;
		OutActors.RemoveAt(1, OutActors.Num() - 1, true);
		CustomDelegate.BindUFunction(this, FName("TransformObjectEnd"), Dummy, OutActors);
		SelfActor->GetWorldTimerManager().SetTimer(FTimeBeforeAttack, CustomDelegate, lifetime, false);
	}
}

void UMagicComponent::TransformObjects()
{
}

void UMagicComponent::TransformObjectEnd(AActor* SM_Target, TArray<AActor*> SM_Sources)
{
	for (auto& Single : SM_Sources)
	{
		Single->Destroy();
	}
	SM_Target->SetActorHiddenInGame(false);
	SM_Target->SetActorTickEnabled(true);
	SM_Target->SetActorEnableCollision(true);
}

void UMagicComponent::OnFlockDestroyed(AActor* DestroyedActor)
{
	CreatedFlock = nullptr;
}

void UMagicComponent::OnShieldDestroyed(AActor* DestroyedActor)
{
	CreatedShield = nullptr;
}
