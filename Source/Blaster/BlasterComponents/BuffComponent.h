
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
	void ReplenishShield(float ShieldAmount, float ReplenishTime);
	void BuffSpeed(float BuffBaseSpeed, float BuffCrouchSpeed, float BuffTime);
	void SetInitialSpeeds(float BaseSpeed, float CrouchSpeed);
	void SetInitialJumpVelocity(float Velocity);
	void BuffJump(float BuffJumpVelocity, float BuffTime);
	void BuffBerserk(float BuffFireDelay, float BuffTime);

	void SetInitialFireRate(float FireDelay);


protected:

	virtual void BeginPlay() override;
	void HealRampUp(float DeltaTime);
	void ShieldRampUp(float DeltaTime);

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
	/// Shield Buff
	//

	bool bReplenishingShield = false;
	float ShieldReplenishRate = 0;
	float ShieldReplenishAmount = 0.f;

	//
	/// Speed Buff
	//

	FTimerHandle SpeedBuffTimer;
	void ResetSpeeds();
	float InitialBaseSpeed;
	float InitialCrouchSpeed;
	bool bSpeedBuffActive = false;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastSpeedBuff(float BaseSpeed, float CrouchedSpeed);

	//
	/// JumpBuff
	// 

	FTimerHandle JumpBuffTimer;
	void ResetJump();
	float InitialJumpVelocity;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastJumpBuff(float JumpVelocity);

	//
	/// Berserk Buff
	//

	FTimerHandle BerserkBuffTimer;
	void ResetBerserk();
	float InitialFireDelay;

	UFUNCTION(NetMulticast, Reliable)
	void MulticastBerserkBuff(float BuffFireDelay);

public:	

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	FORCEINLINE FTimerHandle GetSpeedBuffTimer() const { return SpeedBuffTimer; }
	FORCEINLINE FTimerHandle GetJumpBuffTimer() const { return JumpBuffTimer; }
	FORCEINLINE FTimerHandle GetberserkBuffTimer() const { return BerserkBuffTimer; }
	FORCEINLINE bool GetbIsSpeedBuffActive() { return bSpeedBuffActive; }
};
