﻿#pragma once

#include "mtd.h"
#include "Core/MTD_CoreTypes.h"

#include "MTD_Utilities.generated.h"

USTRUCT()
struct FMTD_Utilities
{
	GENERATED_BODY()
	
public:
	template <class T>
	static T *GetActorComponent(AActor *InActor);
	
	template <class T>
	static const T *GetActorComponent(const AActor *InActor);
	
	template <class T>
	static T *GetPawnController(APawn *InPawn);
	
	template <class T>
	static const T *GetPawnController(const APawn *InPawn);

	template <class T>
	static T *FindFirstNotifyByClass(UAnimSequenceBase *Anim);
	
	static FGenericTeamId GetMtdGenericTeamId(const AActor *InActor);
	static ETeamAttitude::Type GetMtdTeamAttitudeBetween(
		const AActor *Lhs, const AActor *Rhs);
	static EMTD_TeamId GenericToMtdTeamId(FGenericTeamId GenericId);
};

template <class T>
T *FMTD_Utilities::GetActorComponent(AActor *InActor)
{
	if (!IsValid(InActor))
		return nullptr;

	UActorComponent *Comp = InActor->GetComponentByClass(T::StaticClass());
	if (!IsValid(Comp))
		return nullptr;
	
	return Cast<T>(Comp);
}

template <class T>
const T *FMTD_Utilities::GetActorComponent(const AActor *InActor)
{
	if (!IsValid(InActor))
		return nullptr;

	UActorComponent *Comp = InActor->GetComponentByClass(T::StaticClass());
	if (!IsValid(Comp))
		return nullptr;
	
	return Cast<T>(Comp);
}

template <class T>
T *FMTD_Utilities::GetPawnController(APawn *InPawn)
{
	if (!IsValid(InPawn))
		return nullptr;
	
	AController *Controller = InPawn->GetController();
	if (!IsValid(Controller))
		return nullptr;
	
	return Cast<T>(Controller);
}

template <class T>
const T *FMTD_Utilities::GetPawnController(const APawn *InPawn)
{
	if (!IsValid(InPawn))
		return nullptr;
	
	const AController *Controller = InPawn->GetController();
	if (!IsValid(Controller))
		return nullptr;
	
	return Cast<T>(Controller);
}

template<class T>
T *FMTD_Utilities::FindFirstNotifyByClass(UAnimSequenceBase *Anim)
{
	if (!IsValid(Anim))
		return nullptr;

	const TArray<FAnimNotifyEvent> &NotifyEvents = Anim->Notifies;
	for (auto &Item : NotifyEvents)
	{
		T *CastedItem = Cast<T>(Item.Notify);
		if (IsValid(CastedItem))
			return CastedItem;
	}
	return nullptr;
}