#pragma once

#include "Blueprint/UserWidget.h"
#include "mtd.h"
#include "MTD_CoreTypes.h"

#include "MTD_PlayerHudWidget.generated.h"

class UMTD_HealthComponent;

UCLASS()
class MTD_API UMTD_PlayerHudWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    virtual bool Initialize() override;

    UFUNCTION(BlueprintCallable, Category="MTD|UI")
    bool IsPlayerAlive() const;

    UFUNCTION(BlueprintCallable, Category="MTD|UI")
    bool IsPlayerSpectating() const;

private:
};
