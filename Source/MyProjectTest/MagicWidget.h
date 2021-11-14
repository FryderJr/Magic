// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "MagicWidget.generated.h"

USTRUCT(BlueprintType)
struct FSpellPoints
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere, Category = MagicControl)
		TArray<int32> Points;
};
/**
 * Widgets that implements UI for magic component
 */
UCLASS()
class MYPROJECTTEST_API UMagicWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UMagicWidget(const FObjectInitializer& ObjectInitializer);

	virtual void NativeConstruct() override;

	virtual void SynchronizeProperties() override;

	FVector2D GetImagePosition(int32 Index);

	/*
	* Check if there is no recurring lines

	*/
	bool CheckPoints(int32 Index);

	FString CheckForSpell();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UBorder* MagicBorder;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UCanvasPanel* MagicCanvas;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button3;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button4;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button5;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button6;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button7;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (BindWidget))
	class UButton* Button8;

	UPROPERTY(EditAnywhere, Category = MagicControl)
	UMaterialInterface* MaterialPassive;

	UPROPERTY(EditAnywhere, Category = MagicControl)
	UMaterialInterface* MaterialActive;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = MagicControl)
		TArray<FVector2D> Positions;

	UPROPERTY(EditAnywhere, Category = MagicControl)
		TMap<FString, FSpellPoints> Spells;

protected:

	void CustomClicked(int Index);

	UFUNCTION(BlueprintImplementableEvent, Category = MagicControl)
	void CastedSpell(const FString& Spell);

	UFUNCTION()
	void EndSpell();

	UFUNCTION()
	void Click1();

	UFUNCTION()
	void Click2();

	UFUNCTION()
	void Click3();

	UFUNCTION()
	void Click4();

	UFUNCTION()
	void Click5();

	UFUNCTION()
	void Click6();

	UFUNCTION()
	void Click7();

	UFUNCTION()
	void Click8();

private:
	bool bTouched;

	FTimerHandle FTimeBeforeShutdown;

	TArray<UButton*> Buttons;

	TArray<int32> Points;
};
