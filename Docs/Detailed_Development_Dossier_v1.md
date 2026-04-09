# 详细开发文案 v1（AnimeRogue）

## 1. 项目目标与边界
- 类型：纯单机、回合制卡牌肉鸽
- 目标：玩法循环对齐 StS，内容与视觉二次元原创
- 当前代码目标：先形成稳定 MVP 主循环，再扩展内容规模

## 2. 当前架构（已实现）
- `ABattleManager`
  - 回合状态机
  - 结束战斗后 3 选 1 卡牌奖励
  - 敌人意图脚本执行（`atkN`/`defN`）
  - 玩家/敌人状态与战斗日志
- `UCardSystemComponent`
  - 抽牌/弃牌/洗牌
  - 出牌扣费
  - 卡牌费用由 `CardDataTable` 驱动
- `ARunManager`
  - 单局推进（楼层与节点）
  - 单局资源（HP、金币、卡组、遗物）
  - 商店行为（买卡、买遗物、删卡）
  - 事件脚本执行器

## 3. 数据与文件映射
- 卡牌模板：`DataTemplates/DT_Cards_Template.csv`
- 意图模板：`DataTemplates/DT_EnemyIntentScripts.csv`
- 二次元怪物模板：`DataTemplates/DT_AnimeMonsters_Act1.csv`
- 二次元事件模板：`DataTemplates/DT_AnimeEvents_Act1.csv`
- 地图样例：`DataTemplates/Map_Act1_Sample.json`

## 4. 战斗循环（当前版本）
1. `StartBattle()`
2. 玩家回合开始：回能量、抽牌、显示敌方下个意图
3. 玩家出牌：`TryPlayCardFromHand(index)`
4. 玩家结束回合：`EndPlayerTurn()`
5. 敌方执行意图：
   - `atkN` -> 扣玩家格挡后扣血
   - `defN` -> 敌方加格挡
6. 胜利时：`CompleteBattle(true)` -> 触发奖励
7. 领取奖励：`PickRewardCard(index, bSkip)`

## 5. 跑图循环（当前版本）
1. `StartRun(seed)` 初始化单局
2. 节点推进 `AdvanceToNextNode()`
3. 根据节点类型进入对应流程：
   - Battle/Elite/Boss -> 战斗
   - Event -> `ExecuteEventScript(scriptId)`
   - Shop -> `BuyCard / BuyRandomRelic / RemoveCardAtShop`
   - Rest -> `HealPlayer`

## 6. 事件脚本执行规则（当前）
- 已实现：
  - `event_gain_gold_50`, `event_gain_gold_40`
  - `event_heal_small`, `event_heal_20pct`
  - `event_take_damage_get_relic`
  - `event_remove_card`
  - `event_add_skill_card`, `event_gain_attack_card`, `event_gain_defense_card`
  - `event_buy_random_relic`
  - `event_pay_hp_get_rare_card`, `event_curse_for_rare`
  - `event_nothing`, `event_leave`
- 未实现脚本会写日志，不会崩溃

## 7. UE5 接入步骤（本周）
1. 重新生成工程并编译
2. 导入 CSV 为 DataTable
3. 在关卡放置：
   - `BattleManager`
   - `RunManager`
4. 在 `BattleManager` 面板设置：
   - `CardDataTable`
   - `EnemyIntentScriptTable`
   - `EnemyIntentScriptId`
5. 在 `Level Blueprint`：
   - BeginPlay 调 `StartRun(12345)` 与 `StartBattle()`
   - 按钮事件调用 `EndPlayerTurn()`
   - 结算测试调用 `CompleteBattle(true)`

## 8. 验收标准（MVP 第 1 门）
- 能跑完 `StartRun -> Battle -> Reward -> NextNode`
- 战斗不会在 `atk/def` 脚本下卡住
- 事件脚本执行不会导致崩溃
- UI 可读取运行日志并展示

## 9. 下一阶段开发任务（按优先级）
1. 卡牌效果执行器（按 `EffectScriptId` 真正造成伤害/格挡/抽牌）
2. 敌方多单位战斗
3. 商店 UI 与实际卡池刷新规则
4. 休息点“升级卡牌”流程
5. 局内/局外存档真正落 `USaveGame`

## 10. 风险控制
- 风险：系统扩展快于可视化调试
  - 措施：所有关键入口写 `RunLog/BattleLog`
- 风险：数据行缺失造成空引用
  - 措施：所有读表失败回退默认行为并记日志
- 风险：蓝图绑定混乱
  - 措施：统一只读接口，写操作集中在 Manager
