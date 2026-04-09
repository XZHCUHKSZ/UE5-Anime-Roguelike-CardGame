# 当前迭代说明：EventManager + ShopManager

## 新增类
- `AEventManager`
  - `RollRandomEvent()`
  - `GetCurrentEvent()`
  - `ChooseOption(index, RunManager)`
- `AShopManager`
  - `GenerateShop(RunManager)`
  - `GetCurrentOffers()`
  - `BuyCardOffer(index, RunManager)`
  - `BuyRelic(RunManager)`
  - `RemoveCardService(deckIndex, RunManager)`

## 相关数据结构
- `FEventDataRow`
- `FEventOptionData`
- `FResolvedEventData`
- `FShopCardOffer`

## UE 蓝图最短接法
1. 关卡里放 3 个 Actor：
   - `RunManager`
   - `EventManager`
   - `ShopManager`
2. BeginPlay：
   - 调 `RunManager.StartRun(seed)`
3. 进入 Event 节点时：
   - 调 `EventManager.RollRandomEvent()`
   - 从 `GetCurrentEvent()` 读取标题和三个选项文本
   - 玩家点击后调用 `ChooseOption(OptionIndex, RunManager)`
4. 进入 Shop 节点时：
   - 调 `ShopManager.GenerateShop(RunManager)`
   - 用 `GetCurrentOffers()` 展示商品
   - 点击购买调用 `BuyCardOffer`
   - 买遗物调用 `BuyRelic`
   - 删卡调用 `RemoveCardService`

## 现状说明
- 已可支持“事件三选一”和“商店买卖”的完整逻辑闭环
- UI仅需绑定这两个 Manager 的读取接口即可
