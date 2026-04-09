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

UENUM(BlueprintType)
enum class ENodeFlowState : uint8
{
    Idle,
    EnteringNode,
    InBattle,
    InEvent,
    InShop,
    InRest,
    RunComplete
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

USTRUCT(BlueprintType)
struct FEventDataRow : public FTableRowBase
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName EventId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FString Type;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText OptionA;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText OptionB;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FText OptionC;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ResultAScript = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ResultBScript = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadOnly)
    FName ResultCScript = NAME_None;
};

USTRUCT(BlueprintType)
struct FEventOptionData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Text;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName ScriptId = NAME_None;
};

USTRUCT(BlueprintType)
struct FResolvedEventData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EventId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEventOptionData> Options;
};

USTRUCT(BlueprintType)
struct FShopCardOffer
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName CardId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Price = 0;
};

USTRUCT(BlueprintType)
struct FRunSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRunActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 RunSeed = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Gold = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentHP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxHP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMapNodeData CurrentNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> DeckCardIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> RelicIds;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasPendingEvent = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FResolvedEventData PendingEvent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasPendingShop = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FShopCardOffer> PendingShopOffers;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENodeFlowState SavedFlowState = ENodeFlowState::Idle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasBattleSnapshot = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBattlePhase BattlePhase = EBattlePhase::BattleStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BattleEnergy = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState BattlePlayerState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState BattleEnemyState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName BattleEnemyIntentScriptId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 BattleEnemyIntentIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString BattleCurrentEnemyIntent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> BattleDrawPile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> BattleHand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> BattleDiscardPile;
};

USTRUCT(BlueprintType)
struct FMetaSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalRuns = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 TotalWins = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 AscensionLevel = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FName> UnlockedCharacters;
};

USTRUCT(BlueprintType)
struct FUIBattleSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Energy = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState PlayerState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState EnemyState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString EnemyIntent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> HandCards;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FCardRewardOption> RewardOptions;
};

USTRUCT(BlueprintType)
struct FUIEventSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EventId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FText Title;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FEventOptionData> Options;
};

USTRUCT(BlueprintType)
struct FUIShopSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FShopCardOffer> CardOffers;
};

USTRUCT(BlueprintType)
struct FUIRunSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bRunActive = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 Gold = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentHP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 MaxHP = 1;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FMapNodeData ActiveNode;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> RunLog;
};

USTRUCT(BlueprintType)
struct FUIRuntimeSnapshot
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    ENodeFlowState FlowState = ENodeFlowState::Idle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUIRunSnapshot Run;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUIBattleSnapshot Battle;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUIEventSnapshot Event;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUIShopSnapshot Shop;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FString> BattleLog;
};

USTRUCT(BlueprintType)
struct FCardSystemSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> DrawPile;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> Hand;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    TArray<FRuntimeCard> DiscardPile;
};

USTRUCT(BlueprintType)
struct FBattleSaveData
{
    GENERATED_BODY()

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bHasActiveBattle = false;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    EBattlePhase CurrentPhase = EBattlePhase::BattleStart;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 CurrentEnergy = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState PlayerState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FUnitState EnemyState;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FName EnemyIntentScriptId = NAME_None;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    int32 EnemyIntentIndex = 0;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FString CurrentEnemyIntent;

    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    FCardSystemSaveData CardSystem;
};
