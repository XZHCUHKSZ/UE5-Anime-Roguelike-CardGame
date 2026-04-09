# 当前迭代说明：RunManager 事件与商店

## 新增可调用接口
- `StartRun(seed)`
- `AdvanceToNextNode()`
- `GainGold(amount)` / `SpendGold(amount)`
- `HealPlayer(amount)` / `DamagePlayer(amount)`
- `BuyCard(cardId, price)`
- `BuyRandomRelic(price)`
- `RemoveCardAtShop(deckIndex)`
- `ExecuteEventScript(eventScriptId)`

## 蓝图里最常用读取
- `GetGold()`
- `GetCurrentHP()` / `GetMaxHP()`
- `GetDeck()`
- `GetRelics()`
- `GetRunLog()`
- `GetCurrentNode()`

## 节点流程建议
1. 进入 Event 节点：
   - 展示三选项
   - 根据选择调用 `ExecuteEventScript`
2. 进入 Shop 节点：
   - 买卡 -> `BuyCard`
   - 买遗物 -> `BuyRandomRelic`
   - 删卡 -> `RemoveCardAtShop`
3. 完成节点后统一调用 `AdvanceToNextNode`
