﻿#include "Items/MTD_FloatingToken.h"

#include "Components/SphereComponent.h"
#include "Items/MTD_TokenMovementComponent.h"

AMTD_FloatingToken::AMTD_FloatingToken()
{
    PrimaryActorTick.bCanEverTick = true;
    PrimaryActorTick.bStartWithTickEnabled = true;
    PrimaryActorTick.TickInterval = 0.1f; // Don't run it too often

    CollisionComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Collision Component"));
    SetRootComponent(CollisionComponent);
    CollisionComponent->SetSphereRadius(25.f);

    CollisionComponent->SetCollisionProfileName(FloatingTokenCollisionProfileName);
    CollisionComponent->SetGenerateOverlapEvents(false);
    CollisionComponent->SetCanEverAffectNavigation(false);

    ActivationTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Activation Trigger Component"));
    ActivationTriggerComponent->SetupAttachment(GetRootComponent());
    ActivationTriggerComponent->SetSphereRadius(50.f);

    ActivationTriggerComponent->SetGenerateOverlapEvents(true);
    ActivationTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    ActivationTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    ActivationTriggerComponent->SetCollisionResponseToAllChannels(ECR_Ignore);
    ActivationTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECR_Overlap);
    ActivationTriggerComponent->SetCanEverAffectNavigation(false);

    DetectTriggerComponent = CreateDefaultSubobject<USphereComponent>(TEXT("Detect Trigger Component"));
    DetectTriggerComponent->SetupAttachment(GetRootComponent());
    DetectTriggerComponent->SetSphereRadius(500.f);

    DetectTriggerComponent->SetGenerateOverlapEvents(true);
    DetectTriggerComponent->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
    DetectTriggerComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
    DetectTriggerComponent->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
    DetectTriggerComponent->SetCollisionResponseToChannel(PlayerCollisionChannel, ECollisionResponse::ECR_Overlap);
    DetectTriggerComponent->SetCanEverAffectNavigation(false);

    MovementComponent = CreateDefaultSubobject<UMTD_TokenMovementComponent>(TEXT("Movement Component"));
}

void AMTD_FloatingToken::Tick(float DeltaSeconds)
{
    Super::Tick(DeltaSeconds);

    const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
    
    // Try to find a new target if there's none
    if (!IsValid(Target))
    {
        const AActor *NewTarget = FindNewTarget();
        MovementComponent->HomingTargetComponent = (IsValid(NewTarget)) ? (NewTarget->GetRootComponent()) : (nullptr);
    }
}

void AMTD_FloatingToken::BeginPlay()
{
    Super::BeginPlay();

    CollisionComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnCollisionBeginOverlap);

    DetectTriggerComponent->OnComponentBeginOverlap.AddDynamic(this, &ThisClass::OnTriggerBeginOverlap);
    DetectTriggerComponent->OnComponentEndOverlap.AddDynamic(this, &ThisClass::OnTriggerEndOverlap);
}

bool AMTD_FloatingToken::CanBeActivatedOn(APawn *Pawn) const
{
    return true;
}

void AMTD_FloatingToken::OnActivate_Implementation(APawn *Pawn)
{
    Destroy();
}

APawn *AMTD_FloatingToken::FindNewTarget() const
{
    return nullptr;
}

void AMTD_FloatingToken::OnPawnAdded(APawn *Pawn)
{
    // Home towards the actor if there's no target and the actor isn't full
    const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
    if ((!IsValid(Target)) && (CanBeActivatedOn(Pawn)))
    {
        const FVector P0 = GetActorLocation();
        const FVector P1 = Pawn->GetActorLocation();
        const FVector Displacement = P1 - P0;
        
        FVector Direction;
        float Distance;
        
        Displacement.ToDirectionAndLength(Direction, Distance);
        
        MovementComponent->SetUpdatedComponent(GetRootComponent());
        MovementComponent->AddForce(Direction * (FMath::Min(Distance * 0.1f, 10.f)));
        MovementComponent->HomingTargetComponent = Pawn->GetRootComponent();
    }
}

void AMTD_FloatingToken::OnPawnRemoved(APawn *Pawn)
{
    // If the actor was the homing target, select the first players from the detected players if any remained
    const USceneComponent *Target = MovementComponent->HomingTargetComponent.Get();
    if ((IsValid(Target)) && (Pawn == Target->GetOwner()))
    {
        USceneComponent *NewTarget = (DetectedPawns.IsEmpty()) ? (nullptr) : (DetectedPawns[0]->GetRootComponent());
        MovementComponent->HomingTargetComponent = NewTarget;
    }
}

void AMTD_FloatingToken::OnCollisionBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (CanBeActivatedOn(Pawn))
    {
        OnActivate(Pawn);
    }
}

void AMTD_FloatingToken::OnTriggerBeginOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex,
    bool bFromSweep,
    const FHitResult &SweepResult)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Pawn, TEXT("Actor [%s] is not a Pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedPawns.Add(Pawn);
    OnPawnAdded(Pawn);
}

void AMTD_FloatingToken::OnTriggerEndOverlap(
    UPrimitiveComponent *OverlappedComponent,
    AActor *OtherActor,
    UPrimitiveComponent *OtherComp,
    int32 OtherBodyIndex)
{
    auto Pawn = Cast<APawn>(OtherActor);
    if (!ensureMsgf(Pawn, TEXT("Actor [%s] is not a Pawn."), *OtherActor->GetName()))
    {
        return;
    }
    
    DetectedPawns.Remove(Pawn);
    OnPawnRemoved(Pawn);
}