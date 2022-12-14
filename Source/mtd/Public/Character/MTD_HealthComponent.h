#pragma once

#include "Components/ActorComponent.h"
#include "mtd.h"

#include "MTD_HealthComponent.generated.h"

class UMTD_AbilitySystemComponent;
class UMTD_HealthSet;
class UMTD_HealthComponent;
struct FGameplayEffectSpec;
struct FOnAttributeChangeData;

UENUM(BlueprintType)
enum class EMTD_DeathState : uint8
{
    NotDead = 0,
    DeathStarted,
    DeathFinished
};

UCLASS(Blueprintable, ClassGroup="Character", meta=(BlueprintSpawnableComponent))
class MTD_API UMTD_HealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathEventSignature, AActor*, OwningActor);

    DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(
        FHealthAttributeChangedSignature,
        UMTD_HealthComponent*, HealthComponent,
        float, OldValue,
        float, NewValue,
        AActor*, Instigator);

public:
    UMTD_HealthComponent();

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    static UMTD_HealthComponent *FindHealthComponent(const AActor *Actor);

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    void InitializeWithAbilitySystem(UMTD_AbilitySystemComponent *Asc);

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    void UninitializeFromAbilitySystem();

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    float GetHealth() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    float GetMaxHealth() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    float GetHealthNormilized() const;

    UFUNCTION(BlueprintCallable, Category="MTD|Health")
    EMTD_DeathState GetDeathState() const;

    UFUNCTION(BlueprintCallable, BlueprintPure=false, Category="MTD|Health",
        meta=(ExpandBoolAsExecs="ReturnValue"))
    bool IsDeadOrDying() const;

    virtual void StartDeath();
    virtual void FinishDeath();

    virtual void SelfDestruct(bool bFeelOutOfWorld = false);

protected:
    virtual void OnUnregister() override;

    virtual void ClearGameplayTags();

    virtual void OnHealthChanged(const FOnAttributeChangeData &ChangeData);
    virtual void OnMaxHealthChanged(const FOnAttributeChangeData &ChangeData);
    virtual void OnOutOfHealth(
        AActor *DamageInstigator,
        AActor *DamageCauser,
        const FGameplayEffectSpec &DamageEffectSpec,
        float DamageMagnitude);

public:
    UPROPERTY(BlueprintAssignable)
    FDeathEventSignature OnDeathStarted;

    UPROPERTY(BlueprintAssignable)
    FDeathEventSignature OnDeathFinished;

    UPROPERTY(BlueprintAssignable)
    FHealthAttributeChangedSignature OnHealthChangedDelegate;

    UPROPERTY(BlueprintAssignable)
    FHealthAttributeChangedSignature OnMaxHealthChangedDelegate;

private:
    UPROPERTY()
    TObjectPtr<UMTD_AbilitySystemComponent> AbilitySystemComponent = nullptr;

    UPROPERTY()
    TObjectPtr<const UMTD_HealthSet> HealthSet = nullptr;

    UPROPERTY()
    EMTD_DeathState DeathState = EMTD_DeathState::NotDead;
};

inline UMTD_HealthComponent *UMTD_HealthComponent::FindHealthComponent(const AActor *Actor)
{
    return (IsValid(Actor)) ? (Actor->FindComponentByClass<UMTD_HealthComponent>()) : (nullptr);
}

inline EMTD_DeathState UMTD_HealthComponent::GetDeathState() const
{
    return DeathState;
}

inline bool UMTD_HealthComponent::IsDeadOrDying() const
{
    return (DeathState > EMTD_DeathState::NotDead);
}
