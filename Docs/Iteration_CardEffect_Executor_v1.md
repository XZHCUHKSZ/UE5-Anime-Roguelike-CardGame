# 当前迭代说明：卡牌效果执行器 v1

## 本次新增
- 出牌后不再只扣能量，已接入真实效果执行：
  - 攻击：对敌方扣血（先扣格挡）
  - 防御：玩家获得格挡
  - 抽牌：立即抽牌
- 若 `EffectScriptId` 未实现，使用 fallback：
  - Attack 卡按 `BaseValue` 伤害
  - Skill 卡按 `BaseValue` 格挡
- 敌方 HP <= 0 时自动触发 `CompleteBattle(true)`

## 涉及代码
- `BattleManager::TryPlayCardFromHand`
- `BattleManager::ApplyCardEffectById`
- `BattleManager::FindCardData`
- `BattleManager::ApplyDamageToEnemy`

## 效果
- 当前可在 UE 内验证“打牌 -> 敌人掉血 -> 战斗胜利 -> 三选一卡奖励”闭环
