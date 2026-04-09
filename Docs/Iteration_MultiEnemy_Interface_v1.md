# 当前迭代说明：多敌战斗接口 v1

## 新增能力
- `BattleManager.ConfigureEncounter(EnemyIds)`
- `BattleManager.GetEnemyUnits()`
- `RogueTypes` 新增：
  - `FEnemyDataRow`
  - `FEnemyRuntimeState`

## 行为说明
- 支持配置多个敌人并在 UI 中读取敌人数组
- 当前战斗结算仍以“首个存活敌人”为主目标（兼容现有逻辑）
- 为后续真正多目标选取和群体攻击打基础

## 数据支持
- 若配置了 `EnemyDataTable`，会读取每个敌人的：
  - `HpMax`
  - `IntentPattern`
- 未配置时回退到默认 HP 与默认意图
