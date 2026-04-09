# 当前迭代说明：多敌行动 + AOE 真多目标 v2

## 本次升级
- 敌方回合从“单敌行动”升级为“所有存活敌人依次行动”
- `effect_aoe_damage` 改为命中所有存活敌人

## 接口
- `ConfigureEncounter(EnemyIds)` 可传多个敌人
- `GetEnemyUnits()` 可供 UI 展示敌人列表

## 当前限制
- 单体卡依旧默认攻击“首个存活敌人”
- 后续版本会补“玩家手动选定目标”
