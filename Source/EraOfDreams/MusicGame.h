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
	// Sets default values for this actor's properties
	AMusicGame();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* boxComponent;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool isStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	bool isGameStart;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State")
	class ULevelSequence* gameSequence;			// ���� ���� ������

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UFUNCTION()
	void OnCollisionPlayer(class UPrimitiveComponent* overlappedComponent, 
						class AActor* otherActor, 
						class UPrimitiveComponent* otherComponent, 
						int32 otherBodyIndex, 
						bool bFromSweep, 
						const FHitResult& SweepResult);

private:
	void ChangeToGameCamera();			// �÷��̾�� ���� ī�޶�� �̵�

	class ACharacter* player;					// �÷��̾� ������Ʈ
	class APlayerController* playerController;	// �÷��̾� ��Ʈ�ѷ�
};