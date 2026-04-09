# 技术文案 v1（UE5 单机二次元卡牌肉鸽）

## 1. 技术目标
- 引擎：UE5（C++ + Blueprint）
- 形态：纯单机、离线可玩、可本地存档
- 当前里程碑：先打通“1场战斗闭环”再扩展地图与局外成长

## 2. 目录与模块
- 工程根：`GameProject/AnimeRogue`
- C++ 模块：`AnimeRogue`
- 核心代码：
  - `BattleManager`：回合状态机和战斗流程
  - `CardSystemComponent`：牌堆、手牌、弃牌、出牌
  - `RogueTypes`：战斗枚举与数据结构

## 3. 战斗状态机
`BattleStart -> PlayerTurnStart -> PlayerMain -> PlayerTurnEnd -> EnemyTurnStart -> EnemyMain -> EnemyTurnEnd -> (循环) -> BattleEnd`

## 4. 数据驱动策略（当前阶段）
- 卡牌、敌人、遗物、事件均由 `DataTemplates` CSV 驱动
- 运行时暂用 `FRuntimeCard` 记录卡牌动态属性（升级/临时费用）
- 下一阶段接入 `DataTable` 与 `EffectScript` 分发表

## 5. 存档设计（单机）
- `MetaSave`：局外解锁、设置、统计
- `RunSave`：单局进度、地图节点、战斗快照
- 版本字段：`save_version`，用于后续兼容迁移

## 6. 编程约定
- 命名：`AR*`（Actor）、`UR*`（UObject/Component）、`ER*`（Enum）
- 逻辑优先写 C++，展示层和流程编排用 Blueprint
- 所有关键流程保留日志钩子（便于调试和回放）

## 7. 近期实施清单（本次已覆盖）
1. 初始化 UE5 C++ 项目骨架（`.uproject` + `Source` + `Config`）
2. 将 `StarterCode` 升级为实际模块代码（`ANIMEROGUE_API`）
3. 提供可生成工程文件与打开编辑器的操作指引

## 8. 下一步（你确认后我继续）
1. 新增 `RunManager` 和战斗奖励三选一卡流程
2. 接入 `DataTable` 读取真实卡牌费用与效果ID
3. 增加最小 `WBP_BattleHUD` 字段绑定规范文档
