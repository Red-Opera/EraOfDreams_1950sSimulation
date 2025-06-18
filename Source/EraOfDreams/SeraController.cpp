#include "SeraController.h"
#include "KeyInputManager.h"

void ASeraController::BeginPlay()
{
	Super::BeginPlay();

	const TArray<FKey> keys = { EKeys::G, EKeys::Q };

	for (const FKey& key : keys)
	{
		InputComponent->BindKey(key, IE_Pressed, this, &ASeraController::OnAnyKeyPressed);
		InputComponent->BindKey(key, IE_Released, this, &ASeraController::OnAnyKeyReleased);
	}
}

void ASeraController::OnAnyKeyPressed(FKey Key)
{
	KeyInputManager::KeyDown(Key);
}

void ASeraController::OnAnyKeyReleased(FKey Key)
{
	KeyInputManager::KeyUp(Key);
}
