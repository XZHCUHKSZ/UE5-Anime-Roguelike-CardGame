# 当前迭代说明：严格节点状态还原 v1

## 目标
- 读档后不再“事件重抽、商店重刷”
- 事件与商店进入时优先恢复存档中的节点内状态

## 本次实现
1. `FRunSaveData` 新增：
   - `bHasPendingEvent`, `PendingEvent`
   - `bHasPendingShop`, `PendingShopOffers`
2. `RunManager` 新增：
   - `Set/Clear/Get PendingEvent`
   - `Set/Clear/Get PendingShopOffers`
   - Build/Apply 存档时同步 pending 状态
3. `EventManager` 新增：
   - `SetCurrentEvent`
   - `ClearCurrentEvent`
4. `ShopManager` 新增：
   - `SetCurrentOffers`
   - `ClearCurrentOffers`
5. `NodeFlowManager` 调整：
   - 进入 Event 节点：
     - 有 pending -> 直接恢复
     - 无 pending -> 生成并写入 pending
   - 进入 Shop 节点：
     - 有 pending -> 直接恢复
     - 无 pending -> 生成并写入 pending
   - 事件结算后清空 pending event
   - 离开商店后清空 pending shop

## 结果
- Continue 读档时，事件与商店内容保持一致，不会变化
