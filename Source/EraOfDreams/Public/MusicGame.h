// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MusicGame.generated.h"

UCLASS()
class ERAOFDREAMS_API AMusicGame : public AActor
{
	GENERATED_BODY()
	
public:	
	AMusicGame();

	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool isStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool isGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	class ULevelSequence* gameSequence;			// 게임 레벨 시퀀스

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCollisionPlayer(class UPrimitiveComponent* overlappedComponent, 
						class AActor* otherActor, 
						class UPrimitiveComponent* otherComponent, 
						int32 otherBodyIndex, 
						bool bFromSweep, 
						const FHitResult& SweepResult);

private:
	void ChangeToGameCamera();			// 플레이어에서 영상 카메라로 이동

	class ACharacter* player;					// 플레이어 오브젝트
	class APlayerController* playerController;	// 플레이어 컨트롤러
};
