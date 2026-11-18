# UOX3 Control Panel

A Windows WPF control panel for running and administering a local UOX3 server.

## Features

- Start, graceful shutdown, restart, and guarded force-stop controls
- Live server output with ANSI control codes removed
- Live shard name, address, uptime, process ID, and memory usage
- One-click reload commands for INI, accounts, regions, spawns, spells, commands, definitions, JavaScript, and HTML
- World/account saves, player list, performance, memory, settings, command help, broadcasts, and custom console keys
- `uox.ini` editor with structural validation and timestamped backups
- Log browser that can safely read logs while the server is writing them
- Automatic discovery of `data/uox3.exe`, with an executable picker for standalone shard directories
- Remembers the selected shard executable between launches

UOX3 starts with its console in secure mode. The control panel sends the existing `*` key once after launch to enable its administration buttons; UOX3 records the change in the console output.

## Build and run

Install the .NET 10 SDK (or the **.NET desktop development** workload in Visual Studio 2022), then run from the repository root:

```powershell
dotnet build tools\UOX3ControlPanel\UOX3ControlPanel.csproj -c Release
dotnet run --project tools\UOX3ControlPanel\UOX3ControlPanel.csproj
```

Release builds are written directly to `tools/UOX3ControlPanel/bin/Release/`; Debug builds are written to `tools/UOX3ControlPanel/bin/Debug/`.

The small redirected-input branch in `source/cConsole.cpp` must be included in the UOX3 server build for GUI buttons to send console keys. Normal interactive console input is unchanged.
