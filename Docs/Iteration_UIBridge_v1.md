# 当前迭代说明：UI Bridge v1（Blueprint Function Library）

## 新增
- `URogueUIBridgeLibrary`
  - `BuildUIRuntimeSnapshot(...)`
  - `ToNodeTypeText(...)`
  - `ToFlowStateText(...)`
- UI聚合结构：
  - `FUIRuntimeSnapshot`
  - `FUIRunSnapshot`
  - `FUIBattleSnapshot`
  - `FUIEventSnapshot`
  - `FUIShopSnapshot`

## 目标
- UMG 不再到处拉 manager 接口
- 每帧或定时调用一次 `BuildUIRuntimeSnapshot`，一次拿到全部界面数据

## UMG最短接法
1. 在 Widget Blueprint 新建变量：
   - `NodeFlowManagerRef`
   - `RunManagerRef`
   - `BattleManagerRef`
   - `EventManagerRef`
   - `ShopManagerRef`
2. Tick 或定时器中调用：
   - `BuildUIRuntimeSnapshot(...)` -> 存到本地 `Snapshot`
3. 各面板读取：
   - 顶栏：`Snapshot.Run.Gold`, `Snapshot.Run.CurrentHP/MaxHP`
   - 战斗：`Snapshot.Battle.HandCards`, `Snapshot.Battle.Energy`, `Snapshot.Battle.EnemyIntent`
   - 事件：`Snapshot.Event.Title`, `Snapshot.Event.Options`
   - 商店：`Snapshot.Shop.CardOffers`
   - 状态：`ToFlowStateText(Snapshot.FlowState)`
