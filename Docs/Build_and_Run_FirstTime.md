# 首次编译运行（AnimeRogue）

## 1. 生成工程文件
在资源管理器中右键：
- `D:\Unrealgamedemo\GameProject\AnimeRogue\AnimeRogue.uproject`
- 选择 `Generate Visual Studio project files`

## 2. 打开并编译
- 双击生成的 `AnimeRogue.sln`
- 选择 `Development Editor` + `Win64`
- 执行 `Build Solution`

## 3. 启动编辑器
- 直接双击 `AnimeRogue.uproject`
- 首次会自动编译模块，等待完成

## 4. 验证战斗骨架
1. 新建关卡，拖入 `BattleManager` Actor
2. 在关卡蓝图或按钮事件里调用 `StartBattle`
3. 调用 `TryPlayCardFromHand` 和 `EndPlayerTurn`
4. 检查能量与手牌是否循环变化

## 5. 常见错误
- 缺少 C++ 工具链：回到 Visual Studio Installer 勾选 `使用 C++ 的游戏开发`
- 打不开 `.uproject`：先右键生成工程文件再打开
