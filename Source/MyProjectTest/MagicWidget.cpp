// Fill out your copyright notice in the Description page of Project Settings.


#include "MagicWidget.h"
#include "TimerManager.h"
#include "Components/Image.h"
#include "Components/Button.h"
#include "Blueprint/SlateBlueprintLibrary.h"
#include "Blueprint/WidgetBlueprintLibrary.h"

#define print(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 1.5, FColor::White,text)

UMagicWidget::UMagicWidget(const FObjectInitializer & ObjectInitializer) : Super(ObjectInitializer)
{

}

void UMagicWidget::NativeConstruct()
{
	Super::NativeConstruct();

	FSpellPoints ShieldSpell;
	FSpellPoints ThrowSpell;
	FSpellPoints CircleThrowSpell;
	FSpellPoints FireBallSpell;
	FSpellPoints FlockSpell;
	FSpellPoints TransformSpell;
	FSpellPoints HomingballSpell;
	ShieldSpell.Points = {
		0,
		1
	};
	ThrowSpell.Points = {
		3,
		1
	};
	CircleThrowSpell.Points = {
		1,
		2,
		4,
		6,
		7,
		5,
		3,
		0,
		1
	};
	FireBallSpell.Points = {
		3,
		4
	};
	FlockSpell.Points = {
		2,
		4
	};
	TransformSpell.Points = {
		1,
		3
	};
	HomingballSpell.Points = {
		0,
		7,
		2
	};
	Spells.Add("Shield", ShieldSpell);
	Spells.Add("Throw", ThrowSpell);
	Spells.Add("ForceWave", CircleThrowSpell);
	Spells.Add("Fireball", FireBallSpell);
	Spells.Add("Flock", FlockSpell);
	Spells.Add("Transform", TransformSpell);
	Spells.Add("Homingball", HomingballSpell);

	Button1->OnClicked.AddDynamic(this, &UMagicWidget::Click1);
	Button2->OnClicked.AddDynamic(this, &UMagicWidget::Click2);
	Button3->OnClicked.AddDynamic(this, &UMagicWidget::Click3);
	Button4->OnClicked.AddDynamic(this, &UMagicWidget::Click4);
	Button5->OnClicked.AddDynamic(this, &UMagicWidget::Click5);
	Button6->OnClicked.AddDynamic(this, &UMagicWidget::Click6);
	Button7->OnClicked.AddDynamic(this, &UMagicWidget::Click7);
	Button8->OnClicked.AddDynamic(this, &UMagicWidget::Click8);
}

void UMagicWidget::SynchronizeProperties()
{
	Super::SynchronizeProperties();

	FButtonStyle tempStyle;
	FSlateBrush tempBrush;
	tempBrush.SetResourceObject(MaterialPassive);
	tempBrush.ImageSize = FVector2D(32, 32);
	tempBrush.DrawAs = ESlateBrushDrawType::Image;
	tempStyle.Normal = tempBrush;
	tempStyle.Hovered = tempBrush;
	tempStyle.Pressed = tempBrush;

	Buttons.Emplace(Button1);
	Buttons.Emplace(Button2);
	Buttons.Emplace(Button3);
	Buttons.Emplace(Button4);
	Buttons.Emplace(Button5);
	Buttons.Emplace(Button6);
	Buttons.Emplace(Button7);
	Buttons.Emplace(Button8);

	for (size_t i = 0; i < 8; i++)
	{
		Buttons[i]->SetStyle(tempStyle);
	}
}

FVector2D UMagicWidget::GetImagePosition(int32 Index)
{
	FVector2D PixelPos;
	FVector2D ViewportPos;
	FVector2D ImageSize = Buttons[Index]->GetCachedGeometry().GetLocalSize();
	USlateBlueprintLibrary::LocalToViewport(GetWorld(), Buttons[Index]->GetCachedGeometry(), FVector2D(0, 0), PixelPos, ViewportPos);
	return FVector2D(ViewportPos.X + ImageSize.X / 2, ViewportPos.Y + ImageSize.Y / 2);
}

bool UMagicWidget::CheckPoints(int32 Index)
{
	int32 Num = Points.Num();
	if (Num < 2) {
		return true;
	}
	int32 pointA = Index;
	int32 pointB = Points.Last();
	if (pointA == pointB)
	{
		return false;
	}
	for (int32 i = 0; i < Num - 1; i++)
	{
		if (pointA == Points[i] && pointB == Points[i + 1])
		{
			return false;
		}
		if (pointB == Points[i] && pointA == Points[i + 1])
		{
			return false;
		}
	}
	return true;
}

FString UMagicWidget::CheckForSpell()
{
	for (auto& Elem : Spells)
	{
		if (Elem.Value.Points == Points)
		{
			CastedSpell(Elem.Key);
			EndSpell();
			print(FString::Printf(TEXT("Spell is %s"), &Elem.Key));
		}
	}
	return "SPELL_NONE";
}

void UMagicWidget::CustomClicked(int Index)
{
	FButtonStyle tempStyle;
	FSlateBrush tempBrush;
	tempBrush.SetResourceObject(MaterialPassive);
	tempBrush.ImageSize = FVector2D(32, 32);
	tempBrush.DrawAs = ESlateBrushDrawType::Image;
	tempStyle.Normal = tempBrush;
	tempStyle.Hovered = tempBrush;
	tempStyle.Pressed = tempBrush;

	
	if (CheckPoints(Index))
	{
		Points.Add(Index);
		Positions.Add(GetImagePosition(Index));
		tempBrush.SetResourceObject(MaterialActive);
		tempStyle.Normal = tempBrush;
		tempStyle.Hovered = tempBrush;
		tempStyle.Pressed = tempBrush;
		Buttons[Index]->SetStyle(tempStyle);

		CheckForSpell();

		FTimerDelegate CustomShutdownDelegate;
		CustomShutdownDelegate.BindUFunction(this, FName("EndSpell"));
		GetWorld()->GetTimerManager().SetTimer(FTimeBeforeShutdown, CustomShutdownDelegate, 4.0f, false);
	}
	else
	{
		EndSpell();
	}
}

void UMagicWidget::EndSpell()
{
	FButtonStyle tempStyle;
	FSlateBrush tempBrush;
	tempBrush.SetResourceObject(MaterialPassive);
	tempBrush.ImageSize = FVector2D(32, 32);
	tempBrush.DrawAs = ESlateBrushDrawType::Image;
	tempStyle.Normal = tempBrush;
	tempStyle.Hovered = tempBrush;
	tempStyle.Pressed = tempBrush;
	Points.Empty();
	Positions.Empty();
	for (int i = 0; i < 8; i++)
	{
		Buttons[i]->SetStyle(tempStyle);
	}
}

void UMagicWidget::Click1()
{
	CustomClicked(0);
}

void UMagicWidget::Click2()
{
	CustomClicked(1);
}

void UMagicWidget::Click3()
{
	CustomClicked(2);
}

void UMagicWidget::Click4()
{
	CustomClicked(3);
}

void UMagicWidget::Click5()
{
	CustomClicked(4);
}

void UMagicWidget::Click6()
{
	CustomClicked(5);
}

void UMagicWidget::Click7()
{
	CustomClicked(6);
}

void UMagicWidget::Click8()
{
	CustomClicked(7);
}
