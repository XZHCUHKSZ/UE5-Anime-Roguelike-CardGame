# 当前迭代说明：自动存档 + 继续游戏 v1

## 新增能力
- `SaveManager`：
  - `HasRunSave()`
  - `ClearRunSave()`
- `NodeFlowManager`：
  - `StartFlowFromSave()`
  - 自动存档开关 `bEnableAutoSave`
  - 节点进入与节点结算后自动触发 `SaveRun`

## 主菜单接法（Continue按钮）
1. 获取 `NodeFlowManager` 与 `SaveManager` 引用
2. `HasRunSave()` 为 true 才显示 Continue 按钮
3. 点击 Continue 调 `NodeFlowManager.StartFlowFromSave()`

## 自动存档触发点
- 新开局后
- 进入新节点后
- 战斗领奖并推进后
- 事件结算并推进后
- 商店离开并推进后

## 完局处理
- 失败/胜利时调用 `ClearRunSave()`，避免继续按钮加载已结束局

## 当前边界（v1）
- 读档后会根据当前节点重新进入流程
  - Event 节点会重新抽取事件
  - Shop 节点会重新刷新商品
- 若需“严格还原节点内状态”，下个版本补 `Event/Shop` 的独立快照存档
