#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "RogueTypes.generated.h"

UENUM(BlueprintType)
enum class EBattlePhase : uint8
{
    BattleStart UMETA(DisplayName = "Battle Start"),
    PlayerTurnStart UMETA(DisplayName = "Player Turn Start"),
    PlayerMain UMETA(DisplayName = "Player Main"),
    PlayerTurnEnd UMETA(DisplayName = "Player Turn End"),
    EnemyTurnStart UMETA(DisplayName = "Enemy Turn Start"),
    EnemyMain UMETA(DisplayName = "Enemy Main"),
    EnemyTurnEnd UMETA(DisplayName = "Enemy Turn End"),
    BattleEnd UMETA(DisplayName = "Battle End")
};

UENUM(BlueprintType)
enum class ECardRarity : uint8
{
    Common,
    Uncommon,
    Rare
};

UENUM(BlueprintType)
enum class ECardType : uint8
{
    Attack,
    Skill,
    Power
};

UENUM(BlueprintType)
enum class ECardTargetType : uint8
{
    Self,
    Enemy,
    AllEnemies,
    AllUnits
};

UENUM(BlueprintType)
enum class EMapNodeType : uint8
{
    Battle,
    Event,
    Shop,
    Rest,
    Elite,
    Boss
};

USTRUCT(BlueprintType)
struct FCardData : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName CardId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Name;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 Cost = 1;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECardRarity Rarity = ECardRarity::Common;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECardType CardType = ECardType::Attack;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    ECardTargetType TargetType = ECardTargetType::Enemy;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 BaseValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    int32 UpgradeValue = 0;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EffectScriptId = NAME_None;
};

USTRUCT(BlueprintType)
struct FRuntimeCard
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CardId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bUpgraded = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TempCostOffset = 0;
};

USTRUCT(BlueprintType)
struct FUnitState
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 HP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxHP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Block = 0;
};

USTRUCT(BlueprintType)
struct FCardRewardOption
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CardId = NAME_None;
};

USTRUCT(BlueprintType)
struct FMapNodeData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Floor = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EMapNodeType NodeType = EMapNodeType::Battle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 NodeIndex = 0;
};

USTRUCT(BlueprintType)
struct FEnemyIntentScriptRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName IntentScriptId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Pattern;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Description;
};
