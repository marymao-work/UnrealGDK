# Changelog
All notable changes to the SpatialOS Game Development Kit for Unreal will be documented in this file.

The format of this Changelog is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

**Note**: Since GDK for Unreal v0.10.0, the changelog is published in both English and Chinese. The Chinese version of each changelog is shown after its English version.<br>
**注意**：自虚幻引擎开发套件 v0.10.0 版本起，其日志提供中英文两个版本。每个日志的中文版本都置于英文版本之后。

## [`x.y.z`] - Unreleased
### Breaking changes:
- Removed support for UE 4.24.
- `MaxRPCRingBufferSize` setting has been removed. This was previously used to specify the RPC ring buffer size when generating schema. Now, `DefaultRPCRingBufferSize` is used, and can be overridden per RPC type using `RPCRingBufferSizeOverrides`.
- `RPCRingBufferSizeMap` setting has been renamed to `RPCRingBufferSizeOverrides`.

### Features:
- Added a message box notification when game is closed due to missing generated schema.
- Adapted SpatialDebugger to use SubViews.
- Added a function flag `AlwaysWrite` that allows specifying an RPC to use a separate channel and allow overwriting unacked RPC calls. This is currently limited to Unreliable Server RPCs on classes inheriting from AActor, and only one such RPC can be specified per actor. This feature is disabled by default and can be enabled via `bEnableAlwaysWriteRPCs` setting.
- Enhanced server logging to include load balancing and local worker info on startup.
- Added 'Persistent' spatial class flag, typically used to override a non persistent base class.
- Add a function that allows the worker coordinator to periodically restart the simulated player clients with a bunch of parameters. This feature is disabled by default and can be enabled via `max_lifetime` setting.

### Bug fixes:
- Fixed the exception that was thrown when adding and removing components in Spatial component callbacks.
- Fixed incorrect allocation of entity ID from a non-authoritative server sending a cross-server RPC to a replicated level actor that hasn't been received from runtime.
- Fixed a regression where bReplicates would not be handed over correctly when dynamically set.
- Fixed an issue where resetting handover property to default value would be omitted during handover value replication
- Fixed EntityPool capacity overflow issue by removing the ability from the gdk settings to request a pool size larger than int32_max.
- Fixed an issue where components added to a scene actor would be replicated incorrectly.

## [`0.12.0`] - 2021-02-01

### Breaking changes:
- We no longer support Server Travel. This feature is being re-designed and will be reintroduced in a future version.
- The condition for sending Spatial position updates has been changed, the two variables `PositionUpdateFrequency` and `PositionDistanceThreshold` have now been removed from the GDK settings. To update your project:
  1. Set the value of `PositionUpdateLowerThresholdCentimeters` to the value of `PositionDistanceThreshold` and  the value of `PositionUpdateLowerThresholdSeconds` to 60*(1/`PositionUpdateFrequency`). This makes sure that Actors send Spatial position updates as often as they did before this change.
  2. Set the value of `PositionUpdateThresholdMaxCentimeters` and `PositionUpdateThresholdMaxSeconds` to larger values than the lower thresholds.
  NOTE: If your project does not use custom values for the `PositionUpdateFrequency` or `PositionDistanceThreshold` then, by default, the updates will be sent with the same frequency as before and no action is required.
- Removed the `OnAuthorityLossImminent` Actor event.
- 'WorkerLogLevel' in Runtime Settings is split into two new settings - 'LocalWorkerLogLevel' and 'CloudWorkerLogLevel'. Update these values which will be set to 'Warning' by default.
- The Unreal GDK has been updated to run against SpatialOS 15.0. Older version of SpatialOS will no longer work with the Unreal GDK.
- In SpatialWorkerFlags.h some renaming took place around using delegates on flag changes.
These functions and structs can be referenced in both code and blueprints and it may require updating both:
  1. Delegate struct `FOnWorkerFlagsUpdatedBP` has been renamed `FOnAnyWorkerFlagUpdatedBP`,
  2. Delegate struct `FOnWorkerFlagsUpdated` has been renamed `FOnAnyWorkerFlagUpdated`,
  3. Function `BindToOnWorkerFlagsUpdated` has been renamed to `RegisterAnyFlagUpdatedCallback`
  4. Function `UnbindFromOnWorkerFlagsUpdated` has been renamed to `UnregisterAnyFlagUpdatedCallback`
- Worker configurations must now be stored in the launch config for local deployments. These can be added in the SpatialGDKEditorSettings as before.
- Spot and SpatialD (Spatial Service) dependencies have been removed.
- Compatibility Mode runtime is no longer supported.
- Running without Ring Buffered RPCs is no longer supported, and the option has been removed from SpatialGDKSettings.
- The schema database format has been updated and versioning introduced. Please regenerate your schema after updating.
- The CookAndGenerateSchemaCommandlet no longer automatically deletes previously generated schema. Deletion of previously generated schema is now controlled by the `-DeleteExistingGeneratedSchema` flag.

### Features:
- The DeploymentLauncher tool can be used to start multiple simulated player deployments at once.
- Schema bundles are generated by default (in addition to the schema descriptor). Specifying a Schema bundle location via `-AdditionalSchemaCompilerArguments="--bundle_out=..."` will result in the bundle being written to both the default location (`build/assembly/schema/schema.sb`) _and_ the location specified by the additional arguments.
- Spatial Position Updates are sent based on a different logic. Previously, a position update was sent to Spatial if enough time had passed since the Actor's last update(computed using the SpatialUpdateFrequency) AND if the Actor had moved more than `PositionDistanceThreshold` centimeters. This change allows for Spatial position updates to be sent if any of the following is true:
  1. The time elapsed since the last sent Spatial position update is greater than or equal to `PositionUpdateLowerThresholdSeconds` AND the distance travelled since the last update is greater than or equal to `PositionUpdateLowerThresholdCentimeters`.
  1. The time elapsed since the last sent Spatial position update is greater than or equal to `PositionUpdateThresholdMaxSeconds` AND the Actor has moved a non-zero amount.
  1. The distance travelled since the last Spatial position update was sent is greater than or equal to `PositionUpdateThresholdMaxCentimeters`.
