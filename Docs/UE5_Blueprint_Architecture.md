# UE5 蓝图架构（单机卡牌肉鸽）

## 1. 关键蓝图类（建议命名）
- `BP_GI_Rogue` (GameInstance)
- `BP_GM_Run` (GameMode)
- `BP_PC_Battle` (PlayerController)
- `BP_RunManager` (Actor or UObject)
- `BP_BattleManager` (Actor)
- `BP_PlayerUnit` / `BP_EnemyUnit` (Pawn or Actor)
- `BP_CardSystemComponent` (ActorComponent)
- `BP_StatusComponent` (ActorComponent)
- `BP_IntentComponent` (ActorComponent)
- `WBP_BattleHUD`, `WBP_HandPanel`, `WBP_CardItem`, `WBP_IntentIcon`

## 2. 回合状态机（枚举）
`E_BattlePhase`
- `BattleStart`
- `PlayerTurnStart`
- `PlayerMain`
- `PlayerTurnEnd`
- `EnemyTurnStart`
- `EnemyMain`
- `EnemyTurnEnd`
- `BattleEnd`

## 3. 数据结构（Struct）
- `S_CardData`：卡牌静态配置（来自DT_Cards）
- `S_RuntimeCard`：运行时卡实例（升级、临时费用变化）
- `S_UnitState`：单位属性（HP/Block/Buffs）
- `S_BattleSnapshot`：局内可存档状态
- `S_RunState`：当前爬塔进度

## 4. 执行顺序（玩家回合）
1. `OnPlayerTurnStart`: 回复能量、抽牌、触发回合开始Buff
2. `OnPlayCard`: 校验能量 -> 扣费 -> 执行效果脚本 -> 丢弃/消耗
3. `OnPlayerTurnEnd`: 触发回合结束Buff -> 切敌方回合

## 5. 效果脚本建议
- 用 `GameplayTag` + `DataDriven Effect` 映射
- 示例：`Effect.Damage.Direct` / `Effect.Block.Gain` / `Effect.Draw`
- 每个效果脚本统一入口：`ApplyCardEffect(EffectScriptId, Source, Target)`

## 6. 存档分层
- `Slot_Meta_0.sav`：局外解锁与设置
- `Slot_Run_0.sav`：当前局临时进度（可中断继续）

## 7. 今日开工顺序（建议）
1. 建 `S_CardData` + `DT_Cards` 导入模板CSV
2. 建 `BP_CardSystemComponent`，先打通抽牌/出牌/弃牌
3. 建 `BP_BattleManager` + `E_BattlePhase`
4. 建最简UI：手牌列表 + End Turn按钮
5. 做3张测试卡跑通首场战斗
