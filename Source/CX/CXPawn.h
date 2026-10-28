

#pragma once

#include "CoreMinimal.h"
#include "WheeledVehiclePawn.h"
#include "CXPawn.generated.h"

class UCameraComponent;
class USpringArmComponent;
class UInputAction;
class UChaosWheeledVehicleMovementComponent;
struct FInputActionValue;

DECLARE_LOG_CATEGORY_EXTERN(LogTemplateVehicle, Log, All);


UCLASS(abstract)
class ACXPawn : public AWheeledVehiclePawn
{
	GENERATED_BODY()

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* FrontSpringArm;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* FrontCamera;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	USpringArmComponent* BackSpringArm;

	
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
	UCameraComponent* BackCamera;

	
	TObjectPtr<UChaosWheeledVehicleMovementComponent> ChaosVehicleMovement;

protected:

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* SteeringAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ThrottleAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* BrakeAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* HandbrakeAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* LookAroundAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ToggleCameraAction;

	
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input)
	UInputAction* ResetVehicleAction;

	
	bool bFrontCameraActive = false;

public:
	ACXPawn();

	

	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;

	

	

	virtual void Tick(float Delta) override;

	

protected:
    UPROPERTY(EditAnywhere, Category="Input") TObjectPtr<UInputAction> AttackAction;
    UFUNCTION(Server, Reliable) void ServerAttack();
    UFUNCTION(NetMulticast, Reliable) void MulticastAttack(const FVector& Start, const FVector& End, bool bHit);
    void Attack(const FInputActionValue& Value);
    virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;
    float LastAttackTime = -1.f;

    
	void Steering(const FInputActionValue& Value);

	
	void Throttle(const FInputActionValue& Value);

	
	void Brake(const FInputActionValue& Value);

	
	void StartBrake(const FInputActionValue& Value);
	void StopBrake(const FInputActionValue& Value);

	
	void StartHandbrake(const FInputActionValue& Value);
	void StopHandbrake(const FInputActionValue& Value);

	
	void LookAround(const FInputActionValue& Value);

	
	void ToggleCamera(const FInputActionValue& Value);

	
	void ResetVehicle(const FInputActionValue& Value);

	
	UFUNCTION(BlueprintImplementableEvent, Category="Vehicle")
	void BrakeLights(bool bBraking);

public:
	
	FORCEINLINE USpringArmComponent* GetFrontSpringArm() const { return FrontSpringArm; }
	
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FrontCamera; }
	
	FORCEINLINE USpringArmComponent* GetBackSpringArm() const { return BackSpringArm; }
	
	FORCEINLINE UCameraComponent* GetBackCamera() const { return BackCamera; }
	
	FORCEINLINE const TObjectPtr<UChaosWheeledVehicleMovementComponent>& GetChaosVehicleMovement() const { return ChaosVehicleMovement; }
};
