# 当前迭代说明：战斗内恢复 Continue v1

## 本次目标
- Continue 不仅恢复楼层节点，还恢复到“战斗进行中”的现场

## 关键实现
1. `FRunSaveData` 扩展战斗字段
   - `BattlePhase`
   - `BattleEnergy`
   - `BattlePlayerState / BattleEnemyState`
   - `BattleEnemyIntentScriptId / BattleEnemyIntentIndex / BattleCurrentEnemyIntent`
   - `BattleDrawPile / BattleHand / BattleDiscardPile`
2. `CardSystemComponent`
   - `BuildSaveData()`
   - `ApplySaveData()`
3. `BattleManager`
   - `BuildBattleSaveData()`
   - `ApplyBattleSaveData()`
4. `SaveManager`
   - `SaveFlowState(...)`
   - `LoadFlowState(...)`
5. `NodeFlowManager`
   - `StartFlowFromSave()` 走 `LoadFlowState`
   - `RestoreFromLoadedData(...)` 按 flow 状态恢复现场

## 当前效果
- Continue 时：
  - InBattle：恢复手牌/牌堆/能量/敌我状态/当前意图
  - InEvent：恢复原事件选项
  - InShop：恢复原商店商品

## 说明
- 这一版已经能完成“战斗中断恢复”的核心闭环
