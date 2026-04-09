# UE5 从零到开工指南（Windows）

## 目标
- 把这台机器配置成可开发 UE5 单机卡牌肉鸽
- 让仓库 `UE5-Anime-Roguelike-CardGame` 可以直接接入 UE5 C++ 工程

## A. 自动脚本（建议先跑）
在仓库根目录执行：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\setup_ue5_env.ps1 -InstallEpicLauncher -InstallVsGameWorkload
```

如果你不想自动安装，只做检测：

```powershell
powershell -ExecutionPolicy Bypass -File .\scripts\setup_ue5_env.ps1
```

## B. 必装组件清单
1. Epic Games Launcher
2. Unreal Engine 5（建议当前最新稳定 UE5）
3. Visual Studio 2022 + `Game development with C++`
4. Windows 10/11 SDK（通常在 VS 工作负载中一起安装）

## C. 在 Launcher 安装 UE5
1. 打开 Epic Launcher，登录账号
2. 进入 `Unreal Engine -> Library`
3. 安装 UE5（建议安装到剩余空间大的盘）
4. 安装选项中建议勾选：
   - Editor symbols（调试更友好）
   - C++ source（需要看引擎源码时有用）

## D. 创建首个工程并接入仓库
1. 启动 UE5，创建 `Games -> Blank -> C++`
2. 项目名可用：`AnimeRogue`
3. 路径选择：`D:\Unrealgamedemo\GameProject\AnimeRogue`
4. 创建完成后，把本仓库中的以下内容复制/迁移进工程：
   - `StarterCode/*` -> `Source/<YourModule>/`
   - `DataTemplates/*` -> 导入到 `Content/Data`
   - `Docs/*` -> 保留在仓库根目录

## E. 首日可运行目标
1. 编译成功并进入编辑器
2. 建立 `ABattleManager` 测试场景
3. 手动喂10张测试卡
4. 按 `End Turn` 能进入敌方回合再回到我方回合

## F. 常见问题
1. 编译报找不到 `YOURMODULE_API`
   - 把模板里的 `YOURMODULE_API` 改成你的模块宏（如 `ANIMEROGUE_API`）
2. 找不到编译器或SDK
   - 在 VS Installer 里确认 `Game development with C++` 已勾选
3. UE 打不开 C++ 项目
   - 右键 `.uproject` -> `Generate Visual Studio project files` 后重试
