# 当前迭代说明：NodeFlowManager v1（自动节点流转）

## 新增类
- `ANodeFlowManager`

## 作用
- 统一协调 `RunManager + BattleManager + EventManager + ShopManager`
- 自动处理节点进入逻辑：
  - Battle/Elite/Boss -> 进入战斗
  - Event -> 抽事件并等待选项
  - Shop -> 刷商店并等待离开
  - Rest -> 自动回血后进入下一节点

## 关键接口
- `StartFlow(seed)`
- `CompleteBattleNode(bPlayerWon)`
- `ClaimBattleRewardAndAdvance(optionIndex, bSkip)`
- `ResolveEventAndAdvance(optionIndex)`
- `LeaveShopAndAdvance()`

## 蓝图接线建议
1. 关卡放置 5 个 Actor：
   - `NodeFlowManager`
   - `RunManager`
   - `BattleManager`
   - `EventManager`
   - `ShopManager`
2. 在 `NodeFlowManager` 细节面板把四个 Manager 引用拖进去
3. BeginPlay 调 `StartFlow(seed)`
4. 战斗胜利时：
   - `CompleteBattleNode(true)`
   - 选卡后 `ClaimBattleRewardAndAdvance`
5. 事件选择后：
   - `ResolveEventAndAdvance(optionIndex)`
6. 商店离开：
   - `LeaveShopAndAdvance()`

## 结果
- 你现在可以不手动写复杂分支，直接按节点类型推进单局流程
