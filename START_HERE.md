# START HERE（立即开工）

## 1. 先建UE5 C++工程
- 创建一个UE5 C++项目（例如项目名：`AnimeRogue`）
- 在 `Source/<模块名>/` 下新建对应文件

## 2. 复制模板代码
- 将 `StarterCode` 下的 `.h/.cpp` 复制到工程 `Source/<模块名>/`
- 把所有 `YOURMODULE_API` 替换为你的模块导出宏（如 `ANIMEROGUE_API`）

## 3. 导入数据表模板
- 在UE中创建对应 `UStruct`（先用 `FCardData`）
- 创建 `DataTable` 并导入：
  - `DataTemplates/DT_Cards_Template.csv`
  - `DataTemplates/DT_Enemies_Template.csv`
  - `DataTemplates/DT_Relics_Template.csv`
  - `DataTemplates/DT_Events_Template.csv`

## 4. 第一版可玩目标（今天）
- 场景中放置 `ABattleManager`
- 给它喂一个10张测试卡组
- UI放一个“End Turn”按钮调用 `EndPlayerTurn`
- 打通“抽牌 -> 出牌 -> 回合结束 -> 敌方回合占位 -> 下回合”

## 5. 本地存档落地
- 参考 `Docs/SaveGame_Schema.json` 建两个 `USaveGame` 类：
  - `UMetaSaveGame`
  - `URunSaveGame`
