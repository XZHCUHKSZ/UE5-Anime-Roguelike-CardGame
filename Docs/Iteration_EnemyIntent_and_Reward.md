# 当前迭代说明：敌人意图执行 + 三选一卡奖励

## 本次代码能力
- 敌人意图支持从 `EnemyIntentScriptTable` 读取 `pattern`（如 `atk6|atk6|def5`）
- 敌人回合可执行：
  - `atkN`：对玩家造成伤害（先扣格挡）
  - `defN`：敌人获得格挡
- 玩家/敌人状态可蓝图读取：
  - `GetPlayerState()`
  - `GetEnemyState()`
  - `GetCurrentEnemyIntent()`
  - `GetBattleLog()`
- 战斗胜利后触发三选一卡：
  - `CompleteBattle(true)`
  - `GetCurrentRewardOptions()`
  - `PickRewardCard(index, bSkip)`

## UE中接线建议
1. 在 `BattleManager` 细节面板设置：
   - `CardDataTable` -> 导入后的卡牌DT
   - `EnemyIntentScriptTable` -> 导入后的意图DT
   - `EnemyIntentScriptId` -> 例如 `intent_basic_melee`
2. BeginPlay 调 `StartBattle`
3. 每次玩家点结束回合调 `EndPlayerTurn`
4. 用 UI 绑定 `GetBattleLog` 显示最近日志
