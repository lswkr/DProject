// Fill out your copyright notice in the Description page of Project Settings.


#include "AbilitySystem/Ability/PointsExplosion.h"

FString UPointsExplosion::GetDescription(int32 Level)
{
	return FString::Printf(TEXT(
				// Title
				"<Title>폭발</>\n\n"
				// Level
				"<Level>Level: </><Level>%d</>\n"
				// ManaCost
				"<Default>지정된 곳에 폭발을 일으킨다.</>"
				),Level
				);
}
