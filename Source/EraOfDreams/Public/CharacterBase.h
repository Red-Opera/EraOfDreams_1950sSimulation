#pragma once

#include "CoreMinimal.h"
#include "Samples/GameAnimationSampleCharacter.h"
#include "Animation/AnimMontage.h"
#include "Sound/SoundBase.h"
#include "MoveParkoruAsset/ParkoruAudioBankInterface.h"
#include "CharacterBase.generated.h"

UCLASS()
class ERAOFDREAMS_API ACharacterBase : public AGameAnimationSampleCharacter, public IParkoruAudioBankInterface 
{
    GENERATED_BODY()
    
public:
    ACharacterBase();

protected:

private:
};
