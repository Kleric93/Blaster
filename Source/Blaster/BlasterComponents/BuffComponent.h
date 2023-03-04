
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "BuffComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class BLASTER_API UBuffComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UBuffComponent();
	friend class ABlasterCharacter;
	void Heal(float HealAmount, float HealingTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);

protected:

	virtual void BeginPlay() override;
	void healRampUp(float DeltaTime);

private:

	UPROPERTY()
	class ABlasterCharacter* Character;

	//
	/// Health Buff
	//

	bool bHealing = false;
	float HealingRate = 0;
	float AmountToHeal = 0.f;

	//
	/// Speed Buff
	//

	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchedSpeed);

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

		
};
