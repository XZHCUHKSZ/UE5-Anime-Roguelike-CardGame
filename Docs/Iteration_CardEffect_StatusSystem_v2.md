# 当前迭代说明：卡牌效果执行器 v2 + 状态系统

## 本次新增
- `FUnitState` 新增 `StatusStacks`
- 战斗内状态支持：
  - `Strength`
  - `Weak`
  - `Vulnerable`
  - `Frail`
  - `Bleed`
  - `Thorns`
  - `NextAttackBonus`

## 伤害与防御规则
- 输出伤害修正：
  - `Strength` 增伤
  - `Weak` 伤害衰减
  - `NextAttackBonus` 一次性加伤（出手后消耗）
- 受伤修正：
  - `Vulnerable` 受到伤害增加
- 格挡修正：
  - `Frail` 降低获得格挡
- 回合开始状态结算：
  - `Bleed` 造成伤害并衰减 1 层
  - `Weak/Vulnerable/Frail` 每回合衰减

## 新接入/完善的卡牌效果
- `effect_two_hit_damage`
- `effect_apply_bleed_2`
- `effect_gain_block_and_thorns`
- `effect_gain_1_energy`
- `effect_discard_draw`
- `effect_next_attack_plus_damage`
- `effect_bonus_vs_block`
- `effect_heal_small`
- `effect_signature_execute`

## 敌方意图扩展
- `buff_strN`
- `debuff_weakN`
- `debuff_frailN`
- 仍未实现的 token 会记录日志
