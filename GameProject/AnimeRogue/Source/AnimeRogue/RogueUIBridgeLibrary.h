#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RogueTypes.h"
#include "RogueUIBridgeLibrary.generated.h"

class ANodeFlowManager;
class ARunManager;
class ABattleManager;
class AEventManager;
class AShopManager;

UCLASS()
class ANIMEROGUE_API URogueUIBridgeLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintPure, Category="AnimeRogue|UI")
    static FUIRuntimeSnapshot BuildUIRuntimeSnapshot(
        ANodeFlowManager* NodeFlowManager,
        ARunManager* RunManager,
        ABattleManager* BattleManager,
        AEventManager* EventManager,
        AShopManager* ShopManager
    );

    UFUNCTION(BlueprintPure, Category="AnimeRogue|UI")
    static FText ToNodeTypeText(EMapNodeType NodeType);

    UFUNCTION(BlueprintPure, Category="AnimeRogue|UI")
    static FText ToFlowStateText(ENodeFlowState FlowState);
};
