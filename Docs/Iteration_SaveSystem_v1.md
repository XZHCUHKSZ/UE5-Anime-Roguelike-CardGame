# 当前迭代说明：存档系统 v1（USaveGame）

## 新增类
- `URogueRunSaveGame`
- `URogueMetaSaveGame`
- `ASaveManager`

## RunManager 新增
- `BuildRunSaveData()`
- `ApplyRunSaveData()`

## 蓝图接线（最短）
1. 关卡放置：
   - `RunManager`
   - `SaveManager`
2. 开局：
   - `RunManager.StartRun(seed)`
3. 手动存档按钮：
   - `SaveManager.SaveRun(RunManager)`
4. 读档按钮：
   - `SaveManager.LoadRun(RunManager)`
5. Meta 存档：
   - `SaveManager.SaveMeta(metaDataStruct)`
   - `SaveManager.LoadMeta(outMetaDataStruct)`

## 当前能力
- 可保存/恢复单局关键状态：
  - seed、HP、金币、当前楼层节点、卡组、遗物
- 可保存/读取局外元进度结构（Meta）