- New setting "Auto-stop local SpatialOS deployment" allows you to specify Never (doesn't automatically stop), OnEndPIE (when a PIE session is ended) and OnExitEditor (only when the editor is shutdown). The default is OnExitEditor.
- Added `OnActorReady` bindable callback triggered when SpatialOS entity data is first received after creating an entity corresponding to an Actor. This event indicates you can safely refer to the entity without risk of inconsistent state after worker crashes or snapshot restarts. The callback contains the active actor's authority.
- Added support for the main build target having `TargetType.Client` (`<ProjectName>.Target.cs`). This target is automatically built with arguments `-client -noserver` passed to UAT when building from the editor. If you use the GDK build script or executable manually, you need to pass `-client -noserver` when building this target (for example, `BuildWorker.bat GDKShooter Win64 Development GDKShooter.uproject -client -noserver`).
- Added ability to specify `USpatialMultiWorkerSettings` class from command line. Specify a `SoftClassPath` via `-OverrideMultiWorkerSettingsClass=MultiWorkerSettingsClassName`.
- You can override the load balancing strategy in-editor so that it is different from the cloud. Set `Editor Multi Worker Settings Class` in the `World Settings` to specify the in-editor load balancing strategy. If it is not specified, the existing `Multi Worker Settings Class` defines both the local and cloud load balancing strategy.
- Added `BroadcastNetworkFailure` with type `OutdatedClient` on client schema hash mismatch with server. Add your own callback to `GEngine->NetworkFailureEvent` to add custom behaviour for outdated clients attempting to join.
- You can see the Spatial Debugger in-editor mode similar to the one you see in play mode. Select `Spatial Editor Debugger` from the drop-down menu on the `Start Deployment` button on the toolbar to toggle the visibility of the worker boundaries on and off in-editor.
- Enabled the `bUseSpatialView` property by default and added the `--OverrideUseSpatialView` flag.
- Added facilities to manipulate worker interest and Actor authority in SpatialFunctionalTest.
- Allow specifying the locator port via `?locatorPort=` URL option when performing client travel.
- You can now enable/disable the multi-worker load balancing strategy with an in-editor toggle so that no Uasset files are changed. Select `Enable Multi-Worker` from the drop-down menu on the `Start Deployment` button on the toolbar to use the multi-worker strategy or de-select to use a single worker strategy in the editor. The `Enable Multi-Worker` toggle in World Settings and the command line option  `-OverrideMultiWorker` have been removed as they are now redundant.
- Enabled packaging the command line arguments when building a mobile client by default.
- Added settings for the positioning and opacity of the Spatial Debugger worker region visualisation.
- You can now configure what the Spatial Debugger visualises in an in-game menu. Use F9 (by default) to open and close it. The key can be changed through a setting on the Spatial Debugger object.
- Added a setting for the Spatial Debugger to visualise all replicated actors in the local player's hierarchy, instead of just the player's controller, player state and pawn.
- You can now see worker information displayed on the worker's boundaries. The worker name and virtual worker ID displayed corresponds to the worker that you are currently looking at and will be visible when you are near a border.
- You can now filter logs for Local and Cloud deployments separately with Editor settings. The 'WorkerLogLevel' GDK setting was removed and has been replaced by 'LocalWorkerLogLevel' and 'CloudWorkerLogLevel'.
- You can now disable logging to SpatialOS for local and/or cloud deployments from the GUI (Project Settings -> Runtime Settings -> Logging). The command line argument -NoLogToSpatial can still be used for that as well.
- Servers now log a warning message when detecting a client has timed out.
- Handover is now optional depending on whether the load balancing strategy implementations require it . See `RequiresHandoverData`
- Improved the failed hierarchy migration logs. The logs are now more specific, the frequency of repeated logs is suppressed and cross-server migration diagnostic logs have been added.
- You can now select an actor for SpatialOS debugging in-game. Use F9 (by default) to open the Spatial Debugger in-game config menu and then press the `Start Select Actor(s)` button. Hover over an actor with the mouse to highlight and right-click  (by default) to select. You can select multiple actors. To deselect an actor right-click on it a second time. If there are multiple actors under the cursor use the mouse wheel (by default) to highlight the desired actor then right-click to confirm your selection.
- SpatialWorldSettings is now the default world settings in supported engine versions.
- Worker SDK version compatibility is checked at compile time. 
- SpatialWorkerFlags has reworked how to add callbacks for flag updates:
  1. `BindToOnWorkerFlagsUpdated` is changed to `RegisterAnyFlagUpdatedCallback` to better differentiate it from the newly added functions for register callbacks. 
  2. `RegisterFlagUpdatedCallback` is added to register callbacks for individual flag updates
  3. `RegisterAndInvokeAnyFlagUpdatedCallback` & `RegisterAndInvokeFlagUpdatedCallback` variants are added that will invoke the callback if the flag was previously set.
- Overhauled local deployment workflows. Significantly faster local deployment start times (0.1s~ vs 7s~).
  - Switched to using binary (standalone) versions of the Runtime and Inspector which allow us to start deployments much faster.
  - Runtime and Inspector binary fetching is now handled by the GDK. To fetch these binaries you must start the game project at least once.
  - The Runtime and Inspector will be restarted between each Editor session.
  - Support for standalone launch configurations introduced, these are now generated by default for your local deployments, worker configurations for local deployments are now stored inside the launch config. 
    - Generated local launch configs can still be found in `<GameProject>\Game\Intermediate\Improbable\<MapName>_LocalLaunchConfig.json`.
  - Cloud deployments still use the older launch config and worker config format, we still generate these automatically for your maps.
    - Generated cloud launch configs can  be found in `<GameProject>\Game\Intermediate\Improbable\<MapName>_CloudLaunchConfig.json`.
  - The Launch Configuration Generator tool can be used to generate either local or cloud launch configs via a checkbox in the GUI.
  - Compatibility Mode Runtime is no longer supported.
  - Local deployment logs timestamp format has changed to `yyyy.mm.dd-hh.mm.ss`. Example log path: `<GameProject>\spatial\logs\localdeployment\2020.12.02-14.13.39`
  - Launch.log no longer exists in the localdeployment logs, the equivalent logs can be found in the runtime.log
  - Local deployments are now restarted by default when using PIE. We recommend this setting for all your projects. 
  - Snapshots taken of running local deployments are saved in `<GameProject>\spatial\snapshots\<yyyy.mm.dd-hh.mm.ss>\snapshot_<n>.snapshot`
  - Inspector URL is now http://localhost:33333/inspector-v2
  - Inspector version can be overridden in the SpatialGDKEditorSettings under `Inspector Version Override`
- The SpatialNetDriver can disconnect a client worker when given the system entity ID for that client and does so when `GameMode::PreLogin` returns with a non-empty error message.
- Unreal Engine version 4.26.0 is supported! Refer to https://documentation.improbable.io/gdk-for-unreal/docs/keep-your-gdk-up-to-date for versioning information and how to upgrade.
- Running with an out-of-date schema database reports a version warning when attempting to launch in editor.
- Reworked schema generation (incremental and full) pop-ups to be clearer. 
- Unreal Engine version 4.26.0 is now supported! Refer to https://documentation.improbable.io/gdk-for-unreal/docs/keep-your-gdk-up-to-date for versioning information and how to upgrade.
- Added cross-server variants of ability activation functions on the Ability System Component.
- Added `SpatialSwitchHasAuthority` function to differentiate authoritative server, non-authoritative server, and clients. This can be called in code or used in blueprints that derive from actor.
- Added blueprint callable function `GetMaxDynamicallyAttachedSubobjectsPerClass` to `USpatialStatics` that gets the maximum dynamically attached subobjects per class as set in `SpatialGDKSettings`
- Running with an out-of-date schema database will now report a version warning when attempting to launch in editor.
- Simulated Player deployments no longer depend on DeploymentLauncher for readiness. You can now restart them via the Console and expect them to reconnect to your main deployment. DeploymentLauncher will also restart any crashed or incorrectly finished simulated players applications.
- Added a `-FailOnNetworkFailure` flag that makes a Spatial-enabled game fail on any NetworkFailure.
- Reworked schema generation (incremental + full) pop-ups to be clearer.
- Added a `-FailOnNetworkFailure` flag that makes a Spatial-enabled game fail on any NetworkFailure
- Added `URemotePossessionComponent` to deal with Cross-Server Possession. Add this componenet to an AController, it will possess the Target Pawn after OnAuthorityGained. It can be implemented in C++ and Blueprint.

### Bug fixes:
- Fixed a bug that stopped the travel URL being used for initial Spatial connection if the command line arguments could not be used.
- Added the `Handover` tag to `APlayerController::LastSpectatorSyncLocation` and `APlayerController::LastSpectatorSyncRotation` in order to fix a character spawning issue for players starting in the `Spectating` state when using zoning.
- No longer AddOwnerInterestToServer unless the owner is replicating, otherwise this warning fires erroneously: "Interest for Actor <ActorName> is out of date because owner <OwnerName> does not have an entity id."
- Properly handle pairs of Add/Remove component in critical section. The issue manifested in the form of remnant actors which the worker should have lost interest in.
- Made the DeploymentLauncher stop multiple deployments in parallel to improve performance when working with large numbers of deployments.
- Fixed an issue where possessing a new pawn and immediately setting the owner of the old pawn to the controller could cause server RPCs from that pawn to be dropped.
- Added support for the `bHidden` relevancy flag. Clients will not checkout Actors that have `bHidden` set to true (unless they are always relevant or the root component has collisions enabled).
- Fixed an issue with deployments failing due to the incorrect number of workers when the launch config was specified, rather than automatically generated.
- Fixed the `too many dynamic subobjects` error on Clients appearing when a Startup Actor, with one dynamic subobject was leaving and re-entering interest multiple times. Added the `RemovedDynamicSubobjectObjectRefs` map in `USpatialPackageMapClient` that keeps the dynamic subobjects removed from Startup Actor's client's interest to avoid duplication of the dynamic subojects when the Startup Actor re-enters the Client's interest.
- Fixed an issue that prevented the Interest component from being initialized properly when provided with `Worker_ComponentData`.
- Cleaned up startup logs of a few noisy messages.
- Fixed a crash that sometimes occurred upon trying to resolve a pointer to an object that has been unloaded.
- Fixed a crash when spawn requests are forwarded but the `APlayerStart` actor is not resolvable on the target worker.
- By default, only an Actor's replicated owner hierarchy is used when determining which worker should have authority over an actor. Non-replicated Actors are now ignored.
- Fixed a crash that would sometimes occur when connection to SpatialOS fails.
- Fixed a crash that occurred when an actor subobject became invalid after applying initial component data.
- Non-replicated Actors net roles are not touched during startup.
- Fixed a bug which dropped component updates on authority delegation.
- The DeploymentLauncher checks the validity of the simulated players deployment name.
- Worker configuration watcher only rebuilds worker configs when `*.worker.json` files are changed.
- Added support for FPredictionKey's conditional replication logic. GameplayCues now activate on all clients, instead of only the client that initiated them.
- Fixed a bug where deployment would fail in the presence of trailing spaces in the `Flags` and `LegacyFlags` fields of the `SpatialGDKEditorSettings`.
- Fixed a crash that would occur when performing multiple Client Travels at once.
- Fixed an issue where bReplicates would not be handed over correctly when dynamically set.
- Add ServerOnlyAlwaysRelevant component and component set schema definitions
- Fixed a snapshot reloading issue where worker would create extra actors, as if they were loading on a fresh deployment.
- Server workers use TCP (instead of KCP) by default.
- Fixed a rare crash where a RepNotify callback can modify a GDK data structure being iterated upon.
- Fixed race condition in Spatial Test framework that would cause tests to time out with one or more workers not ready to begin the test.
- Fixed client connection not being cleaned up when moving out of interest of a server.
- Fixed an assertion being triggered on async loaded entities due to queuing some component addition.
- Fixed a bug where consecutive invocations of CookAndGenerateSchemaCommandlet for different levels could fail when running the schema compiler.
- Fixed an issue where GameMode values won't be replicated between server workers if it's outside their Interest.
- Fixed gameplay cues receiving OnActive/WhileActive events twice on the predicting client in a multi-worker single-process PIE environment.
- Fixed an issue where a NetworkFailure won't be reported when connecting to a deployment that doesn't support dev_login with a developer token, and in some other configuration-dependent cases.
- Fixed a crash that occured when opening the session frontend with VS 16.8.0 using the bundled dbghelp.dll.
- Spatial Debugger no longer consumes input.
- Fixed an issue where we would always create a folder for a snapshots for a deployment even when we made no snapshots
- Fixed an issue in the SpatialTestCharacterMigration test where trigger boxes sometimes wouldn't trigger at low framerates.
- Spatial bundles no longer requested at startup if `UGeneralProjectSettings::bSpatialNetworking` is disabled.
- Fixed an issue where heartbeats could be ran on a controller after its destruction.
- Fixed an issue that led to the launch config being left in non-classic style with certain engine and project path configurations.

## [`0.11.0`] - 2020-09-03

### Breaking changes:
- We no longer support Unreal Engine version 4.23. We recommend that you upgrade to the newest version 4.25 to continue receiving updates. See [Unreal Engine Version Support](https://documentation.improbable.io/gdk-for-unreal/docs/versioning-scheme#section-unreal-engine-version-support) for more information on versions. Follow the instructions in [Update your GDK](https://documentation.improbable.io/gdk-for-unreal/docs/keep-your-gdk-up-to-date) to update to 4.25.
- We have removed multi-worker settings from the `SpatialWorldSettings` properties and added them to a new class `USpatialMultiWorkerSettings`. To update your project, create a derived `USpatialMultiWorkerSettings` class mimicking your previous configuration. Then, in your level’s World settings, select that class as the `Multi-worker settings class` property.
- The `-nocompile` flag used with `Buildworker.bat` is now split into two. Use the following flags:
  - `-nobuild` to skip building the game binaries.
  - `-nocompile` to skip compiling the automation scripts.
- Updated the simulated player worker configuration. Instead of using `connect.to.spatialos` to indicate that you want to connect to a cloud deployment, we now use `127.0.0.1` to ensure that an address resolves when a connection initializes. The passed IP address is not used when connecting to a cloud deployment.
- The `SpatialMetrics::WorkerMetricsUpdated` function is no longer static and the function signature now receives histogram metrics.

### New known issues:
- The `Use RPC Ring Buffers` option in **SpatialOS GDK for Unreal** > **Runtime Settings** is required when using multi-worker configurations, but is not currently enforced.

### Features:
- You can now use the new GDK editor setting `Stop local deployment on stop play in editor` to automatically stop a deployment when you stop playing in the Editor.
- Added a checkbox `Connect local server worker to the cloud deployment` to the SpatialOS **Editor Settings**. This controls whether you start and connect a local server-worker instance to the cloud deployment, when `Connect to cloud deployment` is enabled.
- You can now suppress RPC warnings of the form `Executed RPC <RPCName> with unresolved references`, by RPC type. To do this, use the new `SpatialGDKsetting RPCTypeAllowUnresolvedParamMap`.
- Decoupled `QueuedIncomingRPCWaitTime` from reprocessing flush time by adding a new parameter `QueuedIncomingRPCRetryTime` (default value 1.0s). This enables you to independently control how long to wait for queued RPCs to resolve parameters, and how frequently to check whether parameters are resolved.
- Command-line arguments are now only available by default in non-shipping builds. To enable them in a shipping build, use the target rule `bEnableSpatialCmdlineInShipping`.
- Dynamic worker flags are now supported with the Standard Runtime variant.
- Dynamic worker flags now enable faster startup for simulated player deployments started with the `DeploymentLauncher`. `DeploymentLauncher createsim` now includes a boolean argument `auto-connect`. This enables you to automatically connect your sim players to your deployment when it is ready.

### Bug fixes:
- The example worker configuration for the simulated player coordinator is now compatible with the authentication flow.
- The `Cloud Deployment Name` field in the `Start Deployment` drop-down menu and the `Deployment name` in the `Cloud Deployment Configuration` window now refer to the same property. The `Start Deployment` toolbar button now uses the name specified in the drop-down menu.
- The `Local Deployment IP` and `Cloud Deployment Name` labels in the `Start Deployment` drop-down menu are now grayed out correctly when the edit box is disabled.
- Entering an invalid IP into the `Exposed local runtime IP address` field in the **Editor Settings** now triggers a warning pop-up and resets the value to an empty string.
- Fixed a bug that caused this error: `ResolveObjectReferences: Removed unresolved reference: AbsOffset >= MaxAbsOffset`.
- When `GridBasedLBStrategy` can't locate a worker instance to take authority over an Actor, it now logs an error that includes `Position`.
- The `SpatialGDKsetting bEnableMultiWorker` is now private, to enforce the use of `IsMultiWorkerEnabled` which respects the `-OverrideMultiWorker` flag.
- When the `SpatialStatics::GetActorEntityId` function is passed a `nullptr`, it now returns `SpatialConstants::INVALID_ENTITY_ID`.
- Removed the `EditorWorkerController` class. It is no longer required for running consecutive PIE sessions.
- Fixed a crash that occurred when overflowed RPCs remained overflowed after a flush attempt.
- Fixed a crash that sometimes occurred after performing server travel with ring buffers enabled.

### [`0.11.0`] 中文版更新日志

### 重大变化：
- 我们不再维护虚幻引擎 4.23 版本。建议您升级至最新版本 4.25 以继续接收更新，按照 [更新 GDK](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/keep-your-gdk-up-to-date) 的操作步骤即可更新至 4.25 版本。注：更多版本相关信息，请参见[虚幻引擎版本控制方案](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/versioning-scheme#section-unreal-engine-version-support)。
- 多 worker 设置已从 `SpatialWorldSettings` 属性中删除，并添加至一个新的类 `USpatialMultiWorkerSettings`。为更新项目，请您创建一个派生的 `USpatialMultiWorkerSettings` 类，以模仿您之前的配置。然后在关卡的 World Settings 中，选择该类作为 `Multi-worker settings class` 属性。
- `Buildworker.bat` 所使用的 `-nocompile` 标记现已拆分为二，分别如下：
  - `-nobuild` 用于跳过构建游戏二进制文件。
  - `-nocompile` 用于跳过编译自动化脚本。
- 模拟玩家 worker 配置已更新。我们现在使用 `127.0.0.1` 来确保在连接初始化时解析地址，而不是使用 `connect.to.spatialos` 来表示您想要连接到云部署。连接到云部署时，不会使用已传递的 IP 地址。
- `SpatialMetrics::WorkerMetricsUpdated` 函数不再为静态，并且函数签名现在接收直方图指标。

### 已知问题：
- 使用多 worker 配置时，要求选择 `Use RPC Ring Buffers` (位于：**SpatialOS 虚幻引擎 GDK > Runtime Settings**)，但目前没有强制执行。

### 功能介绍：
- 现在您可以使用 GDK 编辑器的新设置 `Stop local deployment on stop play in editor`，在您停止“在编辑器中运行”(PIE) 时自动停止部署。
- 在 SpatialOS **Editor Settings** 中添加了 `Connect local server worker to the cloud deployment` 复选框。在启用 `Connect to cloud deployment` 后，这将用于控制您是否启动并将本地服务端 worker 实例连接至云部署。
- 现在您可以按照 RPC 类型，来抑制 `Executed RPC <RPCName> with unresolved references` 形式的 RPC 警告。请使用新的 `SpatialGDKsetting RPCTypeAllowUnresolvedParamMap` 进行操作。
- 通过添加新的参数 `QueuedIncomingRPCRetryTime` (默认值为 1.0s)，您可以独立控制排队 RPC 解析参数的等待时间 (`QueuedIncomingRPCWaitTime`)，以及检查参数是否已经解析的频率 (`QueuedIncomingRPCRetryTime`)。
- 命令行参数现仅在非交付构建版本中默认可用。为在交付构建版本中启用这些参数，请使用目标规则 `bEnableSpatialCmdlineInShipping`。
- 标准体 (Standard) 运行时变体现支持动态 worker 标记。
- 动态 worker 标记现支持更快地启动用 `DeploymentLauncher` 开始的模拟玩家部署。`DeploymentLauncher createsim` 现包括布尔参数 `auto-connect`。这使您能够在部署就绪时自动将模拟玩家连接至部署。

### 问题修复：
- 模拟玩家协调器的示例 worker 配置现在已与身份验证流兼容。
- `Start Deployment` 下拉菜单中的 `Cloud Deployment Name` 字段，以及 `Cloud Deployment Configuration` 窗口中的 `Deployment name` 字段，现在指的是同一个属性。`Start Deployment` 工具栏按钮现在使用下拉菜单中指定的名称。
- `Start Deployment` 下拉菜单中的 `Local Deployment IP`、`Cloud Deployment Name` 标签，当编辑框禁用时，其颜色会相应变灰。
- 在 **Editor Settings** 中的 `Exposed local runtime IP address` 字段内输入无效 IP 后，现会触发警告弹窗并将值重设为空字符串。
- 修复了导致以下错误的问题： `ResolveObjectReferences: Removed unresolved reference: AbsOffset >= MaxAbsOffset`。
- 当 `GridBasedLBStrategy` 无法定位对一个 Actor 具有管辖权的 worker 实例，现会记录包含 `Position` 的错误。
- `SpatialGDKsetting bEnableMultiWorker` 现为私有变量，来保证通过 `-OverrideMultiWorker` 标记来设置 `IsMultiWorkerEnabled`。
- 当 `SpatialStatics::GetActorEntityId` 函数传入 `nullptr` 时，现会返回 `SpatialConstants::INVALID_ENTITY_ID`。
- 删除了 `EditorWorkerController` 类 (运行连续性 PIE 会话时不再需要此类)。
- 修复一项崩溃：之前在尝试刷新后，溢出 RPC 会保持溢出状态。
- 修复一项崩溃：之前在启用 ring buffer 来执行服务器穿梭后，有时会发生崩溃。


## [`0.10.0`] - 2020-07-08

### New Known Issues:
- Replicated properties that use the `COND_SkipOwner` replication condition can still replicate in the first few frames of an Actor becoming owned.
- Microsoft have fixed a defect in MSVC that previously caused errors when building Unreal Engine. We documented a workaround for this issue in GDK version [`0.7.0-preview`](#070-preview---2019-10-11). If you set up the GDK on your computer between the release of `0.7.0` and `0.10.0`, you have performed this workaround, which is no longer necessary. To undo this workaround, follow these steps:
1. Open Visual Studio Installer.
1. Select **Modify** on your Visual Studio 2019 installation.
1. In the **Installation details** section, clear all the checkboxes for workloads and components except **Visual Studio Code editor**.
1. In the **Workloads** tab, select the following items:
 - **Universal Windows Platform development**
 - **.NET desktop development** (You must also select the **.NET Framework 4.6.2 development tools**.)
 - **Desktop development with C++**
5. Select **Modify** to confirm your changes.

### Breaking Changes:
- We've deprecated the `preview` branches. We now only release the GDK to the `release` branch, which is fully tested, stable, and documented. If you have the `preview` branches checked out, you must check out `release` to receive the latest changes.
- The SpatialOS Runtime Standard variant requires the latest version of the SpatialOS CLI. Run `spatial update` to get the latest version.
- Old snapshots are incompatible with version 0.10 of the GDK. You must generate new snapshots after upgrading to 0.10.
- The old Inspector is incompatible with the SpatialOS Runtime Standard variant. The Standard variant uses the new Inspector by default.
- We’ve removed `Singleton` as a class specifier, and you need to remove your uses of it. You can achieve the behavior of former Singleton Actors by ensuring that your Actor is spawned once by a single server-worker instance in your deployment.
- We’ve renamed `OnConnected` and `OnConnectionFailed` (on `SpatialGameInstance`) to `OnSpatialConnected` and `OnSpatialConnectionFailed`. They are now also Blueprint-assignable.
- The `GenerateSchema` and `GenerateSchemaAndSnapshots` commandlets do not generate schema any more. We’ve deprecated them in favor of `CookAndGenerateSchemaCommandlet`. (`GenerateSchemaAndSnapshots` still works if you use the `-SkipSchema` option.)
- We’ve combined the settings for offloading and load balancing and moved them from the Editor and Runtime Settings to be per map in the World Settings. For more information, see the [offloading tutorial](https://documentation.improbable.io/gdk-for-unreal/docs/1-set-up-multiserver).
- We’ve removed the command-line arguments `OverrideSpatialOffloading` and `OverrideLoadBalancer`, and GDK load balancing is always enabled. To override a map's `Enable Multi Worker` setting, use the command-line flag `OverrideMultiWorker`.
- It is now mandatory to run a deployment with result types (previously result types were enabled by default). We’ve removed the Runtime setting `bEnableResultTypes` to reflect this.
- Whether an Actor is offloaded depends on whether the root owner of that Actor is offloaded. This might affect you if you're using functions such as `IsActorGroupOwnerForActor`.
- We’ve removed `QueuedOutgoingRPCWaitTime`. All RPC failure cases are now correctly queued or dropped.
- We’ve removed `Max connection capacity limit` and `Login rate limit` from the generated worker configuration file, because we no longer support them.
- We no longer support secure worker connections when you run your game within the Unreal Editor. We still support secure worker connections for packaged builds.

### Features:
- The GDK now uses the SpatialOS Worker SDK version [`14.6.1`](https://documentation.improbable.io/sdks-and-data/docs/release-notes#section-14-6-1).
- Added support for the SpatialOS Runtime [Standard variant](https://documentation.improbable.io/gdk-for-unreal/docs/the-spatialos-runtime#section-runtime-variants), version 0.4.3.
- Added support for the SpatialOS Runtime [Compatibility Mode variant](https://documentation.improbable.io/gdk-for-unreal/docs/the-spatialos-runtime#section-runtime-variants), version [`14.5.4`](https://forums.improbable.io/t/spatialos-13-runtime-release-notes-14-5-4/7333).
- Added a new drop-down menu in Editor Settings so that you can select which SpatialOS Runtime variant to use. The two variants are Standard and Compatibility Mode. For Windows users, Standard is the default, but you can use Compatibility Mode if you experience networking issues when you upgrade to the latest GDK version. For macOS users, Compatibility Mode is the default, and you can’t use Standard. For more information, see [Runtime variants](https://documentation.improbable.io/gdk-for-unreal/docs/the-spatialos-runtime#section-runtime-variants).
- Added new default game templates. Your default game template depends on the SpatialOS Runtime variant that you have selected, and on your primary deployment region.
- The SpatialOS Runtime Standard variant uses the new Inspector by default, and is incompatible with the old Inspector. (The Compatibility Mode variant uses the old Inspector by default, and is incompatible with the new Inspector.)
- The Example Project has a new default game mode: Control. This game mode replaces Deathmatch. In Control, two teams compete to capture control points on the map. NPCs guard the control points, and if you capture an NPC’s control point, then the NPC joins your team.
- You can now generate valid schema for classes that start with a leading digit. The generated schema classes are prefixed with `ZZ` internally.
- Handover properties are now automatically replicated when this is required for load balancing. `bEnableHandover` is off by default.
- Added `OnSpatialPlayerSpawnFailed` delegate to `SpatialGameInstance`. This is useful if you have established a successful connection from the client-worker instance to the SpatialOS Runtime, but the server-worker instance crashed.
- Added `bWorkerFlushAfterOutgoingNetworkOp` (default false) which sends RPCs and property replication changes over the network immediately, to allow for lower latencies. You can use this with `bRunSpatialWorkerConnectionOnGameThread` to achieve the lowest available latency at a trade-off with bandwidth.
- You can now edit the project name field in the `Cloud Deployment Configuration` dialog box. Changes that you make here are reflected in your project's `spatialos.json` file.
- You now define worker types in Runtime Settings.
- Local deployments now use the map's load balancing strategy to get the launch configuration settings. The launch configuration file is saved per map in the `Intermediate/Improbable` folder.
- Added a `Launch Configuration Editor` under the Cloud toolbar button.
- In the `Cloud Deployment Configuration` dialog box you can now generate a launch configuration file from the current map, or you can click through to the `Launch Configuration Editor`.
- You can now specify worker load in game logic by using `SpatialMetrics::SetWorkerLoadDelegate`.
- You can now specify deployment tags in the `Cloud Deployment Configuration` dialog box.
- You can now execute RPCs that were declared in a `UInterface`. Previously, this caused a runtime assertion.
- Full Scan schema generation now uses the `CookAndGenerateSchema` commandlet, which results in faster and more stable schema generation for big projects.
- Added an `Open Deployment Page` button to the `Cloud Deployment Configuration` dialog box.
- The `Start Deployment` button in the `Cloud Deployment Configuration` dialog box now generates schema and a snapshot, builds all selected workers, and uploads the assembly before starting the deployment. There are checkboxes so that you can choose whether to generate schema and a snapshot, and whether to build the game client and add simulated players.
- When you start a cloud deployment from the Unreal Editor, the cloud deployment now automatically has the dev_login deployment tag.
- Several command-line parameter changes:
  - Renamed the `enableProtocolLogging` command-line parameter to `enableWorkerSDKProtocolLogging`.
  - Added a parameter named enableWorkerSDKOpLogging so that you can log user-level ops.
  - Renamed the `protocolLoggingPrefix` parameter to workerSDKLogPrefix. This prefix is used for both protocol logging and op logging.
  - Added a parameter named `workerSDKLogLevel` that takes the arguments `debug`, `info`, `warning`, and `error`.
  - Added a parameter named `workerSDKLogFileSize` to control the maximum file size of the Worker SDK log file.
- The icon on the `Start Deployment` toolbar button now changes depending on the connection flow that you select.
- Created a new drop-down menu in the GDK toolbar. You can use it to configure how to connect your PIE client or your Launch on Device client:
  - Choose between `Connect to a local deployment` and `Connect to a cloud deployment` to specify the flow that the client should automatically use when you select `Play` or `Launch`.
  - Added the `Local Deployment IP` field to specify which local deployment the client should connect to. By default, the IP is `127.0.0.1`.
  - Added the `Cloud deployment name` field to specify which cloud deployment the client should connect to. If you select Connect to cloud deployment but you don’t specify a cloud deployment, the client tries to connect to the first running deployment that has the `dev_login` deployment tag.
  - Added the `Editor Settings` field so that you can quickly access the SpatialOS Editor Settings.
- Added the `Build Client Worker` and `Build Simulated Player` checkboxes to the `Connection` drop-down menu, so that you can quickly choose whether to build and include the client-worker instance and simulated player worker instance in the assembly.
- Updated the GDK toolbar icons.
- When you specify a URL to connect a client to a deployment using the Receptionist, the URL port option is now respected. - --- However, in certain circumstances, the initial connection attempt uses the `-receptionistPort` command-line argument.
- When you run `BuildWorker.bat` with `client`, this now builds the client target of your project.
- When you change the project name in the `Cloud Deployment Configuration` dialog box, this automatically regenerates the development authentication token.
- Changed the names of the following toolbar buttons:
  - `Start` is now called `Start Deployment`
  - `Deploy` is now called `Cloud`
- Marked all the required fields in the `Cloud Deployment Configuration` dialog box with asterisks.
- You can now change the project name in Editor Settings.
- Replaced the `Generate from current map` button in the `Cloud Deployment Configuration` dialog box with a checkbox labelled `Automatically Generate Launch Configuration`. If you select this checkbox, the GDK generates an up-to-date launch configuration file from the current map when you select `Start Deployment`.
- Android and iOS are now in preview. We support workflows for developing and doing in-studio playtests on Android and iOS devices, and have documentation for these workflows. We also support macOS (also in preview) for developing and testing iOS game clients.

## Bug fixes:
- Fixed a problem that caused load balanced cloud deployments to fail to start while under heavy load.
- Fix to avoid using packages that are still being processed in the asynchronous loading thread.
- Fixed a bug that sometimes caused GDK setup scripts to fail to unzip dependencies.
- Fixed a bug where RPCs that were called before calling the `CreateEntityRequest` were not processed as early as possible in the RPC ring buffer system, resulting in startup delays on the client.
- Fixed a crash that occurred when running a game with `nullrhi` and using `SpatialDebugger`.
- When you use a URL with options in the command line, we now parse the Receptionist parameters correctly, using the URL if necessary.
- Fixed a bug that occurred when creating multiple dynamic subobjects at the same time, and caused them to fail to be created on clients.
- `OwnerOnly` components are now properly replicated when a worker instance gains authority over an Actor. Previously, they were sometimes only replicated when a value on them changed (after the worker instance had already gained authority).
- Fixed a rare server crash that could occur when closing an Actor channel immediately after attaching a dynamic subobject to that Actor.
- Fixed a defect in `InstallGDK.bat` that sometimes caused it to incorrectly report `Error: Could not clone…` when repositories were cloned correctly.
- Actors from the same ownership hierarchy are now handled together when they are load balanced.

## SpatialOS tooling compatibility:
If you are using the Standard Runtime variant, note the following compatibility issues:
- The [old Inspector](https://documentation.improbable.io/spatialos-tools/docs/the-inspector) won’t work. You must use the [new Inspector](https://documentation.improbable.io/spatialos-tools/docs/the-new-inspector) instead.
- In the [Platform SDK in C#](https://documentation.improbable.io/sdks-and-data/docs/platform-csharp-introduction), you can’t set [capacity limits](https://documentation.improbable.io/sdks-and-data/docs/platform-csharp-capacity-limiting) or use the [remote interaction service](https://documentation.improbable.io/sdks-and-data/docs/platform-csharp-remote-interactions). You also can’t use the Platform SDK to take snapshots of cloud deployments, but we’ll fix this snapshot issue in a future release.
- You can't generate a snapshot of a cloud deployment. We'll fix this in a future release.
- In the [CLI](https://documentation.improbable.io/spatialos-tools/docs/cli-introduction), the following commands don’t work:
  - `spatial local worker replace`
  - `spatial project deployment worker replace`
  - `spatial local worker-flag set`
  - `spatial project deployment worker-flag delete`
  - `spatial project deployment worker-flag set`
  - `spatial cloud runtime flags set` (We’ll improve debug tooling and add functionality to [dynamically change worker flag values](https://documentation.improbable.io/gdk-for-unreal/docs/worker-flags#section-change-worker-flag-values-while-the-deployment-is-running) in future releases.)

If you need any of the functionality mentioned above, [change your Runtime variant to Compatibility Mode](https://documentation.improbable.io/gdk-for-unreal/docs/the-spatialos-runtime#section-change-your-runtime-variant).

### Internal:
Features listed in this section are not ready to use. However, in the spirit of open development, we record every change that we make to the GDK.

- The SpatialOS GDK for Unreal is now released automatically using Buildkite CI. This should result in more frequent releases.
- Improbable now measures the non-functional characteristics of the GDK in Buildkite CI. This enables us to reason about and improve these characteristics. We track them as non-functional requirements (NFRs).

### [`0.10.0`] 中文版更新日志

### 已知问题
- 使用 `COND_SkipOwner` 复制条件的复制属性在 Actor 成为所属的前几帧中仍然可以复制。
- Microsoft 已修复 MSVC 中导致构建虚幻引擎时出错的一个缺陷。我们在 GDK 版本 [`0.7.0-preview`](#070-preview---2019-10-11) 中记录了此问题的解决方法。如果您在计算机上设置的 GDK 版本介于 `0.7.0` 和 `0.10.0` 之间，那么您已经应用此解决方法，但该解决方法不再需要。要撤销此解决方法，完成以下步骤：
1. 打开 Visual Studio Installer。
1. 在您安装的 Visual Studio 2019 中，点击 **Modify**。
1. 在 **Installation details** 区域, 清空所有工作负载和组件的复选框 (除了 **Visual Studio Code editor**)。
1. 在 **Workloads** 选项卡，选中以下内容：
   - **Universal Windows Platform development**
   - **.NET desktop development** (您必须同时勾选 **.NET Framework 4.6.2 development tools**)
   - **Desktop development with C++**
5. 点击 **Modify** 确认更改。

### 重大变化
- 我们已经弃用了 `preview` 分支。现在，我们仅将 GDK 发布到 `release` 分支，该分支经全面测试、性能稳定且有相关文档。如果您检出了 `preview` 分支，那么必须检出 `release` 以接收最新更改。
- SpatialOS 运行时标准体 (Standard) 需要最新版本的 SpatialOS CLI。运行 `spatial update` 以获取最新版本。
- 旧快照与 GDK 的 0.10 版本不兼容。升级到 0.10 版本后，您必须生成新的快照。
- 旧的 Inspector 与 SpatialOS 运行时标准体不兼容。默认情况下，标准体使用新的 Inspector。
- 我们已经移除 `Singleton` 作为类说明符，您也需要删除对它的使用。要实现以前的 Singleton Actor 的行为，确保您的 Actor 仅由部署中的单个服务端 worker 实例生成一次。
- 我们已将 `SpatialGameInstance` 上的 `OnConnected` 和 `OnConnectionFailed` 重命名为 `OnSpatialConnected` 和 `OnSpatialConnectionFailed`。它们现在也可以分配给蓝图。
- `GenerateSchema` 和 `GenerateSchemaAndSnapshots` 命令行开关不再生成结构描述 (schema)。我们已弃用它们，推荐您使用 `CookAndGenerateSchemaCommandlet`。如果您使用 `-SkipSchema` 选项，` GenerateSchemaAndSnapshots` 仍然有效。
- 我们合并了负载拆分和负载均衡设置，并将它们从 Editor 和 Runtime Settings 面板中移到了 World Settings 中的每个地图上。更多信息，查看 [负载拆分教程](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/1-set-up-multiserver)。
- 我们已经删除了命令行参数 `OverrideSpatialOffloading` 和 `OverrideLoadBalancer`，并且 GDK 负载均衡始终处于启用状态。要覆盖地图上的 “`Enable Multi Worker` 设置，使用命令行标记 `OverrideMultiWorker`。
- 现在必须使用结果类型 (此前默认已启用结果类型 - result types) 运行部署。我们已删除运行时设置 `bEnableResultTypes`  以反映这一点。
- 某 Actor 是否已负载拆分取决于该 Actor 的根所有者 (root owner) 是否已负载拆分。如果您使用的是例如 `IsActorGroupOwnerForActor` 的函数，这可能会影响您。
- 我们删除了 `QueuedOutgoingRPCWaitTime`。现在，所有 RPC 故障案例都已正确队列或丢弃。
- 我们已从生成的 worker 配置文件中删除了 `Max connection capacity limit` 和 `Login rate limit`，因为我们不再支持它们。
- 我们不再支持在虚幻编辑器中运行游戏时安全的 worker 连接。我们仍然支持打包构建时安全的 worker 连接。

### 功能介绍
- GDK 现使用 SpatialOS Worker SDK 版本 [`14.6.1`](https://documentation.improbable.io/sdks-and-data/lang-zh_CN/docs/release-notes#section-14-6-1)。
- 支持 SpatialOS 运行时  [标准体](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/the-spatialos-runtime#section-runtime-variants), 0.4.3 版本。
- 支持 SpatialOS 运行时 [兼容模式](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/the-spatialos-runtime#section-runtime-variants)，[`14.5.4`](https://forums.improbable.io/t/spatialos-13-runtime-release-notes-14-5-4/7333) 版本。
- 在 Editor Settings 面板中添加一个新的下拉菜单，因此您可以选择使用哪个 SpatialOS 运行时变体。有两个变体可选：标准体和兼容模式。对于 Windows 用户，默认情况下使用标准体，但当您升级到最新 GDK 版本时如果遇到网络问题，您可以使用兼容模式。对于 macOS 用户，默认情况下使用兼容模式，并且您无法使用标准体。更多信息，查看 [运行时变体](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/the-spatialos-runtime#section-runtime-variants).
- 添加新的默认游戏模板。您的默认游戏模板取决于您选择的 SpatialOS 运行时变体以及您的主要部署地区。
- 默认情况下， SpatialOS 运行时标准使用新的 Inspector，并且与旧的 Inspector 不兼容；默认情况下，“兼容模式”变体使用旧的 Inspector，并且与新的 Inspector 不兼容。
- 示例项目具有一个新的默认游戏模式：控制。该游戏模式取代了死亡竞赛。在 “控制” 中，两个团队竞争以占领地图上的控制点。 NPC 保护控制点，如果您占领 NPC 的控制点，那么 NPC 就会加入您的团队。
- 您现在可以为开头是数字的类生成有效的结构描述。生成的结构描述类在内部以 `ZZ`  作为前缀。
- Handover properties are now automatically replicated when this is required for load balancing. `bEnableHandover` is off by default. 当负载均衡需要时，迁移属性现在会自动迁移复制。默认情况下，`bEnableHandover` 关闭。
- 在 `SpatialGameInstance` 中添加 `OnSpatialPlayerSpawnFailed` 委托。如果您已成功建立从客户端 worker 实例到 SpatialOS 运行时的连接，但服务端 worker 实例崩溃时，这将很有用。
- 添加 `bWorkerFlushAfterOutgoingNetworkOp` (默认为 false)，可立即通过网络发送 RPC 和属性复制更改，以降低延迟。您可以将其与 `bRunSpatialWorkerConnectionOnGameThread` 结合使用，以权衡带宽而获得最低的可用延迟。
- 您现在可以在 `Cloud Deployment Configuration` 对话框中编辑项目名称字段。您在此处所做的更改将反映在项目的 `spatialos.json` 文件中。
- 您现在可以在 Runtime Settings 面板中定义 worker 类型。
- 本地部署现在使用地图的负载均衡策略来获取启动配置设置。每个地图的启动配置文件都保存在 `Intermediate/Improbable` 文件夹中。
- 在 Cloud 工具栏按钮下添加 `Launch Configuration Editor`。
- 在 `Cloud Deployment Configuration` 对话框中，您现在可以从当前地图生成一个启动配置文件，或者您可以点击打开 `Launch Configuration Editor`。
- 现在，您可以使用 `SpatialMetrics::SetWorkerLoadDelegate` 在游戏逻辑中指定Worker 负载。
- 现在，您可以在 `Cloud Deployment Configuration` 对话框中指定部署标签。
- 现在，您可以执行在 `UInterface` 中声明的 RPC。之前，这会引起运行时断言。
- 完全扫描 (Full Scan) 结构描述生成现在使用 `CookAndGenerateSchema`  命令行开关，这样可以为大型项目更迅速、更稳定地生成结构描述。
- 在 `Cloud Deployment Configuration` 对话框中添加 `Open Deployment Page` 按钮。
- `Cloud Deployment Configuration` 对话框中的 `Start Deployment` 按钮现在会生成结构描述和快照，并且在部署启动前构建所有选定的 worker 和上传程序集。提供了复选框，您可以选择是否生成结构描述和快照，以及是否构建游戏客户端和添加模拟玩家。
- 在虚幻编辑器启动云部署时，云部署现在会自动具有 `dev_login` 部署标签。
- 几个命令行参数更改：
  - 将 `enableProtocolLogging` 命令行参数重命名为 `enableWorkerSDKProtocolLogging`。
  - 添加一个名为 `enableWorkerSDKOpLogging` 的参数，因而您可以记录用户级别操作日志。
  - 将 `protocolLoggingPrefix` 参数重命名为 `workerSDKLogPrefix`。该前缀同时用于协议日志和操作日志。
  - 添加一个名为 `workerSDKLogLevel` 的参数，该参数带有参数 `debug`、`info`、`warning` 和 `error`。
  - 添加一个名为 `workerSDKLogFileSize` 的参数，以控制 Worker SDK 日志文件的最大文件大小。
- `Start Deployment` 工具栏按钮上的图标现在会根据您选择的连接工作流而变化。
- 在 GDK 工具栏中创建一个新的下拉菜单，用来配置如何连接您的 PIE 客户端或是设备上启动的客户端：
  - 在 `Connect to a local deployment` 和 `Connect to a cloud deployment` 之间进行选择，以指定在选择 `Play` 或 `Launch` 时客户端应自动使用的流。
  - 添加 `Local Deployment IP` 字段，以指定客户端应连接到的本地部署。默认情况下，IP为 `127.0.0.1`。
  - 添加 `Cloud deployment name` 字段，以指定客户端应连接到的云部署。如果选择 `Connect to cloud deployment`，但未指定云部署，则客户端将尝试连接到第一个运行中具有 `dev_login` 部署标签的部署。
  - 添加 `Editor Settings` 字段，以便您可以快速访问 SpatialOS Editor Settings 面板。
- 在 `Connection` 下拉菜单中添加 `Build Client Worker` 和 `Build Simulated Player` 复选框，以便您可以快速选择是否要构建并在程序集中包括客户端 worker 实例和模拟玩家 worker 实例。
- 更新 GDK 工具栏图标。
- 当您指定一个 URL 来通过 Receptionist 将客户端连接到部署时，现在会使用该 URL 端口选项。但是，在某些情况下，初始连接尝试使用 `-receptionistPort` 命令行参数。
- 现在，当您使用 `client` 运行 `BuildWorker.bat` 时，这将构建您项目的客户端目标。
- 当您在 `Cloud Deployment Configuration` 对话框中更改项目名称时，这将自动重新生成开发身份验证令牌。
- 更改了以下工具栏按钮的名称：
  - `Start` 现改名为 `Start Deployment`
  - `Deploy` 现改名为 `Cloud`
- 在 `Cloud Deployment Configuration` 对话框中，用星号标记所有必填项。
- 您现在可以在 Editor Settings 面板中更改项目名称。
- 将 **Cloud Deployment Configuration** 对话框中的 **Generate from current map** 按钮替换为一个标记为 **Automatically Generate Launch Configuration** 的复选框。如果选中此框，当您点击 **Start Deployment** 时，GDK 会从当前地图生成最新的启动配置文件。
- Android 和 iOS 现处于预览状态。我们支持在 Android 和 iOS 设备上进行游戏开发和工作室内测试的工作流，并提供工作流的相关文档。我们还支持在 macOS (也在预览中) 计算机上开发和测试 iOS 游戏客户端。

### 问题修复
- 修复导致负载均衡的云部署在高负载下无法启动的问题。
- 进行修复，以避免使用仍在异步加载线程中处理的包。
- 修复有时导致 GDK 安装脚本无法解压缩依赖项的错误。
- 修复一个错误，该错误导致在调用  `CreateEntityRequest`前调用的 RPC 在 RPC 环形缓冲区系统中未尽早处理，从而导致客户端启动延迟。
- 修复在运行使用 `nullrhi` 和 `SpatialDebugger` 的游戏时发生的崩溃。
- 当您在命令行中使用带有参数的 URL 时，我们现在可以正确解析 Receptionist 参数，在必要时使用 URL。
- 修复在同时创建多个动态子对象时，导致在客户端上创建失败的错误。
- 当 worker 实例获得对 Actor 的管辖权时， `OwnerOnly` 组件可以正确复制。以前，有时只有当它们的值更改时才会进行复制 (在 worker 实例获得管辖权后)。
- 修复将动态子对象附加到 Actor 后立即关闭该 Actor 通道时可能发生的罕见服务器崩溃的情况。
- 修复 `InstallGDK.bat` 中的一个缺陷，该缺陷有时会导致当正确克隆仓库时，它错误地报告 `Error: Could not clone…`。
- 来自同一所有权层次结构的 Actor，当对它们进行负载均衡后，可以一起处理。

### SpatialOS 工具兼容性
如果您正在使用运行时的标准体，注意以下兼容问题：
- 旧版 [old Inspector](https://documentation.improbable.io/spatialos-tools/lang-zh_CN/docs/the-inspector) 不适用。您必需使用 [新版 Inspector](https://documentation.improbable.io/spatialos-tools/lang-zh_CN/docs/the-new-inspector)。
- 在 [基于 C# 的 Platform SDK](https://documentation.improbable.io/sdks-and-data/lang-zh_CN/docs/platform-csharp-introduction) 中，您不能设置 [容量限制](https://documentation.improbable.io/sdks-and-data/lang-zh_CN/docs/platform-csharp-capacity-limiting)，或使用 [远程交互服务](https://documentation.improbable.io/sdks-and-data/lang-zh_CN/docs/platform-csharp-remote-interactions)。您也不能使用 Platform SDK 生成云部署的快照，但是我们将在以后的版本中修复此快照问题。
- 您不能生成云部署的快照，但是我们将在以后的版本中修复此快照问题。
- [CLI](https://documentation.improbable.io/spatialos-tools/lang-zh_CN/docs/cli-introduction) 中, 以下命令无法工作：
  - `spatial local worker replace`
  - `spatial project deployment worker replace`
  - `spatial local worker-flag set`
  - `spatial project deployment worker-flag delete`
  - `spatial project deployment worker-flag set`
  - `spatial cloud runtime flags set` (在未来的版本中，我们将改进调试工具，并将功能添加到 [动态更改 worker 标记值](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/worker-flags#section-change-worker-flag-values-while-the-deployment-is-running)。)

如果您需要上面列出的任何功能，[将运行时变体更改为兼容模式](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/the-spatialos-runtime#section-change-your-runtime-variant)。

### 内部变更
本节中列出的功能尚无法使用。但是，本着开放、开发的精神，我们记录了对 GDK 所做的每项更改。

- SpatialOS 的虚幻引擎开发套件现通过 Buildkite CI 自动发布。这意味着未来会有更频繁的发布。
- 英礴现可以统计 Buildkite CI 中 GDK 的非功能性特征。这使我们能够判断并改善这些特性。我们将它们作为非功能性需求 (NFRs) 进行跟踪。


## [`0.9.0`] - 2020-05-05

### New Known Issues:
- After you upgrade to Unreal Engine `4.24.3` using `git pull`, you might be left in a state where several `.isph` and `.ispc` files are missing. This state produces [compile errors](https://forums.unrealengine.com/unreal-engine/announcements-and-releases/1695917-unreal-engine-4-24-released?p=1715142#post1715142) when you build the engine. You can fix this by running `git restore .` in the root of your `UnrealEngine` repository.

### Breaking Changes:
- Simulated player worker configurations now require a development authentication token and deployment flag instead of a login token and player identity token. See the Example Project for an example of how to set this up.

### Features:
- We now support Unreal Engine `4.24.3`. You can find the `4.24.3` version of our engine fork [here](https://github.com/improbableio/UnrealEngine/tree/4.24-SpatialOSUnrealGDK-release).
- Added a new variable: `QueuedOutgoingRPCWaitTime`. Outgoing RPCs are now dropped in the following three scenarios: more than `QueuedOutgoingRPCWaitTime` time has passed since the RPC was sent; the worker instance is never expected to have the authority required to receive the RPC (if you're using offloading or zoning); or the Actor that the RPC is sent to is being destroyed.
- In local deployments of the Example Project, you can now launch simulated players with one click. To launch a single simulated player client, run `LaunchSimPlayerClient.bat`. To launch ten simulated player clients, run `Launch10SimPlayerClients.bat`.
- Added support for the UE4 Network Profiler to measure relative size of RPC and Actor replication data.
- Added a `SpatialToggleMetricsDisplay` console command. You must enable `bEnableMetricsDisplay` in order for the metrics display to be available. You must then must call `SpatialToggleMetricsDisplay` on each client that you want the metrics display to be visible for.
- Enabled compression in the Modular UDP networking stack.
- Switched off default RPC-packing. You can re-enable this in `SpatialGDKSettings`.
- When you start a local deployment, we now check to see if the required Runtime port is blocked. If it is, we display a dialog box that asks whether you want to kill the process that is blocking the port.
- A configurable Actor component `SpatialPingComponent` is now available. This enables PlayerControllers to measure the ping time to the server-worker instances that have authority over them. You can access the latest raw ping value via `GetPing()`, or access the rolling average, which is stored in `PlayerState`.
- You can invoke the `GenerateSchema`, `GenerateSchemaAndSnapshots`, and `CookAndGenerateSchema` commandlets with the `-AdditionalSchemaCompilerArguments="..."` command line switch to output additional compiled schema formats. If you don't provide this switch, the output contains only the schema descriptor. This switch's value should be a subset of the arguments that you can pass to the schema compiler directly (for example `--bundle_out="path/to/bundle.sb"`). You can see a full list of possible values in the [schema compiler documentation](https://docs.improbable.io/reference/14.2/shared/schema/introduction#schema-compiler-cli-reference)
- Added the `AllowUnresolvedParameters` function flag. This flag disables warnings that occur during processing of RPCs that have unresolved parameters. To enable this flag, use Blueprints, or add a tag to the `UFUNCTION` macro.
- There is now a warning if you launch a cloud deployment with the `manual_worker_connection_only` flag set to `true`.
- We now support server travel for single-server game worlds. We don't support server travel for game worlds that use zoning or offloading.
- Improved the workflow relating to schema generation issues when you launch local deployments. There is now a warning if you try to launch a local deployment after a schema error.
- `DeploymentLauncher` can now launch a simulated player deployment independently from the target deployment.
Usage: `DeploymentLauncher createsim <project-name> <assembly-name> <target-deployment-name> <sim-deployment-name> <sim-deployment-json> <sim-deployment-region> <num-sim-players> <auto-connect>`
- We now use `HeartbeatTimeoutWithEditorSeconds` if `WITH_EDITOR` is defined. This prevents worker instances from disconnecting when you're running them from the Unreal Editor for debugging.
- Added the `bAsyncLoadNewClassesOnEntityCheckout` setting to `SpatialGDKSettings`. This allows worker instances to load new classes asynchronously when they are receiving the initial updates about an entity. It is `false` by default.
- Added `IndexYFromSchema` functions for the `Coordinates`, `WorkerRequirementSet`, `FRotator`, and `FVector` classes. We've remapped the `GetYFromSchema` functions for the same classes to invoke `IndexYFromSchema` internally, in line with other implementations of the pattern.
- Clients now validate their schema files against the schema files on the server, and log a warning if the files do not match.
- Entries in the schema database are now sorted to improve efficiency when searching for assets in the Unreal Editor. (DW-Sebastien)
- `BatchSpatialPositionUpdates` in `SpatialGDKSettings` now defaults to false.
- Added `bEnableNetCullDistanceInterest` (default `true`) to enable client interest to be exposed through component tagging. This functionality has closer parity to native Unreal client interest.
- Added `bEnableNetCullDistanceFrequency` (default `false`) to enable client interest queries to use frequency. You can configure this functionality using `InterestRangeFrequencyPairs` and `FullFrequencyNetCullDistanceRatio`.
- Introduced the feature flag `bEnableResultTypes` (default `true`). This configures interest queries to include only the set of components required for the queries to run. Depending on your game, this might save bandwidth.
- If you set the `bEnableResultTypes` flag to `true`, this disables dynamic interest overrides.
- Moved the development authentication settings from the Runtime Settings panel to the Editor Settings panel.
- Added the option to use the development authentication flow with the command line.
- Added a button to generate a development authentication token inside the Unreal Editor. To use it, navigate to **Edit** > **Project Setting** > **SpatialOS GDK for Unreal** > **Editor Settings** > **Cloud Connection**.
- Added a new section where you can configure the launch arguments for running a client on a mobile device. To use it, navigate to **Edit** > **Project Setting** > **SpatialOS GDK for Unreal** > **Editor Settings** > **Mobile**.
- You can now choose which Runtime version to use (in the Runtime Settings) when you launch a local or cloud deployment.
- If you set the `--OverrideResultTypes` flag to `true`, server-worker instances no longer receive updates about server RPC components on Actors that they do not own. This should decrease bandwidth for server-worker instances in offloaded and zoned games.
- The `InstallGDK` scripts now `git clone` the correct version of the `UnrealGDK` and `UnrealGDKExampleProject` for the `UnrealEngine` branch that you have checked out. They read `UnrealGDKVersion.txt` and `UnrealGDKExampleProjectVersion.txt` to determine what the correct branches are.
- Removed the `bEnableServerQBI` property and the `--OverrideServerInterest` flag.
- Added custom warning timeouts for each RPC failure condition.
- `SpatialPingComponent` can now also report average ping measurements over a specified number of recent pings. You can use `PingMeasurementsWindowSize` to specify how many measurements you want to record, and call `GetAverageData` to get the measurement data. There is also a delegate `OnRecordPing` that is broadcast whenever a new ping measurement is recorded.
- The Spatial Output Log window now displays deployment startup errors.
- Added `bEnableClientQueriesOnServer` (default false) which makes the same queries on the server as it makes on clients, if the GDK for Unreal's load balancer is enabled. Enable `bEnableClientQueriesOnServer` to avoid a situation in which clients receive updates about entities that the server doesn't receive updates about (which happens if the server's interest query is configured incorrectly).
- We now log a warning when `AddPendingRPC` fails due to `ControllerChannelNotListening`.
- When offloading is enabled, Actors have local authority (`ROLE_Authority`) on servers for longer periods of time, to allow more native Unreal functionality to work without problems.
- When offloading is enabled, if you try to spawn Actors on a server that will not be the Actor Group owner for them, we now log an error and delete the Actors.
- The GDK now uses SpatialOS Runtime version 14.5.1 by default.
- Renamed the configuration setting `bPreventAutoConnectWithLocator` to `bPreventClientCloudDeploymentAutoConnect` and moved it to `SpatialGDKSettings`. To use this feature, enable the setting in `SpatialGDKSettings`.
- Made `USpatialMetrics::WorkerMetricsRecieved` static.
- You can now connect to a local deployment by selecting **Connect to a local deployment** and specifying the local IP address of your computer in the Launch drop-down menu.
- Enabled RPC ring buffers by default. We'll remove the legacy RPC mode in a future release.
- Removed the `bPackRPCs` property and the `--OverrideRPCPacking` flag.
- Added `OnClientOwnershipGained` and `OnClientOwnershipLost` events on Actors and Actor Components. These events trigger when an Actor is added to or removed from the ownership hierarchy of a client's PlayerController.
- Automatically remove UE4CommandLine.txt after finishing a Launch on device session on an Android device (only UnrealEngine 4.24 or above). This is done to prevent the launch session command line from overriding the one built into the APK.

### Bug fixes:
- Queued RPCs no longer spam logs when an entity is deleted.
- We now take the `OverrideSpatialNetworking` command line argument into account as early as possible (previously, `LocalDeploymentManager` queried `bSpatialNetworking` before the command line was parsed).
- Servers now maintain interest in `AlwaysRelevant` Actors.
- `GetActorSpatialPosition` now returns the last spectator sync location while the player is spectating.
- The default cloud launch configuration is now empty.
- Fixed a crash that happened when the GDK attempted to read schema from an unloaded class.
- We now properly handle (and eventually resolve) unresolved object references in replicated arrays of structs.
- Fixed a tombstone-related assert that could fire and bring down the Unreal Editor.
- If an Actor that is placed in the level with `bNetLoadOnClient=false` goes out of a worker instance's view, it is now reloaded if it comes back into view.
- Fixed a crash in `SpatialDebugger` that was caused by the dereference of an invalid weak pointer.
- Fixed a connection error that occurred when using `spatial cloud connect external`.
- The command line argument `receptionistHost <URL>` no longer overrides connections to `127.0.0.1`.
- If you connect a worker instance to a deployment using the Locator, and you initiate a `ClientTravel` using a URL that requires the Receptionist, this now works correctly.
- You can now access the worker flags via `USpatialStatics::GetWorkerFlag` instead of `USpatialWorkerFlags::GetWorkerFlag`.
- Fixed a crash in `SpatialDebugger` that occurs when GDK-space load balancing is disabled.
- The schema database no longer fails to load previous saved state when working in the Unreal Editor.
- If you attempt to launch a cloud deployment, this now runs the `spatial auth` process as required. Previously the deployment would fail.
- Made a minor spelling fix to the connection log message.
- The debug strings in `GlobalStateManager` now display the Actor class name in log files.
- The server no longer crashes when received RPCs are processed recursively.
- The GDK no longer crashes when `SoftObjectPointers` are not yet resolved, but instead serializes them as expected after they are resolved.
- Fixed an issue that occurred when replicating a property for a class that was part of an asynchronously-loaded package, when the package had not finished loading.
- Fixed component interest constraints that are constructed from schema.
- The GDK now tracks properties that contain references to replicated Actors, so that it can resolve them again if the Actor that they reference moves out of and back into relevance.
- PIE sessions no longer occasionally fail to start due to missing schema for the `SpatialDebugger` Blueprint.
- Fixed an issue where a newly-created subobject had empty state when `RepNotify` was called for a property pointing to that subobject.
- Fixed an issue where deleted, initially dormant startup Actors would still be present on other worker instances.
- We now force-activate the RPC ring buffer when load balancing is enabled, to allow RPC handover when authority changes.
- Fixed a race condition where a client that was leaving the deployment could leave its Actor behind on the server, to be cleaned up after a long timeout.
- Fixed a crash that was caused by state in `SpatialGameInstance` persisting across a transition from one deployment to another.
- The GDK no longer crashes when you start and stop PIE clients multiple times.
- The GDK no longer crashes when shadow data is uninitialized when resolving unresolved objects.
- Fixed an occasional issue when sending component RPCs on a recently-created Actor.

### Internal:
Features listed in this section are not ready to use. However, in the spirit of open development, we record every change that we make to the GDK.

- Enabled the SpatialOS toolbar for MacOS.
- Added support for Android.
- `SpatialDebugger` worker regions are now cuboids rather than planes, and can have their `WorkerRegionVerticalScale` adjusted via a setting in the `SpatialDebugger`.
- Added an `AuthorityIntent` component, a `VirtualWorkerTranslation` component, and a partial framework. We'll use these in the future to control load balancing.
- Load balancing strategies and locking strategies can be set per-level using `SpatialWorldSettings`.
- Added a new Runtime Settings flag to enable the GDK for Unreal load balancer. This is a feature that is in development and not yet ready for general use. Enabling the GDK for Unreal load balancer now creates a single query per server-worker instance, depending on the defined load balancing strategy.
- Extracted the logic responsible for taking an Actor and generating the array of SpatialOS components that represents it as an entity in SpatialOS. This logic is now in `EntityFactory`.
- `DeploymentLauncher` can now parse a .pb.json launch configuration.

### External contributors:
@DW-Sebastien


## [`0.8.1`] - 2020-03-17

### English version
### Adapted from 0.8.1-preview
### Features:
- **SpatialOS GDK for Unreal** > **Editor Settings** > **Region Settings** has been moved to **SpatialOS GDK for Unreal** > **Runtime Settings** > **Region Settings**.
- You can now choose which SpatialOS service region you want to use by adjusting the **Region where services are located** setting. You must use the service region that you're geographically located in.
- Deployments can now be launched in China, when the **Region where services are located** is set to `CN`.
- Updated the version of the local API service used by the UnrealGDK.
- The Spatial output log will now be open by default.
- The GDK now uses SpatialOS 14.5.0.

### Bug fixes:
- Replicated references to newly created dynamic subobjects will now be resolved correctly.
- Fixed a bug that caused the local API service to memory leak.
- Cloud deployment flow will now correctly report errors when a deployment fails to launch due to a missing assembly.
- Errors are now correctly reported when you try to launch a cloud deployment without an assembly.
- The Start deployment button will no longer become greyed out when a `spatial auth login` process times out.
------
### 中文版本
### 引用 0.8.1-preview 版本
### 功能介绍:
- **UI 路径变更**：**SpatialOS GDK for Unreal** > **Editor Settings** > **Region Settings** 移动到 **SpatialOS GDK for Unreal** > **Runtime Settings** > **Region Settings** 路径。
- **新增 SpatialOS 服务地区选项**：通过调整 **Region where services are located** 设置，您可以选择想要使用的 SpatialOS 服务地区。您选择的服务地区必须是您所处的地理位置。
- **新增中国地区部署**：通过将 **Region where services are located** 设置为 `CN`，您可以在中国启动游戏部署。
- **本地 API 服务版本**：虚幻引擎开发套件使用的本地 API 服务版本更新。
- **SpatialOS 输出日志**：SpatialOS 输出日志默认开启。
- 虚幻引擎开发套件现已使用 **SpatialOS 14.5.0** 版本。

### 问题修复:

- 修复对新创建的动态子对象的重复引用问题。
- 修复导致本地 API 服务内存泄漏的问题。
- 在没有程序集的情况下启动云部署时发生的错误，现已正确上报。
- 当 `spatial auth login` 进程超时，启动部署的按钮 (Start) 不再显示为灰色。


## [`0.8.1-preview`] - 2020-03-17

### Internal:
### Adapted from 0.6.5
- **SpatialOS GDK for Unreal** > **Editor Settings** > **Region Settings** has been moved to **SpatialOS GDK for Unreal** > **Runtime Settings** > **Region Settings**.
- You can now choose which SpatialOS service region you want to use by adjusting the **Region where services are located** setting. You must use the service region that you're geographically located in.
- Deployments can now be launched in China, when the **Region where services are located** is set to `CN`.

### Features:
- Updated the version of the local API service used by the UnrealGDK.
- The Spatial output log will now be open by default.
- The GDK now uses SpatialOS 14.5.0.

### Bug fixes:
- Replicated references to newly created dynamic subobjects will now be resolved correctly.
- Fixed a bug that caused the local API service to memory leak.
- Cloud deployment flow will now correctly report errors when a deployment fails to launch due to a missing assembly.
- Errors are now correctly reported when you try to launch a cloud deployment without an assembly.
- The Start deployment button will no longer become greyed out when a `spatial auth login` process times out.

## [`0.8.0-preview`] - 2019-12-17

### English version

### Breaking Changes:
- This is the last GDK version to support Unreal Engine 4.22. You will need to upgrade your project to use Unreal Engine 4.23 (`4.23-SpatialOSUnrealGDK-preview`) in order to continue receiving GDK releases and support.
- When upgrading to Unreal Engine 4.23 you must:
1. `git checkout 4.23-SpatialOSUnrealGDK-preview`
1. `git pull`
1. Download and install the `-v15 clang-8.0.1-based` toolchain from this [Unreal Engine Documentation page](https://docs.unrealengine.com/en-US/Platforms/Linux/GettingStarted/index.html).
1. Navigate to the root of GDK repo and run `Setup.bat`.
1. Run `Setup.bat`, which is located in the root directory of the `UnrealEngine` repository.
1. Run `GenerateProjectFiles.bat`, which is in the same root directory.

For more information, check the [Keep your GDK up to date](https://documentation.improbable.io/gdk-for-unreal/docs/keep-your-gdk-up-to-date) SpatialOS documentation.

### Features:
- You can now call `SpatialToggleMetricsDisplay` from the console in your Unreal clients in order to view metrics. `bEnableMetricsDisplay` must be enabled on clients where you want to use this feature.
- The modular-udp networking stack now uses compression by default.
- Reduced network latency by switching off default rpc-packing. If you need this on by default, you can re-enable it by editing `SpatialGDKSettings.ini`
- When you start a local deployment, the GDK now checks the port required by the runtime and, if it's in use, prompts you to kill that process.
- You can now measure round-trip ping from a player controller to the server-worker that's currently authoritative over it using the configurable actor component 'SpatialPingComponent'. The latest ping value can be accessed through the component via 'GetPing()' or via the rolling average stored in 'PlayerState'.
- You can disable the warnings that trigger when RPCs are processed with unresolved parameters using the `AllowUnresolvedParameters` function flag. This flag can be enabled through Blueprints or by adding a tag to the `UFUNCTION` macro.
- Improved logging around entity creation.
- Unreal Engine `4.23.1` is now supported. You can find the `4.23.1` version of our engine fork [here](https://github.com/improbableio/UnrealEngine/tree/4.23-SpatialOSUnrealGDK-preview).
- In Example Project, the default session duration has increased from 5 minutes to 120 minutes so you don't have to re-deploy while playtesting.
- In Example Project, the default lobby timer has decreased from 15 seconds to 3 seconds so you don't have to wait for your playtest to start.
- Added in-editor support for exposing a local runtime at a particular IP address. This offers the same functionality as the `--runtime_ip` option in the SpatialOS CLI.
- Spatial networking is now always enabled in built assemblies.

### Bug fixes:
- Fixed a bug that could cause name collisions in schema generated for sublevels.
- Downgraded name collisions during schema generation from Warning to Display.
- Replicating a static subobject after it has been deleted on a client no longer results in client attaching a new dynamic subobject.
- Fixed a bug that caused entity pool reservations to cease after a request times out.
- Running `BuildWorker.bat` for `SimulatedPlayer` no longer fails if the project path has a space in it.
- Fixed a crash when starting PIE with out-of-date schema.
- Fixed an issue where launching a cloud deployment with an invalid assembly name or deployment name wouldn't show a helpful error message.

### Internal:
Features listed in the internal section are not ready to use but, in the spirit of open development, we detail every change we make to the GDK.
- We've added a partial loadbalancing framework. When this is completed in a future release, you will be able to control loadbalancing using server-workers.
------

### 中文版本
### 重大变化:
- 这是最后支持虚幻引擎 4.22 版本的 GDK 版本。您需要升级您的项目来使用虚幻引擎 4.23 (`4.23-SpatialOSUnrealGDK-preview`)以便继续获取 GDK 发布和支持。
- 要升级到虚幻引擎 4.23 版本，您必须完成以下步骤：<br>
        1. `git checkout 4.23-SpatialOSUnrealGDK-preview`<br>
        2. `git pull`<br>
        3. 下载并安装 `-v15 clang-8.0.1-based` 工具链。详情参见 [虚幻引擎文档](https://docs.unrealengine.com/zh-CN/Platforms/Linux/GettingStarted/index.html)。<br>
        4. 打开 GDK 的根目录，运行 `Setup.bat`。<br>
        5. 运行 `Setup.bat`, 该程序位于 `UnrealEngine` 仓库的根目录。<br>
        6. 在同样的根目录中，运行 `GenerateProjectFiles.bat`。

更多信息，查看 [使 GDK 保持更新](https://documentation.improbable.io/gdk-for-unreal/lang-zh_CN/docs/keep-your-gdk-up-to-date) 文档。


### 功能介绍:
- 您可以在虚幻引擎客户端的控制台调用 `SpatialToggleMetricsDisplay` 来查看指标。您必须在客户端开启 `bEnableMetricsDisplay` 来使用该功能。
- 默认情况下，模块化udp网络栈现在使用压缩。
- 通过关闭默认 rpc 打包来减少网络延迟。如果需要默认情况下启用它，则可以通过编辑`SpatialGDKSettings.ini`来重新启用它。
- 当您开始本地部署时，GDK 会检查运行时所需的端口。如果该端口正在被使用，则会提示您终止该进程。
- 您可以使用可配置的 Actor 组件 `SpatialPingComponent` 来测量从玩家控制器到当前对其具有管辖权的服务端 worker 的往返 ping。您可以通过 `GetPing()`或存储在 `PlayerState` 中的移动平均值来访问组件获取最新的 ping 值。
- 通过 `AllowUnresolvedParameters` 函数标记，您可以禁用因使用未解析的参数处理 RPC 时触发的警告。您可以使用蓝图或通过向 `UFUNCTION` 宏添加标签来启用此标志。
- 改进关于实体创建的日志输出。
- 现已支持虚幻引擎 `4.23.1`。您可以在 [此处](https://github.com/improbableio/UnrealEngine/tree/4.23-SpatialOSUnrealGDK-preview) 找到虚幻引擎分支的 `4.23.1` 版本。
- 在示例项目中，默认的副本持续时间从5分钟增加到120分钟，因此您不必在进行游戏测试时重新部署。
- 在示例项目中，默认的游戏大厅计时器从15秒减少到3秒，因此您不必等待游戏测试开始。
- 添加编辑器内公开特定 IP 地址的本地运行时支持。它提供的功能与 SpatialOS CLI 中的 `--runtime_ip` 选项相同。
- SpatialOS 网络在构建的程序集中始终开启。

### 问题修复:
- 修复可能导致为关卡分段生成的模式语言中名称冲突的错误。
- 降低模式语言生成过程中的名称冲突级别：由 Warning (警告) 到 Display (显示)。
- 在客户端上删除静态子对象后对其进行复制不再导致客户端附加新的动态子对象。
- 修复请求超时后实体池预留停止的问题。
- 如果项目路径中有空格，为 `SimulatedPlayer` 运行 `BuildWorker.bat` 不再失败。
- 修复使用过期模式语言启动 PIE 时发生崩溃的问题。
- 修复使用无效的程序集名称或部署名称启动云部署不会显示有用的错误消息的问题。

### 内部:
内部部分中列出的功能尚未准备就绪，但本着开放式开发的精神，我们详细介绍了对 GDK 所做的所有更改。
- 我们添加了部分负载均衡框架。在未来的版本中完成此框架后，您将可以使用服务端 worker 来控制负载均衡。


## [`0.7.1-preview`] - 2019-12-06

### Adapted from 0.6.3
### Bug fixes:
- The C Worker SDK now communicates on port 443 instead of 444. This change is intended to protect your cloud deployments from DDoS attacks.

### Internal:
Features listed in the internal section are not ready to use but, in the spirit of open development, we detail every change we make to the GDK.
- The GDK is now compatible with the `CN` launch region. When Improbable's online services are fully working in China, they will work with this version of the GDK. You will be able to create SpatialOS Deployments in China by specifying the `CN` region in the Deployment Launcher.
- `Setup.bat` and `Setup.sh` both accept the `--china` flag, which will be required in order to run SpatialOS CLI commands in the `CN` region.
- **SpatialOS GDK for Unreal** > **Editor Settings** now contains a **Region Settings** section. You will be required to set **Region where services are located** to `CN` in order to create SpatialOS Deployments in China.

## [`0.7.0-preview`] - 2019-10-11

### New Known Issues:
- MSVC v14.23 removes `typeinfo.h` and replaces it with `typeinfo`. This change causes errors when building the Unreal Engine. This issue **only affects Visual Studio 2019** users. You can work around the issue by:
1. Open Visual Studio Installer.
1. Select "Modify" on your Visual Studio 2019 installation.
1. In the Installation details section uncheck all workloads and components until only **Visual Studio code editor** remains.
1. Select the following items in the Workloads tab:
* **Universal Windows Platform development**
* **.NET desktop development**
* You must also select the **.NET Framework 4.6.2 development tools** component in the Installation details section.
* **Desktop development with C++**
* You must then deselect **MSVC v142 - VS 2019 C++ x64/x86 build tools (v14.23)**, which was added as part of the **Desktop development with C++** Workload. You will be notified that: "If you continue, we'll remove the componenet and any items liseted above that depend on it." Select remove to confirm your decision.
* Lastly, add **MSVC v142 - VS 2019 C++ x64/x86 build tools (v14.22)** from the Individual components tab.
5. Select "Modify" to confirm your changes.

### Breaking Changes:
- If your project uses replicated subobjects that do not inherit from ActorComponent or GameplayAbility, you now need to enable generating schema for them using `SpatialType` UCLASS specifier, or by checking the Spatial Type checkbox on blueprints.
- Chunk based interest is no longer supported. All interest is resolved using query-based interest (QBI). You should remove streaming query and chunk based interest options from worker and launch config files to avoid unnecessary streaming queries being generated.
- If you already have a project that you are upgrading to this version of the GDK, it is encouraged to follow the upgrade process to SpatialOS `14.1.0`:
1. Open the `spatialos.json` file in the `spatial/` directory of your project.
1. Replace the `sdk_version` value and the version value of all dependencies with `14.1.0`.
1. Replace all other instances of the version number in the file.

### Features:
- The GDK now uses SpatialOS `14.1.0`.
- Visual Studio 2019 is now supported.
- You can now delete your schema database using options in the GDK toolbar and the commandlet.
- The GDK now checks that schema and a snapshot are present before attempting to start a local deployment. If either are missing then an error message is displayed.
- Added optional net relevancy check in replication prioritization. If enabled, an actor will only be replicated if IsNetRelevantFor is true for one of the connected client's views.
- You can now specify which actors should not persist as entities in your Snapshot. You do this by adding the flag `SPATIALCLASS_NotPersistent` to a class or by entering `NotPersistent` in the `Class Defaults` > `Spatial Description` field on blueprints.
- Deleted startup actors are now tracked.
- Added a user bindable delegate to `SpatialMetrics` which triggers when worker metrics have been received.
- Local deployments now create a new log file known as `launch.log` which will contain logs relating to starting and running a deployment. Additionally it will contain worker logs which are forwarded to the SpatialOS runtime.
- Added a new setting to SpatialOS Runtime Settings `Worker Log Level` which allows configuration of which verbosity of worker logs gets forwarded to the SpatialOS runtime.
- Added a new developer tool called 'Spatial Output Log' which will show local deployment logs from the `launch.log` file.
- Added logging for queued RPCs.
- Added several new STAT annotations into the ServerReplicateActors call chain.
- The GDK no longer generates schema for all UObject subclasses. Schema generation for Actor, ActorComponent and GameplayAbility subclasses is enabled by default, other classes can be enabled using `SpatialType` UCLASS specifier, or by checking the Spatial Type checkbox on blueprints.
- Added new experimental CookAndGenerateSchemaCommandlet that generates required schema during a regular cook.
- Added the `OverrideSpatialOffloading` command line flag. This allows you to toggle offloading at launch time.
- The initial connection from a worker will attempt to use relevant command line arguments (receptionistHost, locatorHost) to inform the connection. If these are not provided the standard connection flow will be followed. Subsequent connections will not use command line arguments.
- The command "Open 0.0.0.0" can be used to connect a worker using its command line arguments, simulating initial connection.
- The command "ConnectToLocator <login> <playerToken>" has been added to allow for explicit connections to deployments.
- Add SpatialDebugger and associated content.  This tool can be enabled via the SpatialToggleDebugger console command.  Documentation will be added for this soon.

### Bug fixes:
- Fixed a bug where the spatial daemon started even with spatial networking disabled.
- Fixed an issue that could cause multiple Channels to be created for an Actor.
- PlayerControllers on non-auth servers now have BeginPlay called with correct authority.
- Attempting to replicate unsupported types (such as TMap) results in an error rather than crashing the game.
- Generating schema when the schema database is locked by another process will no longer crash the editor.
- When the schema compiler fails, schema generation now displays an error.
- Fixed crash during initialization when running GenerateSchemaCommandlet.
- Generating schema after deleting the schema database now correctly triggers an initial schema generation.
- Streaming levels with query-based interest (QBI) enabled no longer produces errors if the player connection owns unreplicated actors.
- Fixed an issue that prevented player movement in a zoned deployment.
- Fixed an issue that caused queued incoming RPCs with unresolved references to never be processed.
- Muticast RPCs that are sent shortly after an actor is created are now correctly processed by all clients.
- When replicating an actor, the owner's Spatial position will no longer be used if it isn't replicated.
- Fixed a crash upon checking out an actor with a deleted static subobject.
- Fixed an issue where launching a cloud deployment with an invalid assembly name or deployment name wouldn't show a helpful error message.

## [`0.6.5`] - 2020-01-13
### Internal:
Features listed in the internal section are not ready to use but, in the spirit of open development, we detail every change we make to the GDK.
- **SpatialOS GDK for Unreal** > **Editor Settings** > **Region Settings** has been moved to **SpatialOS GDK for Unreal** > **Runtime Settings** > **Region Settings**.
- Local deployments can now be launched in China, when the **Region where services are located** is set to `CN`.

## [`0.6.4`] - 2019-12-13
### Bug fixes:
- The Inspector button in the SpatialOS GDK for Unreal toolbar now opens the correct URL.

## [`0.6.3`] - 2019-12-05
### Bug fixes:
- The C Worker SDK now communicates on port 443 instead of 444. This change is intended to protect your cloud deployments from DDoS attacks.

### Internal:
Features listed in the internal section are not ready to use but, in the spirit of open development, we detail every change we make to the GDK.
- The GDK is now compatible with the `CN` launch region. When Improbable's online services are fully working in China, they will work with this version of the GDK. You will be able to create SpatialOS Deployments in China by specifying the `CN` region in the Deployment Launcher.
- `Setup.bat` and `Setup.sh` both accept the `--china` flag, which will be required in order to run SpatialOS CLI commands in the `CN` region.
- **SpatialOS GDK for Unreal** > **Editor Settings** now contains a **Region Settings** section. You will be required to set **Region where services are located** to `CN` in order to create SpatialOS Deployments in China.

## [`0.6.2`] - 2019-10-10

- The GDK no longer relies on an ordering of entity and interest queries that is not guaranteed by the SpatialOS runtime.
- The multiserver offloading tutorial has been simplified and re-factored.

## [`0.6.1`] - 2019-08-15

### Features:
- The [Multiserver zoning shooter tutorial](https://docs.improbable.io/unreal/alpha/content/tutorials/multiserver-shooter/tutorial-multiserver-intro) has been updated to use the Example Project.

### Bug fixes:
- Simulated player launch configurations are no longer invalid when the GDK is installed as an Engine Plugin.
- RPCs that have been queued for execution for more than 1 second (the default value in `SpatialGDKSettings QueuedIncomingRPCWaitTime`) are now executed even if there are unresolved parameters. This stops unresolved parameters from blocking the execution queue.
- Offloading is no longer enabled by default in the Example Project. You can toggle offloading on using [these steps](https://docs.improbable.io/unreal/alpha/content/tutorials/offloading-tutorial/offloading-setup#step-4-enable-offloading).
- Guns no longer intermittently detatch from simulated players in the Example Project.
- Default cloud deployment settings are now correctly set. This means you don't need to manually reset them before doing a cloud deployment.

## [`0.6.0`] - 2019-07-31

### Breaking Changes:
* You must [re-build](https://docs.improbable.io/unreal/alpha/content/get-started/example-project/exampleproject-setup#step-4-build-the-dependencies-and-launch-the-project) your [Example Project](https://github.com/spatialos/UnrealGDKExampleProject) if you're upgrading it to `0.6.0`.
* This is the last GDK version to support Unreal Engine 4.20. You will need to upgrade your project to use Unreal Engine 4.22 (`4.22-SpatialOSUnrealGDK-release`) in order to continue receiving GDK releases and support.

### New Known Issues:
- Workers will sometimes not gain authority when quickly reconnecting to an existing deployment, resulting in a failure to spawn or simulate. When using the editor if you Play - Stop - Play in quick succession you can sometimes fail to launch correctly.

### Features:
- The GDK now uses SpatialOS `13.8.1`.
- Dynamic components are now supported. You can now dynamically attach and remove replicated subobjects to Actors.
- Local deployment startup time has been significantly reduced.
- Local deployments now start automatically when you select `Play`. This means you no longer need to select `Start` in the GDK toolbar before you select `Play` in the Unreal toolbar.
- If your schema has changed during a local deployment, the next time you select `Play` the deployment will automatically restart.
- Local deployments no longer run in a seperate Command Prompt. Logs from these deployments are now found in the Unreal Editor's Output Log.
- SpatialOS Runtime logs can now be found at `<GameRoot>\spatial\logs\localdeployment\<timestamp>\runtime.log`.
- An option to `Show spatial service button` has been added to the SpatialOS Settings menu. This button can be useful when debugging.
- Every time you open a GDK project in the Unreal Editor, 'spatial service' will be restarted. This ensures the service is always running in the correct SpatialOS project. You can disable this auto start feature via the new SpatialOS setting `Auto-start local deployment`.
- Added external schema code-generation tool for [non-Unreal server-worker types]({{urlRoot}}/content/non-unreal-server-worker-types). If you create non-Unreal server-worker types using the [SpatialOS Worker SDK](https://docs.improbable.io/reference/13.8/shared/sdks-and-data-overview) outside of the GDK and your Unreal Engine, you manually create [schema]({{urlRoot}/content/glossary#schema). Use the new [helper-script]({{urlRoot}}/content/helper-scripts) to generate Unreal code from manually-created schema; it enables your Unreal game code to interoperate with non-Unreal server-worker types.
- Added simulated player tools, which will allow you to create logic to simulate the behavior of real players. Simulated players can be used to scale test your game to hundreds of players. Simulated players can be launched locally as part of your development flow for quick iteration, as well as part of a separate "simulated player deployment" to connect a configurable amount of simulated players to your running game deployment.
- Factored out writing of Linux worker start scripts into a library, and added a standalone `WriteLinuxScript.exe` to _just_ write the launch script (for use in custom build pipelines).
- Added temporary MaxNetCullDistanceSquared to SpatialGDKSettings to prevent excessive net cull distances impacting runtime performance. Set to 0 to disable.
- Added `OnWorkerFlagsUpdated`, a delegate that can be directly bound to in C++. To bind via blueprints you can use the blueprint callable functions `BindToOnWorkerFlagsUpdated` and `UnbindToOnWorkerFlagsUpdated`. You can use `OnWorkerFlagsUpdated` to register when worker flag updates are received, which allows you to tweak values at deployment runtime.
- RPC frequency and payload size can now be tracked using console commands: `SpatialStartRPCMetrics` to start recording RPCs and `SpatialStopRPCMetrics` to stop recording and log the collected information. Using these commands will also start/stop RPC tracking on the server.
- Spatial now respects `bAlwaysRelevant` and clients will always checkout Actors that have `bAlwaysRelevant` set to true.

### Bug fixes:
- The `improbable` namespace has been renamed to `SpatialGDK`. This prevents namespace conflicts with the C++ SDK.
- Disconnected players no longer remain on the server until they time out if the client was shut down manually.
- Fixed support for relative paths as the engine association in your games .uproject file.
- RPCs on `NotSpatial` types are no longer queued forever and are now dropped instead.
- Fixed issue where an Actor's Spatial position was not updated if it had an owner that was not replicated.
- BeginPlay is now only called with authority on startup actors once per deployment.
- Fixed null pointer dereference crash when trying to initiate a Spatial connection without an existing one.
- URL options are now properly sent through to the server when doing a ClientTravel.
- The correct error message is now printed when the SchemaDatabase is missing.
- `StartEditor.bat` is now generated correctly when you build a server worker outside of editor.
- Fixed an issue with logging errored blueprints after garbage collection which caused an invalid pointer crash.
- Removed the ability to configure snapshot save folder. Snapshots should always be saved to `<ProjectRoot>/spatial/snapshots`. This prevents an issue with absolute paths being checked in which can break snapshot generation.
- Introduced a new module, `SpatialGDKServices`, on which `SpatialGDK` and `SpatilGDKEditorToolbar` now depend. This resolves a previously cyclic dependency.
- RPCs called before entity creation are now queued in case they cannot yet be executed. Previously they were simply dropped. These RPCs are also included in RPC metrics.
- RPCs are now guaranteed to arrive in the same order for a given actor and all of its subobjects on single-server deployments. This matches native Unreal behavior.

## [`0.5.0-preview`](https://github.com/spatialos/UnrealGDK/releases/tag/0.5.0-preview) - 2019-06-25
- Prevented `Spatial GDK Content` from appearing under Content Browser in the editor, as the GDK plugin does not contain any game content.

### Breaking Changes:
- If you are using Unreal Engine 4.22, the AutomationTool and UnrealBuildTool now require [.NET 4.6.2](https://dotnet.microsoft.com/download/dotnet-framework/net462).

### New Known Issues:

### Features:
- Unreal Engine 4.22 is now supported. You can find the 4.22 verson of our engine fork [here](https://github.com/improbableio/UnrealEngine/tree/4.22-SpatialOSUnrealGDK-release).
- Setup.bat can now take a project path as an argument. This allows the UnrealGDK to be installed as an Engine Plugin, pass the project path as the first variable if you are running Setup.bat from UnrealEngine/Engine/Plugins.
- Removed the need for setting the `UNREAL_HOME` environment variable. The build and setup scripts will now use your project's engine association to find the Unreal Engine directory. If an association is not set they will search parent directories looking for the 'Engine' folder.
- Added the `ASpatialMetricsDisplay` class, which you can use to view UnrealWorker stats as an overlay on the client.
- Added the runtime option `bEnableHandover`, which you can use to toggle property handover when running in non-zoned deployments.
- Added the runtime option `bEnableMetricsDisplay`, which you can use to auto spawn `ASpatialMetricsDisplay`, which is used to remote debug server metrics.
- Added the runtime option `bBatchSpatialPositionUpdates`, which you can use to batch spatial position updates to the runtime.
- Started using the [schema_compiler tool](https://docs.improbable.io/reference/13.8/shared/schema/introduction#using-the-schema-compiler-directly) to generate [schema descriptors](https://docs.improbable.io/reference/13.8/shared/flexible-project-layout/build-process/schema-descriptor-build-process#schema-descriptor-introduction) rather than relying on 'spatial local launch' to do this.
- Changed Interest so that NetCullDistanceSquared is used to define the distance from a player that the actor type is *interesting to* the player. This replaces CheckoutRadius which defined the distance that an actor is *interested in* other types. Requires engine update to remove the CheckoutRadius property which is no longer used.
- Added ActorInterestComponent that can be used to define interest queries that are more complex than a radius around the player position.
- Enabled new Development Authentication Flow
- Added new "worker" entities which are created for each server worker in a deployment so they correctly receive interest in the global state manager.
- Added support for spawning actors with ACLs configured for offloading using actor groups.
- Removed the references to the `Number of servers` slider in the Play in editor drop-down menu. The number of each server worker type to launch in PIE is now specified within the launch configuration in the `Spatial GDK Editor Settings` settings tab.
- Added `SpatialWorkerId` which is set to the worker ID when the worker associated to the `UGameInstance` connects.
- Added `USpatialStatics` helper blueprint library exposing functions for checking if SpatialOS networking is enabled, whether offloading is enabled, and more SpatialOS related checks.


### Bug fixes:
- BeginPlay is not called with authority when checking out entities from Spatial.
- Launching SpatialOS would fail if there was a space in the full directory path.
- GenerateSchemaAndSnapshots commandlet no longer runs a full schema generation for each map.
- Reliable RPC checking no longer breaks compatibility between development and shipping builds.
- Fixed an issue with schema name collisions.
- Running Schema (Full Scan) now clears generated schema files first.
- [Singleton actor's](https://docs.improbable.io/unreal/latest/content/singleton-actors#singleton-actors) authority and state now resumes correctly when reconnecting servers to snapshot.
- Retrying reliable RPCs with `UObject` arguments that were destroyed before the RPC was retried no longer causes a crash.
- Fixed path naming issues in setup.sh
- Fixed an assert/crash in `SpatialMetricsDisplay` that occurred when reloading a snapshot.
- Added Singleton and SingletonManager to query-based interest (QBI) constraints to fix issue preventing Test configuration builds from functioning correctly.
- Failing to `NetSerialize` a struct in spatial no longer causes a crash, it now prints a warning. This matches native Unreal behavior.
- Query response delegates now execute even if response status shows failure. This allows handlers to implement custom retry logic such as clients querying for the GSM.
- Fixed a crash where processing unreliable RPCs made assumption that the worker had authority over all entities in the SpatialOS op
- Ordering and reliability for single server RPCs on the same Actor are now guaranteed.

### External contributors:

In addition to all of the updates from Improbable, this release includes x improvements submitted by the incredible community of SpatialOS developers on GitHub! Thanks to these contributors:

* @cyberbibby

## [`0.4.2`](https://github.com/spatialos/UnrealGDK/releases/tag/0.4.2) - 2019-05-20

### New Known Issues:
- `BeginPlay()` is not called on all `WorldSettings` actors [#937](https://github.com/spatialos/UnrealGDK/issues/937)
- Replicated properties within `DEBUG` or `WITH_EDITORONLY_DATA` macros are not supported [#939](https://github.com/spatialos/UnrealGDK/issues/939)
- Client connections will be closed by the `ServerWorker` when using blueprint or C++ breakpoints during play-in-editor sessions [#940](https://github.com/spatialos/UnrealGDK/issues/940)
- Clients that connect after a Startup Actor (with `bNetLoadOnClient = true`) will not delete the Actor [#941](https://github.com/spatialos/UnrealGDK/issues/941)
- Generating schema while asset manager is asynchronously loading causes editor to crash [#944](https://github.com/spatialos/UnrealGDK/issues/944)

### Bug fixes:
- Adjusted dispatcher tickrate to reduce latency
- GenerateSchemaAndSnapshots commandlet no longer runs a full schema generation for each map.
- Launching SpatialOS would fail if there was a space in the full directory path.
- Fixed an issue with schema name collisions.
- Schema generation now respects "Directories to never cook".
- The editor no longer crashes during schema generation when the database is readonly.
- Replicating `UInterfaceProperty` no longer causes crashes.

## [`0.4.1`](https://github.com/spatialos/UnrealGDK/releases/tag/0.4.1) - 2019-05-01

### Bug fixes:
- Fixed an issue where schema components were sometimes generated with incorrect component IDs.

## [`0.4.0`](https://github.com/spatialos/UnrealGDK/releases/tag/0.4.0) - 2019-04-30

### Features:
- The GDK now uses SpatialOS `13.6.2`.
- Added this Changelog
- Added an error when unsupported replicated gameplay abilities are found in schema generation.
- Demoted various logs to Verbose in SpatialSender and SpatialReceiver
- You can now use the Project Settings window to pass [command line flags](https://docs.improbable.io/reference/latest/shared/spatial-cli/spatial-local-launch#spatial-local-launch) to local deployments launched from the GDK toolbar.
- You can now adjust the SpatialOS update frequency and the distance an action must move before we update its SpatialOS position.

### Bug fixes:
- The worker disconnection flow is now handled by `UEngine::OnNetworkFailure` rather than the existing `OnDisconnection` callback, which has been removed.
- Fix duplicated log messages in `spatial CLI` output when running in PIE.
- Fixed deserialization of strings from schema.
- Ensure that components added in blueprints are replicated.
- Fixed potential loading issue when attempting to load the SchemaDatabase asset.
- Add pragma once directive to header file.
- Schema files are now generated correctly for subobjects of the blueprint classes.
- Fixed being unable to launch SpatialOS if project path had spaces in it.
- Editor no longer crashes when setting LogSpatialSender to Verbose.
- Server-workers quickly restarted in the editor will connect to runtime correctly.
- Game no longer crashes when connecting to Spatial with async loading thread suspended.

## [`0.3.0`](https://github.com/spatialos/UnrealGDK/releases/tag/0.3.0) - 2019-04-04

### New Known Issues:
- Enabling Query Based Interest is needed for level streaming support, but this might affect performance in certain scenarios and is currently being investigated.
- Replicated `TimelineComponents` are not supported.

For current known issues, please visit [this](https://docs.improbable.io/unreal/alpha/known-issues) docs page

### Features:
- The default connection protocol is now TCP.
- Query Based Interest is now supported as an opt-in feature.
- Level streaming is now supported. You must enable Query Based Interest checkbox in the Runtime Settings to use level streaming.
- The GDK Toolbar now recognises when a local deployment is running, and contextually displays start and stop buttons. - (@DW-Sebastien)
- Added interface support for Unreal Engine 4.21 `UNetConnection`. - (@GeorgeR)
- Unreliable RPCs are now implemented using events instead of commands. This resolves associated performance issues.
- The `delete dynamic entities` setting now works when used in conjunction with multiple processes.
- You can now determine the type of a SpatialOS worker from within the game instance.
- Entity IDs are now reserved in batches instead of individually. This accelerates the creation of SpatialOS entities.
- You can now serialize and deserialize component data defined in external schema (schema that is not-generated by the Unreal GDK). You can use this to send and receive data, and edit snapshots.
- Improved logging during RPCs.

### Bug fixes:
- The GDK now automatically compiles all dirty blueprints before generating schema.
- Attempting to load a class which is not present in the schema database now causes the game to quit instead of crashing the entire editor.
- `Actor::ReplicateSubobjects` is now called in the replication flow. This means that Subobjects are now replicated correctly.
- Schema generation is no longer fatally halted when blueprints fail to compile.
- `AActor::TornOff` is now called when a `TearOff` event is received. This is in-line with the native implementation.
- References to objects within streaming levels, that are resolved before the level has streamed in, no longer cause defective behavior on the client.
- Attempting to replicate a `NonSpatial` actor no longer causes a crash.
- The SpatialOS Launcher now launches the correct game client, even when `UnrealCEFSubProcess.exe` is present in the assembly.
- Duplicate startup-actors are no longer created when a server-worker reconnects to a deployment.
- `BeginPlay` is no-longer called authoritatively when a server-worker reconnects to a deployment.
- Fast Array Serialization now operates correctly in conjunction with `GameplayAbilitySystem`.
- Reference parameters for RPCs are now correctly supported.
- Clients now load the map specified by the global state manager, rather than loading the `GameDefaultMap` _before_ querying the global state manager.
- Automatically generated launch configurations for deployments with a prime numbers of server-workers are now generated with the correct number of rows and columns.
- Generating schema for a level blueprint no longer deletes schema that has been generated for other levels.
- Deleting recently created actors no longer causes crashes.
- Having multiple EventGraphs no longer causes incorrect RPCs to be called.
- `TimerManager`, which is used by SpatialOS networking, is no longer subject to time dilation in the `World` instance.
- Clients no longer crash after being assigned multiple players.
- `GetWorkerFlag` can now be called from C++ classes.
- Pathless mapname arguments are now supported by the GDK commandlet.
- When `NotifyBeginPlay` is called, `BeginPlay` is no longer called on actors before their `Role` is correctly set.
- Deployments containing multiple server-workers no longer fails to initialize properly when launched through PIE with the `use single process` option unchecked.

### External contributors:

In addition to all of the updates from Improbable, this release includes 2 improvements submitted by the incredible community of SpatialOS developers on GitHub! Thanks to these contributors:

* @DW-Sebastien
* @GeorgeR

## [`0.2.0`](https://github.com/spatialos/UnrealGDK/releases/tag/0.2.0) - 2019-02-26

Startup actors revamp is merged! Snapshots are now simpler. Many bugfixes.

### New Known Issues:
- A warning about an out of date net driver is printed at startup of clients and server.

For current known issues, please visit [this](https://docs.improbable.io/unreal/alpha/known-issues) docs page

### Features:
- Actors placed in the level are no longer saved to the snapshot. They are instead spawned dynamically at the start of the game. This should fix quite a few issues such as missing references, and non-replicated instanced data being incorrectly set
- Pass player name and login options in the login URL
- Server will identify clients that have been disconnected from Spatial and trigger the cleanup on their NetConnection
- Exposed SpatialOS connection events in `USpatialNetDriver`
- Dynamic Component Ids now start from 10000, Gdk Components will now use 9999 - 0 to avoid future clashes
- Report an error during schema generation if a blueprint RPC has a "by reference" argument
- Launch configs can now be auto-generated to match the selected number of servers to launch from within the PIE editor
- Placeholder entities placed into the generated snapshot are now optional with a UI switch in the SpatialOS Settings
- Implemented updated functionality for UnrealGDKEditorCommandlet: Whenever loading a map for schema/snapshot generation, all sublevels will also be loaded before generation is started
	1. Will now loop through maps (skipping duplicates) during schema generation, to leverage the "iterative schema generation" feature
	2. Accepts an additional argument -MapPaths that can specify a collection of specific maps and/or directories (recursive) containing maps, delimited by semicolons. If not provided, defaults to "All maps in project"
	3. The paths passed in via -MapPaths are flexible

### Bug fixes:
- StartPlayInEditorGameInstance() now correctly call OnStart() on PIE_Client - (@DW-Sebastien)
- Redirect logging in the cloud to output to the correct file
- Changed type of key in `TMap` so Linux build will not give errors
- Disabled loopback of component updates
- Fix hanging on shutdown for PIE when disconnected from SpatialOS
- Fixed an issue which caused a character controller to not be destroyed when leaving the view of an observing client
- Fixed crash on multiserver PIE shutdown
- Fixed single-worker shutdown issues when launching SpatialOS through Unreal Engine 4 with Use - - Single Process unchecked in Play Options
- Fixed crash on closing client from cloud deployment
- Fix `DeleteDynamicEntities` not getting used correctly in shutdown
- Only call `BeginPlay()` on Actors if the World has begun play
- Fixed an issue with schema generation for the default GameMode
- Deleting the schema database reset the starting component ID
- Report invalid name errors during schema generation instead of when launching a deployment.
- `SchemaDatabase` can now be deleted and component ids will reset.
- `COND_InitialOnly` are only replicated once at the start
- Fixed a bug where standalone clients run locally would not connect to spatial
- Missing classes when connecting via a non-editor client
- Schema is now generated for classes that only have RPCs
- Fixed issue where properties won’t be replicated at the start of the game sometimes
- Fixed path bug when specifying snapshot output file in the settings
- Fixed up default connection flows
- Fixed issue will stale shadow data when crossing worker boundaries.
- Removed actors from replication consider list if Unreal server-worker is not authoritative over said actor
- Remove legacy flag "qos_max_unacked_pings_rate" in generated default config - (@DW-Sebastien)

### External contributors:
@DW-Sebastien

## [`0.1.0`](https://github.com/spatialos/UnrealGDK/releases/tag/0.1.0) - 2019-02-08

## Release Notes 0.1.0

Support for the new Player Auth APIs has been added and general stability improvements.

### New Known Issues:
Level streaming is currently not supported.
For other current known issues, please visit [this docs page](https://docs.improbable.io/unreal/alpha/known-issues).

### Features:
* Support for the new Player Auth APIs
* FUniqueNetId support
* Support for the new network protocol KCP
* Lazy loading of FClassInfo
* Augmented BuildWorker.bat to support additional UBT parameters
* Add IsValid() to FUnrealObjRef

### Bug fixes:
* Fixed critical errors related to Unresolved Objects
* Fixed a bug with Player State appearing to be null
* Fixed a bug related to Create Entity responses coming off the wire after a corresponding actor has been deleted
* Fixed a bug with activating actor components. We now check Initial Data for Actor components and only apply updates if `bReplicates` is true
* Fixed a bug when replicating a null list / array
* Fixed a crash with unresolved handover properties
* Changed RakNet to default network protocol temporarily to avoid performance issues with KCP
* Fixed a bug where cloud logging would not work correctly
