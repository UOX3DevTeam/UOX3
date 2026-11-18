using Microsoft.Win32;
using System.Diagnostics;
using System.IO;
using System.Net.NetworkInformation;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Text.Json;
using System.Security.Cryptography;
using System.IO.Compression;
using System.Text.RegularExpressions;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Threading;
using Forms = System.Windows.Forms;
using Brush = System.Windows.Media.Brush;
using Brushes = System.Windows.Media.Brushes;
using Button = System.Windows.Controls.Button;
using ComboBoxItem = System.Windows.Controls.ComboBoxItem;
using Color = System.Windows.Media.Color;
using ColorConverter = System.Windows.Media.ColorConverter;
using MessageBox = System.Windows.MessageBox;
using OpenFileDialog = Microsoft.Win32.OpenFileDialog;
using TextBlock = System.Windows.Controls.TextBlock;

namespace UOX3ControlPanel;

public partial class MainWindow : Window
{
    private Process? _server;
    private int _handlingServerExit;
    private string? _executablePath;
    private readonly StringBuilder _pendingOutput = new();
    private readonly object _outputLock = new();
    private bool _outputScheduled;
    private DateTime? _startedAt;
    private bool _consoleUnlocked;
    private string _consoleProbe = string.Empty;
    private readonly DispatcherTimer _statusTimer;
    private readonly Forms.NotifyIcon _trayIcon;
    private readonly Forms.ToolStripMenuItem _trayShowItem;
    private readonly Forms.ToolStripMenuItem _trayStartItem;
    private readonly Forms.ToolStripMenuItem _trayShutdownItem;
    private bool _allowExit;
    private bool _shownTrayHint;
    private Mutex? _portMutex;
    private int? _reservedPort;
    private string _shardName = "UOX3 Shard";
    private readonly List<UpdateRecord> _availableUpdates = new();
    private static readonly string PreferencesPath = Path.Combine(
        Environment.GetFolderPath(Environment.SpecialFolder.LocalApplicationData), "UOX3ControlPanel", "settings.json");
    private readonly TerminalOutputParser _terminalParser = new();
    private Paragraph _consoleParagraph = null!;

    public MainWindow()
    {
        InitializeComponent();
        ConsoleText.Document.Blocks.Clear();
        _consoleParagraph = new Paragraph { Margin = new Thickness(0) };
        ConsoleText.Document.Blocks.Add(_consoleParagraph);
        _executablePath = LoadSavedExecutable() ?? FindDefaultExecutable();
        _statusTimer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1) };
        _statusTimer.Tick += (_, _) => UpdateRuntimeDetails();
        _statusTimer.Start();

        _trayStartItem = new Forms.ToolStripMenuItem("Start server", null, (_, _) => Dispatcher.Invoke(() => Start_Click(this, new RoutedEventArgs())));
        _trayShutdownItem = new Forms.ToolStripMenuItem("Graceful shutdown", null, (_, _) => Dispatcher.Invoke(() => SendKey('q')));
        _trayShowItem = new Forms.ToolStripMenuItem("Show UOX3 Shard", null, (_, _) => Dispatcher.Invoke(ShowFromTray))
        {
            Font = new System.Drawing.Font(Forms.Control.DefaultFont, System.Drawing.FontStyle.Bold)
        };
        var trayMenu = new Forms.ContextMenuStrip();
        trayMenu.Items.Add(_trayShowItem);
        trayMenu.Items.Add(new Forms.ToolStripSeparator());
        trayMenu.Items.Add(_trayStartItem);
        trayMenu.Items.Add(_trayShutdownItem);
        trayMenu.Items.Add(new Forms.ToolStripSeparator());
        trayMenu.Items.Add(new Forms.ToolStripMenuItem("Exit", null, async (_, _) => await Dispatcher.InvokeAsync(ExitApplicationAsync)));
        _trayIcon = new Forms.NotifyIcon
        {
            Icon = System.Drawing.SystemIcons.Application,
            Text = "UOX3 Control Panel",
            ContextMenuStrip = trayMenu,
            Visible = true
        };
        _trayIcon.DoubleClick += (_, _) => Dispatcher.Invoke(ShowFromTray);
        StateChanged += MainWindow_StateChanged;
        UpdatePathDisplay();
        LoadConfiguration();
        RefreshLogs();
        _ = RefreshNetworkStatusAsync();
        _ = RefreshSecurityAuditAsync();
        Closing += MainWindow_Closing;
    }

    private string? ServerDirectory => _executablePath is null ? null : Path.GetDirectoryName(_executablePath);
    private string? ConfigPath => ServerDirectory is null ? null : Path.Combine(ServerDirectory, "uox.ini");

    private static string? FindDefaultExecutable()
    {
        var candidates = new List<string> { Path.Combine(Environment.CurrentDirectory, "data", "uox3.exe") };
        var cursor = new DirectoryInfo(AppContext.BaseDirectory);
        for (var i = 0; cursor is not null && i < 9; i++, cursor = cursor.Parent)
        {
            candidates.Add(Path.Combine(cursor.FullName, "data", "uox3.exe"));
            candidates.Add(Path.Combine(cursor.FullName, "uox3.exe"));
        }
        return candidates.FirstOrDefault(File.Exists);
    }

    private static string? LoadSavedExecutable()
    {
        try
        {
            if (!File.Exists(PreferencesPath)) return null;
            var settings = JsonSerializer.Deserialize<ControlPanelSettings>(File.ReadAllText(PreferencesPath));
            return settings?.ExecutablePath is { } path && File.Exists(path) ? path : null;
        }
        catch { return null; }
    }

    private void SavePreferences()
    {
        try
        {
            Directory.CreateDirectory(Path.GetDirectoryName(PreferencesPath)!);
            File.WriteAllText(PreferencesPath, JsonSerializer.Serialize(new ControlPanelSettings(_executablePath)));
        }
        catch { /* Preferences are optional; server administration must still work. */ }
    }

    private void UpdatePathDisplay()
    {
        StartButton.IsEnabled = _server is null && _executablePath is not null && File.Exists(_executablePath);
    }

    private async void Start_Click(object sender, RoutedEventArgs e)
    {
        if (_server is not null || _executablePath is null || !File.Exists(_executablePath)) return;
        if (!TryReserveServerPort(out var portError))
        {
            MessageBox.Show(this, portError, "UOX3 port is already in use", MessageBoxButton.OK, MessageBoxImage.Warning);
            FooterText.Text = portError;
            return;
        }
        try
        {
            var startInfo = new ProcessStartInfo(_executablePath)
            {
                WorkingDirectory = Path.GetDirectoryName(_executablePath)!,
                UseShellExecute = false,
                CreateNoWindow = true,
                RedirectStandardInput = true,
                RedirectStandardOutput = true,
                RedirectStandardError = true,
                StandardOutputEncoding = Encoding.UTF8,
                StandardErrorEncoding = Encoding.UTF8
            };
            _server = new Process { StartInfo = startInfo, EnableRaisingEvents = true };
            Interlocked.Exchange(ref _handlingServerExit, 0);
            _server.Exited += Server_Exited;
            _server.Start();
            _startedAt = DateTime.Now;
            _consoleUnlocked = false;
            _consoleProbe = string.Empty;
            SetRunningState(true);
            AppendConsole($"[Control Panel] Started PID {_server.Id}{Environment.NewLine}");
            _ = PumpOutputAsync(_server.StandardOutput);
            _ = PumpOutputAsync(_server.StandardError);
            // UOX3 intentionally starts with keyboard commands locked. This
            // control panel is the interactive console, so unlock them once.
            SendKey('*');
            _ = VerifyConsoleInputAsync(_server);
            await Task.CompletedTask;
        }
        catch (Exception ex)
        {
            _server?.Dispose();
            _server = null;
            ReleaseServerPort();
            SetRunningState(false);
            MessageBox.Show(this, ex.Message, "Unable to start UOX3", MessageBoxButton.OK, MessageBoxImage.Error);
        }
    }

    private async Task PumpOutputAsync(StreamReader reader)
    {
        var buffer = new char[2048];
        try
        {
            while (true)
            {
                var count = await reader.ReadAsync(buffer);
                if (count == 0) break;
                string parsed;
                lock (_terminalParser)
                    parsed = _terminalParser.Process(buffer.AsSpan(0, count));
                if (parsed.Length > 0) AppendConsole(parsed);
            }
        }
        catch (ObjectDisposedException) { }
        catch (IOException) { }
    }

    private void AppendConsole(string text)
    {
        lock (_outputLock)
        {
            _pendingOutput.Append(text);
            if (_outputScheduled) return;
            _outputScheduled = true;
        }
        Dispatcher.BeginInvoke(() =>
        {
            string addition;
            lock (_outputLock)
            {
                addition = _pendingOutput.ToString();
                _pendingOutput.Clear();
                _outputScheduled = false;
            }
            AppendColoredConsoleText(addition);
            ObserveConsoleOutput(addition);
            ConsoleText.ScrollToEnd();
        });
    }

    private void AppendColoredConsoleText(string text)
    {
        var parts = Regex.Split(text, @"(?i)(\[done\]|\[failed\]|\[skipped\]|\b\d+(?=\s+entries\b))");
        foreach (var part in parts)
        {
            if (part.Length == 0) continue;
            if (part.Equals("[done]", StringComparison.OrdinalIgnoreCase))
            {
                AppendStatusWord("done", Brushes.LimeGreen);
                continue;
            }
            if (part.Equals("[failed]", StringComparison.OrdinalIgnoreCase))
            {
                AppendStatusWord("failed", Brushes.OrangeRed);
                continue;
            }
            if (part.Equals("[skipped]", StringComparison.OrdinalIgnoreCase))
            {
                AppendStatusWord("skipped", Brushes.Gold);
                continue;
            }

            var brush = Brushes.LightGray;
            if (part.All(char.IsDigit)) brush = Brushes.Khaki;
            else if (part.Contains("ERROR:", StringComparison.OrdinalIgnoreCase)) brush = Brushes.OrangeRed;
            else if (part.Contains("WARNING:", StringComparison.OrdinalIgnoreCase)) brush = Brushes.DodgerBlue;
            else if (part.TrimStart().StartsWith('o') && part.Contains("---")) brush = Brushes.White;
            _consoleParagraph.Inlines.Add(new Run(part) { Foreground = brush });
        }
    }

    private void AppendStatusWord(string word, System.Windows.Media.Brush wordBrush)
    {
        var bracketBrush = new SolidColorBrush(Color.FromRgb(72, 139, 255));
        _consoleParagraph.Inlines.Add(new Run("[") { Foreground = bracketBrush });
        _consoleParagraph.Inlines.Add(new Run(word) { Foreground = wordBrush });
        _consoleParagraph.Inlines.Add(new Run("]") { Foreground = bracketBrush });
    }

    private void ObserveConsoleOutput(string text)
    {
        _consoleProbe = (_consoleProbe + text);
        if (_consoleProbe.Length > 2000) _consoleProbe = _consoleProbe[^2000..];
        if (!_consoleUnlocked && _consoleProbe.Contains("Secure mode disabled", StringComparison.OrdinalIgnoreCase))
        {
            _consoleUnlocked = true;
            ShutdownButton.IsEnabled = true;
            RestartButton.IsEnabled = true;
            _trayShutdownItem.Enabled = true;
            FooterText.Text = "Console connected — administration commands are ready";
        }
    }

    private async Task VerifyConsoleInputAsync(Process process)
    {
        await Task.Delay(TimeSpan.FromSeconds(12));
        await Dispatcher.InvokeAsync(() =>
        {
            if (!ReferenceEquals(_server, process) || process.HasExited || _consoleUnlocked) return;
            ShutdownButton.IsEnabled = false;
            RestartButton.IsEnabled = false;
            _trayShutdownItem.Enabled = false;
            FooterText.Text = "Console input unavailable — select a UOX3 build with GUI pipe support";
            AppendConsole($"{Environment.NewLine}[Control Panel] ERROR: UOX3 did not acknowledge console input. " +
                          $"Rebuild the server with the redirected-input change in cConsole.cpp.{Environment.NewLine}");
        });
    }

    private void Server_Exited(object? sender, EventArgs e)
    {
        if (Interlocked.Exchange(ref _handlingServerExit, 1) != 0) return;
        var process = sender as Process;
        var exitCode = 0;
        try { exitCode = process?.ExitCode ?? 0; } catch { }
        Dispatcher.BeginInvoke(() =>
        {
            AppendConsole($"{Environment.NewLine}[Control Panel] Server exited with code {exitCode}.{Environment.NewLine}");
            if (ReferenceEquals(_server, process)) _server = null;
            process?.Dispose();
            ReleaseServerPort();
            SetRunningState(false);
            RefreshLogs();
        });
    }

    private bool TryReserveServerPort(out string error)
    {
        error = string.Empty;
        var port = ReadConfiguredPort();
        if (port is null)
        {
            error = "The PORT setting in uox.ini is missing or invalid.";
            return false;
        }

        try
        {
            if (IPGlobalProperties.GetIPGlobalProperties().GetActiveTcpListeners().Any(endpoint => endpoint.Port == port.Value))
            {
                error = $"TCP port {port.Value} is already being used. Choose a different PORT in uox.ini or stop the other server.";
                return false;
            }

            var mutex = new Mutex(false, $@"Local\UOX3ControlPanel-Port-{port.Value}");
            var acquired = false;
            try { acquired = mutex.WaitOne(0); }
            catch (AbandonedMutexException) { acquired = true; }
            if (!acquired)
            {
                mutex.Dispose();
                error = $"Another UOX3 Control Panel is already starting or running a server on port {port.Value}.";
                return false;
            }

            _portMutex = mutex;
            _reservedPort = port;
            return true;
        }
        catch (Exception ex)
        {
            error = $"Could not verify port {port.Value}: {ex.Message}";
            return false;
        }
    }

    private int? ReadConfiguredPort()
    {
        try
        {
            if (ConfigPath is null || !File.Exists(ConfigPath)) return null;
            var match = Regex.Match(File.ReadAllText(ConfigPath), @"(?im)^\s*PORT\s*=\s*(\d+)\s*$");
            return match.Success && int.TryParse(match.Groups[1].Value, out var port) && port is >= 1 and <= 65535 ? port : null;
        }
        catch { return null; }
    }

    private void ReleaseServerPort()
    {
        var mutex = _portMutex;
        _portMutex = null;
        _reservedPort = null;
        if (mutex is null) return;
        try { mutex.ReleaseMutex(); } catch (ApplicationException) { }
        mutex.Dispose();
    }

    private void SetRunningState(bool running)
    {
        StartButton.IsEnabled = !running && _executablePath is not null;
        ShutdownButton.IsEnabled = running && _consoleUnlocked;
        RestartButton.IsEnabled = running && _consoleUnlocked;
        ForceStopButton.IsEnabled = running;
        InstallFullUpdateButton.IsEnabled = !running && _availableUpdates.Any(update => !update.AlreadyCurrent);
        StatusText.Text = running ? "Running" : "Stopped";
        StatusDot.Fill = new SolidColorBrush((Color)ColorConverter.ConvertFromString(running ? "#4BC27A" : "#708090"));
        FooterText.Text = running ? "UOX3 is running" : "UOX3 is stopped";
        _trayStartItem.Enabled = !running && _executablePath is not null;
        _trayShutdownItem.Enabled = running && _consoleUnlocked;
        UpdateShardIdentity(running);
        if (!running) _startedAt = null;
        UpdateRuntimeDetails();
    }

    private void UpdateRuntimeDetails()
    {
        var process = _server;
        if (process is null)
        {
            UptimeText.Text = "—";
            ProcessText.Text = "Not running";
            return;
        }
        var elapsed = DateTime.Now - (_startedAt ?? DateTime.Now);
        UptimeText.Text = elapsed.TotalDays >= 1
            ? $"{(int)elapsed.TotalDays}d {elapsed:hh\\:mm\\:ss}"
            : elapsed.ToString(@"hh\:mm\:ss");
        try { ProcessText.Text = $"PID {process.Id}  •  {process.WorkingSet64 / 1048576:N0} MB"; }
        catch { ProcessText.Text = "Stopping…"; }
    }

    private void SendKey(char key)
    {
        try
        {
            if (_server is null || _server.HasExited) return;
            if (key != '*' && !_consoleUnlocked)
            {
                FooterText.Text = "Command not sent — waiting for the server console connection";
                return;
            }
            _server.StandardInput.Write(key);
            _server.StandardInput.Flush();
            FooterText.Text = $"Sent console key '{key}'";
        }
        catch (Exception ex) { FooterText.Text = $"Could not send command: {ex.Message}"; }
    }

    private void Shutdown_Click(object sender, RoutedEventArgs e) => SendKey('q');
    private void Restart_Click(object sender, RoutedEventArgs e) => SendKey('r');
    private void SendKey_Click(object sender, RoutedEventArgs e)
    {
        if (sender is Button { Tag: string value } && value.Length == 1) SendKey(value[0]);
    }

    private async void ForceStop_Click(object sender, RoutedEventArgs e)
    {
        var process = _server;
        if (process is null) return;
        if (MessageBox.Show(this, "Force stopping can interrupt a world save. Stop UOX3 immediately?", "Force stop",
                MessageBoxButton.YesNo, MessageBoxImage.Warning) == MessageBoxResult.Yes)
        {
            ForceStopButton.IsEnabled = false;
            ShutdownButton.IsEnabled = false;
            RestartButton.IsEnabled = false;
            FooterText.Text = "Stopping UOX3…";
            try
            {
                if (!process.HasExited)
                {
                    process.Kill(entireProcessTree: true);
                    await process.WaitForExitAsync();
                }
            }
            catch (InvalidOperationException)
            {
                // The process exited between the status check and Kill().
            }
            catch (Exception ex)
            {
                if (ReferenceEquals(_server, process)) SetRunningState(true);
                MessageBox.Show(this, ex.Message, "Force stop failed", MessageBoxButton.OK, MessageBoxImage.Error);
            }
        }
    }

    private void ChooseExecutable_Click(object sender, RoutedEventArgs e)
    {
        var dialog = new OpenFileDialog { Filter = "UOX3 server (uox3.exe)|uox3.exe|Executable files (*.exe)|*.exe", CheckFileExists = true };
        if (dialog.ShowDialog(this) != true) return;
        _executablePath = dialog.FileName;
        SavePreferences();
        UpdatePathDisplay();
        LoadConfiguration();
        RefreshLogs();
        _ = RefreshNetworkStatusAsync();
    }

    private void LoadConfig_Click(object sender, RoutedEventArgs e) => LoadConfiguration();

    private void LoadConfiguration()
    {
        try
        {
            if (ConfigPath is null || !File.Exists(ConfigPath))
            {
                ConfigText.Text = string.Empty;
                ConfigStatusText.Text = "uox.ini was not found beside the selected executable.";
                return;
            }
            ConfigText.Text = File.ReadAllText(ConfigPath);
            RefreshIniGuide();
            UpdateShardDetails(ConfigText.Text);
            ConfigStatusText.Text = $"Loaded {ConfigPath}";
            _ = RefreshNetworkStatusAsync();
            _ = RefreshSecurityAuditAsync();
        }
        catch (Exception ex) { ConfigStatusText.Text = ex.Message; }
    }

    private void UpdateShardDetails(string configuration)
    {
        static string ReadValue(string text, string key)
        {
            var match = Regex.Match(text, $@"(?im)^\s*{Regex.Escape(key)}\s*=\s*(.*?)\s*$");
            return match.Success ? match.Groups[1].Value.Trim() : "—";
        }
        var shardName = ReadValue(configuration, "SERVERNAME");
        ShardNameText.Text = shardName;
        _shardName = shardName == "—" ? "UOX3 Shard" : shardName;
        UpdateShardIdentity(_server is not null);
        var ip = ReadValue(configuration, "EXTERNALIP");
        var port = ReadValue(configuration, "PORT");
        AddressText.Text = ip == "—" ? "—" : $"{ip}:{port}";
    }

    private void UpdateShardIdentity(bool running)
    {
        Title = $"{_shardName} — UOX3 Control Panel";
        _trayShowItem.Text = $"Show {_shardName}";
        var state = running ? "running" : "stopped";
        var tooltip = $"{_shardName} — {state}";
        _trayIcon.Text = tooltip.Length <= 63 ? tooltip : tooltip[..63];
    }

    private void SaveConfig_Click(object sender, RoutedEventArgs e)
    {
        if (ConfigPath is null) return;
        var validationError = ValidateConfig(ConfigText.Text);
        if (validationError is not null)
        {
            MessageBox.Show(this, validationError, "Configuration not saved", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        try
        {
            var backup = $"{ConfigPath}.{DateTime.Now:yyyyMMdd-HHmmss}.bak";
            if (File.Exists(ConfigPath)) File.Copy(ConfigPath, backup, false);
            var temporary = ConfigPath + ".tmp";
            File.WriteAllText(temporary, ConfigText.Text, new UTF8Encoding(false));
            File.Move(temporary, ConfigPath, true);
            UpdateShardDetails(ConfigText.Text);
            _ = RefreshNetworkStatusAsync();
            _ = RefreshSecurityAuditAsync();
            ConfigStatusText.Text = $"Saved. Backup: {Path.GetFileName(backup)}";
            FooterText.Text = "Configuration saved; use Reload INI to apply supported settings.";
        }
        catch (Exception ex) { MessageBox.Show(this, ex.Message, "Could not save configuration", MessageBoxButton.OK, MessageBoxImage.Error); }
    }

    private void ConfigText_TextChanged(object sender, TextChangedEventArgs e) => RefreshIniGuide();

    private void IniSearch_TextChanged(object sender, TextChangedEventArgs e) => RefreshIniGuide();

    private void RefreshIniGuide()
    {
        if (IniSettingList is null || ConfigText is null) return;
        var selectedName = (IniSettingList.SelectedItem as IniSettingEntry)?.Name;
        var query = IniSearchText?.Text.Trim() ?? string.Empty;
        var entries = ParseIniSettings(ConfigText.Text)
            .Where(entry => query.Length == 0
                || entry.Name.Contains(query, StringComparison.OrdinalIgnoreCase)
                || entry.Section.Contains(query, StringComparison.OrdinalIgnoreCase)
                || entry.Description.Contains(query, StringComparison.OrdinalIgnoreCase))
            .OrderBy(entry => entry.Section).ThenBy(entry => entry.Name).ToList();
        IniSettingList.ItemsSource = entries;
        if (selectedName is not null) IniSettingList.SelectedItem = entries.FirstOrDefault(entry => entry.Name == selectedName);
        if (IniSettingList.SelectedItem is null && entries.Count > 0) IniSettingList.SelectedIndex = 0;
    }

    private static List<IniSettingEntry> ParseIniSettings(string text)
    {
        var entries = new List<IniSettingEntry>();
        var section = "General";
        var lines = text.Replace("\r", string.Empty).Split('\n');
        for (var lineNumber = 0; lineNumber < lines.Length; lineNumber++)
        {
            var line = lines[lineNumber].Trim();
            if (line.StartsWith('[') && line.EndsWith(']')) { section = line[1..^1]; continue; }
            var equals = line.IndexOf('=');
            if (equals <= 0 || line.StartsWith("//")) continue;
            var name = line[..equals].Trim().ToUpperInvariant();
            var value = line[(equals + 1)..].Trim();
            var help = IniHelpFor(name, section, value);
            entries.Add(new IniSettingEntry(name, value, section, lineNumber, help.Description, help.DefaultValue, help.ApplyNote));
        }
        return entries;
    }

    private static IniHelp IniHelpFor(string name, string section, string value)
    {
        if (IniHelpCatalog.TryGetValue(name, out var known)) return known;
        var readable = Regex.Replace(name, "([A-Z])([A-Z][a-z])", "$1 $2").Replace('_', ' ').ToLowerInvariant();
        var description = value is "0" or "1"
            ? $"Enables or disables {readable}. 0 = disabled; 1 = enabled."
            : name.EndsWith("TIMER", StringComparison.OrdinalIgnoreCase) || name.Contains("TIMEOUT", StringComparison.OrdinalIgnoreCase)
                ? $"Controls the {readable} timing value, usually measured in seconds."
                : name.EndsWith("DIRECTORY", StringComparison.OrdinalIgnoreCase)
                    ? $"Filesystem location used for {readable.Replace("directory", "data").Trim()}."
                    : $"UOX3 setting for {readable} in the [{section}] section.";
        return new IniHelp(description, "UOX3 generated default", "Save the file, then reload INI or restart if the running server does not adopt it.");
    }

    private void IniSettingList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (IniSettingList.SelectedItem is not IniSettingEntry entry)
        {
            IniNameText.Text = IniSectionText.Text = IniCurrentValueText.Text = IniDefaultValueText.Text = IniDescriptionText.Text = IniApplyText.Text = string.Empty;
            return;
        }
        IniNameText.Text = entry.Name;
        IniSectionText.Text = $"[{entry.Section}]";
        IniCurrentValueText.Text = entry.Value;
        IniDefaultValueText.Text = entry.DefaultValue;
        IniDescriptionText.Text = entry.Description;
        IniApplyText.Text = entry.ApplyNote;
    }

    private void JumpToIniSetting_Click(object sender, RoutedEventArgs e)
    {
        if (IniSettingList.SelectedItem is not IniSettingEntry entry) return;
        var start = ConfigText.GetCharacterIndexFromLineIndex(entry.LineNumber);
        if (start < 0) return;
        var lineLength = ConfigText.GetLineLength(entry.LineNumber);
        ConfigText.Focus();
        ConfigText.Select(start, Math.Max(0, lineLength));
        ConfigText.ScrollToLine(entry.LineNumber);
    }

    private static string? ValidateConfig(string text)
    {
        var depth = 0;
        var sectionCount = 0;
        var lineNumber = 0;
        foreach (var raw in text.Replace("\r", string.Empty).Split('\n'))
        {
            lineNumber++;
            var line = raw.Trim();
            if (line.Length == 0 || line.StartsWith("//")) continue;
            if (line.StartsWith('[') && line.EndsWith(']')) { sectionCount++; continue; }
            if (line == "{") { depth++; continue; }
            if (line == "}") { if (--depth < 0) return $"Unexpected closing brace on line {lineNumber}."; continue; }
            if (depth > 0 && !line.Contains('=')) return $"Expected KEY=VALUE on line {lineNumber}.";
        }
        if (depth != 0) return "The configuration has unbalanced braces.";
        if (sectionCount == 0) return "No [sections] were found in the configuration.";
        return null;
    }

    private void RefreshLogs_Click(object sender, RoutedEventArgs e) => RefreshLogs();
    private void RefreshLogs()
    {
        LogList.Items.Clear();
        var logDirectory = ServerDirectory is null ? null : Path.Combine(ServerDirectory, "logs");
        if (logDirectory is null || !Directory.Exists(logDirectory)) return;
        foreach (var file in Directory.EnumerateFiles(logDirectory).OrderByDescending(File.GetLastWriteTime))
            LogList.Items.Add(new FileInfo(file));
        LogList.DisplayMemberPath = nameof(FileInfo.Name);
    }

    private void LogList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        if (LogList.SelectedItem is not FileInfo file) return;
        try
        {
            const int maximum = 2_000_000;
            using var stream = new FileStream(file.FullName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
            if (stream.Length > maximum) stream.Seek(-maximum, SeekOrigin.End);
            using var reader = new StreamReader(stream, detectEncodingFromByteOrderMarks: true);
            LogText.Text = (stream.Position > 0 ? "[Showing the last 2 MB]\n" : string.Empty) + reader.ReadToEnd();
            LogText.ScrollToEnd();
        }
        catch (Exception ex) { LogText.Text = ex.Message; }
    }

    private void OpenServerFolder_Click(object sender, RoutedEventArgs e) => OpenFolder(ServerDirectory);
    private void OpenLogsFolder_Click(object sender, RoutedEventArgs e) => OpenFolder(ServerDirectory is null ? null : Path.Combine(ServerDirectory, "logs"));
    private static void OpenFolder(string? path)
    {
        if (path is not null && Directory.Exists(path))
            Process.Start(new ProcessStartInfo("explorer.exe", path) { UseShellExecute = true });
    }

    private void ClearConsole_Click(object sender, RoutedEventArgs e)
    {
        _consoleParagraph.Inlines.Clear();
    }

    private async void RefreshNetwork_Click(object sender, RoutedEventArgs e) => await RefreshNetworkStatusAsync();

    private async Task RefreshNetworkStatusAsync()
    {
        var port = ReadConfiguredPort();
        var lanAddress = GetLanAddress();
        NetworkPortText.Text = port?.ToString() ?? "Invalid or missing PORT setting";
        LanAddressText.Text = lanAddress ?? "No active IPv4 LAN address found";
        RouterInstructionsText.Text = port is null || lanAddress is null
            ? "A valid PORT and active LAN address are required."
            : $"Forward external TCP {port.Value} to {lanAddress}:{port.Value} (internal TCP).";

        if (port is null)
        {
            ListenerStatusText.Text = "Unavailable";
            FirewallStatusText.Text = "Unavailable";
            return;
        }

        try
        {
            var listening = IPGlobalProperties.GetIPGlobalProperties().GetActiveTcpListeners().Any(endpoint => endpoint.Port == port.Value);
            ListenerStatusText.Text = listening ? $"Listening on TCP {port.Value}" : $"Not listening on TCP {port.Value}";
            ListenerStatusText.Foreground = listening ? Brushes.LimeGreen : Brushes.Gold;
        }
        catch (Exception ex) { ListenerStatusText.Text = $"Check failed: {ex.Message}"; }

        FirewallStatusText.Text = "Checking…";
        try
        {
            var ruleName = GetFirewallRuleName(port.Value);
            var script = $"$r=Get-NetFirewallRule -DisplayName '{EscapePowerShell(ruleName)}' -ErrorAction SilentlyContinue | Where-Object {{$_.Enabled -eq 'True' -and $_.Direction -eq 'Inbound' -and $_.Action -eq 'Allow'}}; if($r){{'ALLOWED'}}else{{'MISSING'}}";
            var result = await RunPowerShellCaptureAsync(script);
            var allowed = result.Contains("ALLOWED", StringComparison.OrdinalIgnoreCase);
            FirewallStatusText.Text = allowed ? "Inbound allow rule installed" : "No control-panel firewall rule found";
            FirewallStatusText.Foreground = allowed ? Brushes.LimeGreen : Brushes.Gold;
        }
        catch (Exception ex)
        {
            FirewallStatusText.Text = $"Check failed: {ex.Message}";
            FirewallStatusText.Foreground = Brushes.OrangeRed;
        }
    }

    private async void AddFirewallRule_Click(object sender, RoutedEventArgs e)
    {
        var port = ReadConfiguredPort();
        if (port is null || _executablePath is null) return;
        var ruleName = GetFirewallRuleName(port.Value);
        var shardRulePattern = $"UOX3 - {EscapePowerShell(_shardName)} - TCP *";
        var script = $"Get-NetFirewallRule -DisplayName '{shardRulePattern}' -ErrorAction SilentlyContinue | Remove-NetFirewallRule; " +
                     $"New-NetFirewallRule -DisplayName '{EscapePowerShell(ruleName)}' -Group 'UOX3 Control Panel' -Direction Inbound -Action Allow -Enabled True -Profile Any -Protocol TCP -LocalPort {port.Value} -Program '{EscapePowerShell(_executablePath)}' | Out-Null";
        if (await RunElevatedPowerShellAsync(script))
        {
            FooterText.Text = $"Windows Firewall now allows {_shardName} on TCP {port.Value}";
            await RefreshNetworkStatusAsync();
        }
    }

    private async void RemoveFirewallRule_Click(object sender, RoutedEventArgs e)
    {
        var port = ReadConfiguredPort();
        if (port is null) return;
        var shardRulePattern = $"UOX3 - {EscapePowerShell(_shardName)} - TCP *";
        var script = $"Get-NetFirewallRule -DisplayName '{shardRulePattern}' -ErrorAction SilentlyContinue | Remove-NetFirewallRule";
        if (await RunElevatedPowerShellAsync(script))
        {
            FooterText.Text = $"Removed the Windows Firewall rule for TCP {port.Value}";
            await RefreshNetworkStatusAsync();
        }
    }

    private static string? GetLanAddress()
    {
        return NetworkInterface.GetAllNetworkInterfaces()
            .Where(adapter => adapter.OperationalStatus == OperationalStatus.Up && adapter.NetworkInterfaceType != NetworkInterfaceType.Loopback)
            .SelectMany(adapter => adapter.GetIPProperties().UnicastAddresses)
            .Where(address => address.Address.AddressFamily == AddressFamily.InterNetwork && !IPAddress.IsLoopback(address.Address))
            .Select(address => address.Address.ToString())
            .FirstOrDefault();
    }

    private string GetFirewallRuleName(int port) => $"UOX3 - {_shardName} - TCP {port}";
    private static string EscapePowerShell(string value) => value.Replace("'", "''");

    private static async Task<string> RunPowerShellCaptureAsync(string script)
    {
        var startInfo = new ProcessStartInfo("powershell.exe")
        {
            UseShellExecute = false,
            CreateNoWindow = true,
            RedirectStandardOutput = true,
            RedirectStandardError = true
        };
        startInfo.ArgumentList.Add("-NoProfile");
        startInfo.ArgumentList.Add("-NonInteractive");
        startInfo.ArgumentList.Add("-Command");
        startInfo.ArgumentList.Add(script);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start PowerShell.");
        var output = await process.StandardOutput.ReadToEndAsync();
        var error = await process.StandardError.ReadToEndAsync();
        await process.WaitForExitAsync();
        if (process.ExitCode != 0) throw new InvalidOperationException(error.Trim());
        return output.Trim();
    }

    private async Task<bool> RunElevatedPowerShellAsync(string script)
    {
        try
        {
            var startInfo = new ProcessStartInfo("powershell.exe") { UseShellExecute = true, Verb = "runas", WindowStyle = ProcessWindowStyle.Hidden };
            startInfo.ArgumentList.Add("-NoProfile");
            startInfo.ArgumentList.Add("-NonInteractive");
            startInfo.ArgumentList.Add("-Command");
            startInfo.ArgumentList.Add(script);
            using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start elevated PowerShell.");
            await process.WaitForExitAsync();
            if (process.ExitCode == 0) return true;
            MessageBox.Show(this, "Windows did not apply the firewall change.", "Firewall change failed", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        catch (System.ComponentModel.Win32Exception ex) when (ex.NativeErrorCode == 1223)
        {
            FooterText.Text = "Firewall change canceled";
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, ex.Message, "Firewall change failed", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        return false;
    }

    private async void CheckUpdates_Click(object sender, RoutedEventArgs e) => await CheckForUpdatesAsync();

    private async Task CheckForUpdatesAsync()
    {
        var repository = FindRepositoryRoot();
        if (repository is null)
        {
            UpdateSummaryText.Text = "No Git repository was found above the selected shard directory.";
            return;
        }
        var branch = SelectedUpdateBranch();
        UpdateSummaryText.Text = $"Fetching official upstream/{branch}…";
        InstallDataUpdatesButton.IsEnabled = false;
        InstallFullUpdateButton.IsEnabled = false;
        UpdateList.Items.Clear();
        _availableUpdates.Clear();
        try
        {
            await RunGitAsync(repository, "fetch", "--prune", "upstream");
            var baseline = ReadDeployedRevision() ?? "HEAD";
            var upstreamRevision = (await RunGitAsync(repository, "rev-parse", $"upstream/{branch}")).Trim();
            var resolutions = LoadMergeResolutions();
            var diff = await RunGitAsync(repository, "diff", "--name-status", $"{baseline}..upstream/{branch}");
            foreach (var line in diff.Split('\n', StringSplitOptions.RemoveEmptyEntries))
            {
                var columns = line.TrimEnd('\r').Split('\t');
                if (columns.Length < 2) continue;
                var status = columns[0][0].ToString();
                var path = columns[^1].Replace('\\', '/');
                var category = CategorizeUpdate(path);
                var alreadyCurrent = status != "D" && await GitDiffIsCleanAsync(repository, $"upstream/{branch}", path);
                var cleanAgainstDeployed = await GitDiffIsCleanAsync(repository, baseline, path);
                var locallyChanged = !cleanAgainstDeployed && !alreadyCurrent;
                var safe = !locallyChanged && status != "D";
                var target = SafeRepositoryPath(repository, path);
                var resolved = locallyChanged && resolutions.TryGetValue(path, out var resolution)
                    && resolution.UpstreamRevision.Equals(upstreamRevision, StringComparison.OrdinalIgnoreCase)
                    && File.Exists(target)
                    && resolution.LocalSha256.Equals(FileSha256(target), StringComparison.OrdinalIgnoreCase);
                _availableUpdates.Add(new UpdateRecord(path, status, category, safe, alreadyCurrent, resolved));
            }
            foreach (var update in _availableUpdates) UpdateList.Items.Add(update);
            var pending = _availableUpdates.Count(update => !update.AlreadyCurrent);
            var safeData = _availableUpdates.Count(update => update.IsDataFile && update.Safe && !update.AlreadyCurrent);
            var conflicts = _availableUpdates.Count(update => !update.Safe && !update.Resolved && update.Status != "D" && !update.AlreadyCurrent);
            var core = _availableUpdates.Count(update => update.Category == "Core" && !update.AlreadyCurrent);
            UpdateSummaryText.Text = core > 0
                ? $"upstream/{branch}: {pending} pending change(s) — {safeData} JS/DFN, {conflicts} local conflict(s), {core} core change(s). Data-only installation is blocked until the matching executable is rebuilt."
                : $"upstream/{branch}: {pending} pending change(s) — {safeData} safe JS/DFN, {conflicts} local conflict(s), no core rebuild required.";
            InstallDataUpdatesButton.IsEnabled = safeData > 0 && core == 0;
            InstallFullUpdateButton.IsEnabled = pending > 0 && conflicts == 0 && _server is null;
        }
        catch (Exception ex)
        {
            UpdateSummaryText.Text = $"Update check failed: {ex.Message}";
        }
    }

    private async void InstallFullUpdate_Click(object sender, RoutedEventArgs e)
    {
        if (_server is not null)
        {
            MessageBox.Show(this, "Gracefully stop UOX3 before installing a full update.", "Server must be stopped", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        var repository = FindRepositoryRoot();
        if (repository is null || _executablePath is null) return;
        var repositoryRoot = Path.GetFullPath(repository).TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
        var executable = Path.GetFullPath(_executablePath);
        if (!executable.StartsWith(repositoryRoot, StringComparison.OrdinalIgnoreCase))
        {
            MessageBox.Show(this, "Full automatic updates currently require the selected shard executable to be inside this Git repository.", "Unsupported shard location", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        var conflicts = _availableUpdates.Where(update => !update.Safe && !update.Resolved && update.Status != "D" && !update.AlreadyCurrent).ToList();
        if (conflicts.Count > 0)
        {
            MessageBox.Show(this, "Resolve local update conflicts before running a full update.", "Conflicts block update", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        var branch = SelectedUpdateBranch();
        if (MessageBox.Show(this, $"Build official upstream/{branch}, back up the current shard, and install its matching executable and JS/DFN files? This can take several minutes.",
                "Build and install full update", MessageBoxButton.YesNo, MessageBoxImage.Question) != MessageBoxResult.Yes) return;

        var staging = Path.Combine(Path.GetTempPath(), "UOX3ControlPanel", Guid.NewGuid().ToString("N"));
        string? backupPath = null;
        var buildClock = Stopwatch.StartNew();
        var buildTimer = new DispatcherTimer { Interval = TimeSpan.FromSeconds(1) };
        buildTimer.Tick += (_, _) => BuildElapsedText.Text = $"Elapsed {buildClock.Elapsed:hh\\:mm\\:ss}";
        try
        {
            InstallFullUpdateButton.IsEnabled = false;
            InstallDataUpdatesButton.IsEnabled = false;
            BuildProgressPanel.Visibility = Visibility.Visible;
            BuildProgressBar.IsIndeterminate = true;
            BuildProgressBar.Value = 0;
            BuildDetailText.Text = "Preparing isolated build workspace…";
            BuildElapsedText.Text = "Elapsed 00:00:00";
            buildTimer.Start();
            UpdateSummaryText.Text = "Creating isolated upstream build workspace…";
            Directory.CreateDirectory(Path.GetDirectoryName(staging)!);
            await RunGitAsync(repository, "worktree", "add", "--detach", staging, $"upstream/{branch}");

            var patchPath = Path.Combine(Path.GetTempPath(), $"uox3-local-source-{Guid.NewGuid():N}.patch");
            var mergeBase = await RunGitAsync(repository, "merge-base", "HEAD", $"upstream/{branch}");
            // Compare the merge base to the working tree. This carries forward
            // both committed branch customizations and uncommitted source edits.
            await RunGitToFileAsync(repository, patchPath, "diff", "--binary", mergeBase.Trim(), "--", "source/");
            if (new FileInfo(patchPath).Length > 0)
            {
                UpdateSummaryText.Text = "Merging local source customizations into the isolated build…";
                await RunGitAsync(staging, "apply", "--3way", patchPath);
            }
            File.Delete(patchPath);

            UpdateSummaryText.Text = "Configuring UOX3 build…";
            BuildDetailText.Text = "CMake is configuring the Visual Studio build…";
            await RunProcessAsync("cmake", staging, ShowBuildOutput, "make/cmake", "-B", "build", "-G", "Visual Studio 17 2022", "-A", "x64", "-T", "ClangCL");
            UpdateSummaryText.Text = "Compiling UOX3 Release…";
            BuildDetailText.Text = "Compiler started; waiting for the first build message…";
            await RunProcessAsync("cmake", staging, ShowBuildOutput, "--build", "build", "--config", "Release", "--target", "uox3", "--parallel", "4");
            var builtExecutable = Path.Combine(staging, "build", "Release", "uox3.exe");
            if (!File.Exists(builtExecutable)) throw new FileNotFoundException("The update build completed without producing build/Release/uox3.exe.");

            var installableData = _availableUpdates.Where(update => update.IsDataFile && update.Safe && !update.AlreadyCurrent && update.Status != "D").ToList();
            var backupRoot = Path.Combine(ServerDirectory!, "archives", "control-panel-updates");
            Directory.CreateDirectory(backupRoot);
            backupPath = Path.Combine(backupRoot, $"uox3-full-{DateTime.Now:yyyyMMdd-HHmmss}.zip");
            using (var archive = ZipFile.Open(backupPath, ZipArchiveMode.Create))
            {
                archive.CreateEntryFromFile(executable, Path.GetRelativePath(repository, executable), CompressionLevel.Optimal);
                foreach (var update in installableData)
                {
                    var target = SafeRepositoryPath(repository, update.Path);
                    if (File.Exists(target)) archive.CreateEntryFromFile(target, update.Path, CompressionLevel.Optimal);
                }
            }

            UpdateSummaryText.Text = "Installing matching executable and data files…";
            File.Copy(builtExecutable, executable, true);
            foreach (var update in installableData)
            {
                var target = SafeRepositoryPath(repository, update.Path);
                Directory.CreateDirectory(Path.GetDirectoryName(target)!);
                File.Copy(Path.Combine(staging, update.Path.Replace('/', Path.DirectorySeparatorChar)), target, true);
            }
            var revision = await RunGitAsync(repository, "rev-parse", $"upstream/{branch}");
            File.WriteAllText(DeployedRevisionPath(), revision.Trim());
            UpdateSummaryText.Text = $"Full update installed successfully. Backup: {backupPath}";
            BuildProgressBar.IsIndeterminate = false;
            BuildProgressBar.Value = 100;
            BuildDetailText.Text = "Build and installation completed successfully.";
            await CheckForUpdatesAsync();
        }
        catch (Exception ex)
        {
            if (backupPath is not null && File.Exists(backupPath))
            {
                try { ZipFile.ExtractToDirectory(backupPath, repository, true); }
                catch { }
            }
            MessageBox.Show(this, $"The full update was not completed. Any deploy-stage files were restored when possible.\n\n{ex.Message}", "Full update failed", MessageBoxButton.OK, MessageBoxImage.Error);
            UpdateSummaryText.Text = "Full update failed; see the error dialog.";
            BuildProgressBar.IsIndeterminate = false;
            BuildProgressBar.Value = 0;
            BuildDetailText.Text = "Build stopped because an error occurred.";
        }
        finally
        {
            buildTimer.Stop();
            buildClock.Stop();
            BuildElapsedText.Text = $"Total {buildClock.Elapsed:hh\\:mm\\:ss}";
            try { await RunGitAsync(repository, "worktree", "remove", "--force", staging); } catch { }
            try { if (Directory.Exists(staging)) Directory.Delete(staging, true); } catch { }
            InstallFullUpdateButton.IsEnabled = _availableUpdates.Count > 0;
        }

        void ShowBuildOutput(string line)
        {
            _ = Dispatcher.InvokeAsync(() =>
            {
                var clean = Regex.Replace(line, @"\s+", " ").Trim();
                if (clean.Length == 0) return;
                BuildDetailText.Text = clean;
                var percent = Regex.Match(clean, @"\[\s*(\d{1,3})%\]");
                if (percent.Success && double.TryParse(percent.Groups[1].Value, out var value))
                {
                    BuildProgressBar.IsIndeterminate = false;
                    BuildProgressBar.Value = Math.Clamp(value, 0, 100);
                }
            });
        }
    }

    private async void InstallDataUpdates_Click(object sender, RoutedEventArgs e)
    {
        var repository = FindRepositoryRoot();
        if (repository is null) return;
        var branch = SelectedUpdateBranch();
        var pendingCore = _availableUpdates.Count(update => update.Category == "Core" && !update.AlreadyCurrent);
        if (pendingCore > 0)
        {
            MessageBox.Show(this, $"This upstream snapshot contains {pendingCore} core change(s). Its JS/DFN files may depend on a newer executable, so data-only installation is blocked. Build and deploy the matching core first.",
                "Matching UOX3 executable required", MessageBoxButton.OK, MessageBoxImage.Warning);
            return;
        }
        var updates = _availableUpdates.Where(update => update.IsDataFile && update.Safe && !update.AlreadyCurrent && update.Status != "D").ToList();
        if (updates.Count == 0) return;
        if (MessageBox.Show(this, $"Back up and install {updates.Count} conflict-free JavaScript/DFN update(s) from official upstream/{branch}?",
                "Install UOX3 data updates", MessageBoxButton.YesNo, MessageBoxImage.Question) != MessageBoxResult.Yes) return;
        try
        {
            InstallDataUpdatesButton.IsEnabled = false;
            var backupRoot = Path.Combine(ServerDirectory ?? repository, "archives", "control-panel-updates");
            Directory.CreateDirectory(backupRoot);
            var backupPath = Path.Combine(backupRoot, $"uox3-data-{DateTime.Now:yyyyMMdd-HHmmss}.zip");
            using (var archive = ZipFile.Open(backupPath, ZipArchiveMode.Create))
            {
                foreach (var update in updates)
                {
                    var target = SafeRepositoryPath(repository, update.Path);
                    if (File.Exists(target)) archive.CreateEntryFromFile(target, update.Path, CompressionLevel.Optimal);
                }
            }
            foreach (var update in updates)
            {
                var target = SafeRepositoryPath(repository, update.Path);
                Directory.CreateDirectory(Path.GetDirectoryName(target)!);
                var temporary = target + ".update-tmp";
                await WriteGitBlobAsync(repository, $"upstream/{branch}:{update.Path}", temporary);
                File.Move(temporary, target, true);
            }
            if (_server is not null && _consoleUnlocked)
            {
                if (updates.Any(update => update.Path.StartsWith("data/dfndata/", StringComparison.OrdinalIgnoreCase))) SendKey('7');
                if (updates.Any(update => update.Path.StartsWith("data/js/", StringComparison.OrdinalIgnoreCase))) SendKey('8');
            }
            UpdateSummaryText.Text = $"Installed {updates.Count} data update(s). Rollback backup: {backupPath}";
            await CheckForUpdatesAsync();
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, ex.Message, "Update installation failed", MessageBoxButton.OK, MessageBoxImage.Error);
            InstallDataUpdatesButton.IsEnabled = true;
        }
    }

    private void UpdateList_SelectionChanged(object sender, SelectionChangedEventArgs e)
    {
        MergeUpdateButton.IsEnabled = UpdateList.SelectedItem is UpdateRecord
        {
            IsDataFile: true,
            Safe: false,
            Resolved: false,
            AlreadyCurrent: false
        } update && update.Status != "D";
    }

    private async void MergeUpdate_Click(object sender, RoutedEventArgs e)
    {
        if (UpdateList.SelectedItem is not UpdateRecord update || !update.IsDataFile || update.Safe || update.Resolved || update.Status == "D") return;
        var repository = FindRepositoryRoot();
        if (repository is null) return;

        var branch = SelectedUpdateBranch();
        var baseline = ReadDeployedRevision() ?? "HEAD";
        var temporaryRoot = Path.Combine(Path.GetTempPath(), "UOX3ControlPanel", "merge-" + Guid.NewGuid().ToString("N"));
        try
        {
            MergeUpdateButton.IsEnabled = false;
            Directory.CreateDirectory(temporaryRoot);
            var baseFile = Path.Combine(temporaryRoot, "base.txt");
            var localFile = Path.Combine(temporaryRoot, "local.txt");
            var upstreamFile = Path.Combine(temporaryRoot, "upstream.txt");
            var target = SafeRepositoryPath(repository, update.Path);

            if (!await TryWriteGitBlobAsync(repository, $"{baseline}:{update.Path}", baseFile)) await File.WriteAllTextAsync(baseFile, string.Empty);
            if (File.Exists(target)) File.Copy(target, localFile, true); else await File.WriteAllTextAsync(localFile, string.Empty);
            if (!await TryWriteGitBlobAsync(repository, $"upstream/{branch}:{update.Path}", upstreamFile))
                throw new InvalidOperationException("The new upstream version of this file could not be read.");

            var (mergedText, hasConflicts) = await RunGitMergeFileAsync(repository, localFile, baseFile, upstreamFile);
            var review = new MergeReviewWindow(
                update.Path,
                await File.ReadAllTextAsync(baseFile),
                await File.ReadAllTextAsync(localFile),
                await File.ReadAllTextAsync(upstreamFile),
                mergedText,
                hasConflicts) { Owner = this };
            if (review.ShowDialog() != true) return;

            Directory.CreateDirectory(Path.GetDirectoryName(target)!);
            if (File.Exists(target)) File.Copy(target, target + $".merge-{DateTime.Now:yyyyMMdd-HHmmss}.bak", false);
            await File.WriteAllTextAsync(target, review.MergedResult, new UTF8Encoding(false));

            var upstreamRevision = (await RunGitAsync(repository, "rev-parse", $"upstream/{branch}")).Trim();
            var resolutions = LoadMergeResolutions();
            resolutions[update.Path] = new MergeResolution(upstreamRevision, FileSha256(target));
            SaveMergeResolutions(resolutions);
            UpdateSummaryText.Text = $"Merged {update.Path}. Your previous local file was backed up beside it.";
            await CheckForUpdatesAsync();
        }
        catch (Exception ex)
        {
            MessageBox.Show(this, ex.Message, "Merge failed", MessageBoxButton.OK, MessageBoxImage.Error);
        }
        finally
        {
            try { if (Directory.Exists(temporaryRoot)) Directory.Delete(temporaryRoot, true); } catch { }
            UpdateList_SelectionChanged(UpdateList, null!);
        }
    }

    private string SelectedUpdateBranch() => UpdateChannelCombo.SelectedItem is ComboBoxItem { Tag: string branch } ? branch : "master";

    private string? FindRepositoryRoot()
    {
        var directory = ServerDirectory is null ? null : new DirectoryInfo(ServerDirectory);
        while (directory is not null)
        {
            if (Directory.Exists(Path.Combine(directory.FullName, ".git")) || File.Exists(Path.Combine(directory.FullName, ".git"))) return directory.FullName;
            directory = directory.Parent;
        }
        return null;
    }

    private string DeployedRevisionPath() => Path.Combine(ServerDirectory!, ".uox3-control-panel-revision");

    private string MergeResolutionsPath() => Path.Combine(ServerDirectory!, ".uox3-control-panel-merges.json");

    private Dictionary<string, MergeResolution> LoadMergeResolutions()
    {
        try
        {
            if (!File.Exists(MergeResolutionsPath())) return new(StringComparer.OrdinalIgnoreCase);
            var saved = JsonSerializer.Deserialize<Dictionary<string, MergeResolution>>(File.ReadAllText(MergeResolutionsPath()));
            return saved is null ? new(StringComparer.OrdinalIgnoreCase) : new(saved, StringComparer.OrdinalIgnoreCase);
        }
        catch { return new(StringComparer.OrdinalIgnoreCase); }
    }

    private void SaveMergeResolutions(Dictionary<string, MergeResolution> resolutions) =>
        File.WriteAllText(MergeResolutionsPath(), JsonSerializer.Serialize(resolutions, new JsonSerializerOptions { WriteIndented = true }));

    private static string FileSha256(string path)
    {
        using var stream = File.OpenRead(path);
        return Convert.ToHexString(SHA256.HashData(stream));
    }

    private string? ReadDeployedRevision()
    {
        try
        {
            var path = DeployedRevisionPath();
            if (!File.Exists(path)) return null;
            var revision = File.ReadAllText(path).Trim();
            return Regex.IsMatch(revision, "^[0-9a-fA-F]{40,64}$") ? revision : null;
        }
        catch { return null; }
    }

    private static string CategorizeUpdate(string path)
    {
        if (path.StartsWith("data/js/", StringComparison.OrdinalIgnoreCase)) return "JavaScript";
        if (path.StartsWith("data/dfndata/", StringComparison.OrdinalIgnoreCase)) return "DFN";
        if (path.StartsWith("source/", StringComparison.OrdinalIgnoreCase) || path.StartsWith("make/", StringComparison.OrdinalIgnoreCase)) return "Core";
        if (path.StartsWith("data/", StringComparison.OrdinalIgnoreCase)) return "Other data";
        return "Project";
    }

    private static async Task<string> RunGitAsync(string repository, params string[] arguments)
    {
        var startInfo = new ProcessStartInfo("git") { WorkingDirectory = repository, UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
        foreach (var argument in arguments) startInfo.ArgumentList.Add(argument);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start Git.");
        var outputTask = process.StandardOutput.ReadToEndAsync();
        var errorTask = process.StandardError.ReadToEndAsync();
        await process.WaitForExitAsync();
        var output = await outputTask;
        var error = await errorTask;
        if (process.ExitCode != 0) throw new InvalidOperationException(error.Trim());
        return output.Trim();
    }

    private static async Task RunGitToFileAsync(string repository, string outputPath, params string[] arguments)
    {
        var startInfo = new ProcessStartInfo("git") { WorkingDirectory = repository, UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
        foreach (var argument in arguments) startInfo.ArgumentList.Add(argument);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start Git.");
        await using (var output = new FileStream(outputPath, FileMode.Create, FileAccess.Write, FileShare.None)) await process.StandardOutput.BaseStream.CopyToAsync(output);
        var error = await process.StandardError.ReadToEndAsync();
        await process.WaitForExitAsync();
        if (process.ExitCode != 0) throw new InvalidOperationException(error.Trim());
    }

    private static async Task RunProcessAsync(string executable, string workingDirectory, Action<string>? outputReceived, params string[] arguments)
    {
        var startInfo = new ProcessStartInfo(executable) { WorkingDirectory = workingDirectory, UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
        foreach (var argument in arguments) startInfo.ArgumentList.Add(argument);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException($"Could not start {executable}.");
        var output = new StringBuilder();
        var error = new StringBuilder();
        var outputTask = ReadProcessLinesAsync(process.StandardOutput, output, outputReceived);
        var errorTask = ReadProcessLinesAsync(process.StandardError, error, outputReceived);
        await process.WaitForExitAsync();
        await Task.WhenAll(outputTask, errorTask);
        if (process.ExitCode != 0) throw new InvalidOperationException(error.Length == 0 ? output.ToString().Trim() : error.ToString().Trim());
    }

    private static async Task ReadProcessLinesAsync(StreamReader reader, StringBuilder captured, Action<string>? outputReceived)
    {
        while (await reader.ReadLineAsync() is { } line)
        {
            captured.AppendLine(line);
            outputReceived?.Invoke(line);
        }
    }

    private static async Task<string?> TryGitAsync(string repository, params string[] arguments)
    {
        try { var value = await RunGitAsync(repository, arguments); return value.Split('\n')[0].Trim(); }
        catch { return null; }
    }

    private static async Task<bool> GitDiffIsCleanAsync(string repository, string reference, string path)
    {
        var startInfo = new ProcessStartInfo("git") { WorkingDirectory = repository, UseShellExecute = false, CreateNoWindow = true };
        startInfo.ArgumentList.Add("diff");
        startInfo.ArgumentList.Add("--quiet");
        startInfo.ArgumentList.Add(reference);
        startInfo.ArgumentList.Add("--");
        startInfo.ArgumentList.Add(path);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start Git.");
        await process.WaitForExitAsync();
        return process.ExitCode == 0;
    }

    private static string SafeRepositoryPath(string repository, string relativePath)
    {
        var root = Path.GetFullPath(repository).TrimEnd(Path.DirectorySeparatorChar) + Path.DirectorySeparatorChar;
        var target = Path.GetFullPath(Path.Combine(repository, relativePath.Replace('/', Path.DirectorySeparatorChar)));
        if (!target.StartsWith(root, StringComparison.OrdinalIgnoreCase)) throw new InvalidOperationException($"Unsafe update path: {relativePath}");
        return target;
    }

    private static async Task WriteGitBlobAsync(string repository, string objectSpec, string destination)
    {
        var startInfo = new ProcessStartInfo("git") { WorkingDirectory = repository, UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
        startInfo.ArgumentList.Add("show");
        startInfo.ArgumentList.Add(objectSpec);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start Git.");
        await using (var output = new FileStream(destination, FileMode.Create, FileAccess.Write, FileShare.None)) await process.StandardOutput.BaseStream.CopyToAsync(output);
        var error = await process.StandardError.ReadToEndAsync();
        await process.WaitForExitAsync();
        if (process.ExitCode != 0)
        {
            File.Delete(destination);
            throw new InvalidOperationException(error.Trim());
        }
    }

    private static async Task<bool> TryWriteGitBlobAsync(string repository, string objectSpec, string destination)
    {
        try { await WriteGitBlobAsync(repository, objectSpec, destination); return true; }
        catch { return false; }
    }

    private static async Task<(string MergedText, bool HasConflicts)> RunGitMergeFileAsync(string repository, string local, string @base, string upstream)
    {
        var startInfo = new ProcessStartInfo("git") { WorkingDirectory = repository, UseShellExecute = false, CreateNoWindow = true, RedirectStandardOutput = true, RedirectStandardError = true };
        foreach (var argument in new[] { "merge-file", "-p", "-L", "Your local file", "-L", "Original base", "-L", "New upstream", local, @base, upstream }) startInfo.ArgumentList.Add(argument);
        using var process = Process.Start(startInfo) ?? throw new InvalidOperationException("Could not start Git merge.");
        var outputTask = process.StandardOutput.ReadToEndAsync();
        var errorTask = process.StandardError.ReadToEndAsync();
        await process.WaitForExitAsync();
        var output = await outputTask;
        var error = await errorTask;
        if (process.ExitCode > 1) throw new InvalidOperationException(string.IsNullOrWhiteSpace(error) ? "Git could not merge this file." : error.Trim());
        return (output, process.ExitCode == 1);
    }

    private async void RefreshSecurity_Click(object sender, RoutedEventArgs e) => await RefreshSecurityAuditAsync();

    private async Task RefreshSecurityAuditAsync()
    {
        var config = ConfigPath is not null && File.Exists(ConfigPath) ? await File.ReadAllTextAsync(ConfigPath) : string.Empty;
        SetAuditStatus(HashingAuditText, ReadIniValue(config, "PASSWORDHASHINGENABLED") == "1", "Enabled", "Disabled");
        var openCreation = ReadIniValue(config, "INTERNALACCOUNTCREATION") == "1";
        CreationAuditText.Text = openCreation ? "Open" : "Restricted";
        CreationAuditText.Foreground = openCreation ? Brushes.Gold : Brushes.LimeGreen;
        SetAuditStatus(ThrottleAuditText, ReadIniValue(config, "LOGINTHROTTLEENABLED") == "1", "Enabled", "Disabled");
        SetAuditStatus(BackupAuditText, ReadIniValue(config, "BACKUPSENABLED") == "1", "Enabled", "Disabled");

        await RefreshNetworkStatusAsync();
        var firewallAllowed = FirewallStatusText.Text.Contains("installed", StringComparison.OrdinalIgnoreCase);
        FirewallAuditText.Text = firewallAllowed ? "Port allowed" : "Not allowed";
        FirewallAuditText.Foreground = firewallAllowed ? Brushes.LimeGreen : Brushes.Gold;

        LoadSecurityAccounts(config);
        LoadFailedLoginIps(config);
    }

    private static void SetAuditStatus(TextBlock control, bool enabled, string enabledText, string disabledText)
    {
        control.Text = enabled ? enabledText : disabledText;
        control.Foreground = enabled ? Brushes.LimeGreen : Brushes.OrangeRed;
    }

    private static string? ReadIniValue(string text, string key)
    {
        var match = Regex.Match(text, $@"(?im)^\s*{Regex.Escape(key)}\s*=\s*(.*?)\s*$");
        return match.Success ? match.Groups[1].Value.Trim() : null;
    }

    private string? ResolveConfiguredDirectory(string config, string key, string fallback)
    {
        if (ServerDirectory is null) return null;
        var configured = ReadIniValue(config, key);
        var value = string.IsNullOrWhiteSpace(configured) ? fallback : configured;
        try { return Path.GetFullPath(value!, ServerDirectory); }
        catch { return null; }
    }

    private void LoadSecurityAccounts(string config)
    {
        SecurityAccountList.Items.Clear();
        var accountsDirectory = ResolveConfiguredDirectory(config, "ACTSDIRECTORY", "./accounts/");
        var accountsFile = accountsDirectory is null ? null : Path.Combine(accountsDirectory, "accounts.adm");
        if (accountsFile is null || !File.Exists(accountsFile)) return;
        try
        {
            foreach (var account in ParseAccounts(File.ReadAllText(accountsFile))) SecurityAccountList.Items.Add(account);
        }
        catch (Exception ex) { FooterText.Text = $"Could not read accounts: {ex.Message}"; }
    }

    private static IReadOnlyList<AccountRecord> ParseAccounts(string text)
    {
        var records = new List<AccountRecord>();
        foreach (Match section in Regex.Matches(text, @"(?ms)^SECTION\s+ACCOUNT\s+\d+\s*(?:\r?\n)\{.*?^\}"))
        {
            var name = Regex.Match(section.Value, @"(?im)^NAME\s+(.+?)\s*$").Groups[1].Value.Trim();
            var flagsText = Regex.Match(section.Value, @"(?im)^FLAGS\s+(0x[0-9a-f]+|\d+)\s*$").Groups[1].Value;
            var lastIp = Regex.Match(section.Value, @"(?im)^LASTIP\s+(.+?)\s*$").Groups[1].Value.Trim();
            if (name.Length == 0 || flagsText.Length == 0) continue;
            var flags = flagsText.StartsWith("0x", StringComparison.OrdinalIgnoreCase)
                ? Convert.ToUInt32(flagsText[2..], 16) : Convert.ToUInt32(flagsText);
            records.Add(new AccountRecord(name, flags, lastIp));
        }
        return records.OrderBy(record => record.Name, StringComparer.OrdinalIgnoreCase).ToList();
    }

    private void LoadFailedLoginIps(string config)
    {
        FailedLoginList.Items.Clear();
        var logsDirectory = ResolveConfiguredDirectory(config, "LOGSDIRECTORY", "./logs/");
        var logFile = logsDirectory is null ? null : Path.Combine(logsDirectory, "accounts.log");
        if (logFile is null || !File.Exists(logFile))
        {
            FailedLoginList.Items.Add("No failed-login log entries yet.");
            return;
        }
        try
        {
            var entries = File.ReadLines(logFile)
                .Where(line => line.Contains("Failed login", StringComparison.OrdinalIgnoreCase) || line.Contains("Login throttle activated", StringComparison.OrdinalIgnoreCase))
                .Reverse().Take(100).ToList();
            if (entries.Count == 0) FailedLoginList.Items.Add("No failed-login log entries yet.");
            else foreach (var entry in entries) FailedLoginList.Items.Add(entry);
        }
        catch (Exception ex) { FailedLoginList.Items.Add($"Could not read accounts.log: {ex.Message}"); }
    }

    private async void AccountBanToggle_Click(object sender, RoutedEventArgs e)
    {
        if (sender is not Button { DataContext: AccountRecord account }) return;
        await SetAccountBanAsync(account, !account.IsBanned);
        e.Handled = true;
    }

    private async Task SetAccountBanAsync(AccountRecord account, bool banned)
    {
        var action = banned ? "ban" : "unban";
        if (MessageBox.Show(this, $"{char.ToUpperInvariant(action[0]) + action[1..]} account '{account.Name}'?", "Confirm account change",
                MessageBoxButton.YesNo, banned ? MessageBoxImage.Warning : MessageBoxImage.Question) != MessageBoxResult.Yes) return;

        var config = ConfigPath is not null && File.Exists(ConfigPath) ? await File.ReadAllTextAsync(ConfigPath) : string.Empty;
        var accountsDirectory = ResolveConfiguredDirectory(config, "ACTSDIRECTORY", "./accounts/");
        var accountsFile = accountsDirectory is null ? null : Path.Combine(accountsDirectory, "accounts.adm");
        if (accountsFile is null || !File.Exists(accountsFile)) return;
        try
        {
            var original = await File.ReadAllTextAsync(accountsFile);
            var changed = false;
            var updated = Regex.Replace(original, @"(?ms)^SECTION\s+ACCOUNT\s+\d+\s*(?:\r?\n)\{.*?^\}", match =>
            {
                var nameMatch = Regex.Match(match.Value, @"(?im)^NAME\s+(.+?)\s*$");
                if (!nameMatch.Success || !nameMatch.Groups[1].Value.Trim().Equals(account.Name, StringComparison.OrdinalIgnoreCase)) return match.Value;
                var flagsMatch = Regex.Match(match.Value, @"(?im)^FLAGS\s+(0x[0-9a-f]+|\d+)\s*$");
                if (!flagsMatch.Success) return match.Value;
                var raw = flagsMatch.Groups[1].Value;
                var flags = raw.StartsWith("0x", StringComparison.OrdinalIgnoreCase) ? Convert.ToUInt32(raw[2..], 16) : Convert.ToUInt32(raw);
                flags = banned ? flags | 0x0001u : flags & ~0x0001u;
                changed = true;
                return match.Value[..flagsMatch.Groups[1].Index] + $"0x{flags:x}" + match.Value[(flagsMatch.Groups[1].Index + flagsMatch.Groups[1].Length)..];
            });
            if (!changed) throw new InvalidOperationException("The selected account was not found in accounts.adm.");
            var backup = $"{accountsFile}.{DateTime.Now:yyyyMMdd-HHmmss}.bak";
            File.Copy(accountsFile, backup, false);
            var temporary = accountsFile + ".tmp";
            await File.WriteAllTextAsync(temporary, updated, new UTF8Encoding(false));
            File.Move(temporary, accountsFile, true);
            if (_server is not null && _consoleUnlocked) SendKey('2');
            FooterText.Text = $"Account '{account.Name}' {(banned ? "banned" : "unbanned")}. Backup: {Path.GetFileName(backup)}";
            await RefreshSecurityAuditAsync();
        }
        catch (Exception ex) { MessageBox.Show(this, ex.Message, "Account update failed", MessageBoxButton.OK, MessageBoxImage.Error); }
    }

    private void Broadcast_Click(object sender, RoutedEventArgs e)
    {
        var message = BroadcastText.Text.Trim();
        if (message.Length == 0) return;
        SendText("Y" + message + "\r");
        BroadcastText.Clear();
        FooterText.Text = "Broadcast sent";
    }

    private void BroadcastText_KeyDown(object sender, System.Windows.Input.KeyEventArgs e)
    {
        if (e.Key != Key.Enter) return;
        Broadcast_Click(sender, e);
        e.Handled = true;
    }

    private void CustomKey_Click(object sender, RoutedEventArgs e)
    {
        if (CustomKeyText.Text.Length != 1) return;
        SendKey(CustomKeyText.Text[0]);
        CustomKeyText.Clear();
    }

    private void SendText(string text)
    {
        try
        {
            if (_server is null || _server.HasExited) return;
            _server.StandardInput.Write(text);
            _server.StandardInput.Flush();
        }
        catch (Exception ex) { FooterText.Text = $"Could not send command: {ex.Message}"; }
    }

    private void MainWindow_Closing(object? sender, System.ComponentModel.CancelEventArgs e)
    {
        if (!_allowExit)
        {
            e.Cancel = true;
            HideToTray();
            return;
        }
        if (_server is null || _server.HasExited) return;
        var answer = MessageBox.Show(this, "UOX3 is still running. Send a graceful shutdown before closing the control panel?",
            "Server is running", MessageBoxButton.YesNoCancel, MessageBoxImage.Question);
        if (answer == MessageBoxResult.Cancel) { e.Cancel = true; return; }
        if (answer == MessageBoxResult.Yes) SendKey('q');
    }

    private void MainWindow_StateChanged(object? sender, EventArgs e)
    {
        if (WindowState == WindowState.Minimized) HideToTray();
    }

    private void HideToTray()
    {
        Hide();
        if (_shownTrayHint) return;
        _shownTrayHint = true;
        _trayIcon.ShowBalloonTip(2500, _shardName, "Still running in the notification area.", Forms.ToolTipIcon.Info);
    }

    private void ShowFromTray()
    {
        Show();
        WindowState = WindowState.Normal;
        Activate();
        Topmost = true;
        Topmost = false;
        Focus();
    }

    private async Task ExitApplicationAsync()
    {
        var process = _server;
        if (process is not null && !process.HasExited)
        {
            var answer = MessageBox.Show(this, "Gracefully shut down UOX3 and exit the control panel?", "Exit UOX3 Control Panel",
                MessageBoxButton.YesNo, MessageBoxImage.Question);
            if (answer != MessageBoxResult.Yes) return;
            SendKey('q');
            try
            {
                using var timeout = new CancellationTokenSource(TimeSpan.FromSeconds(30));
                await process.WaitForExitAsync(timeout.Token);
            }
            catch (OperationCanceledException)
            {
                MessageBox.Show(this, "UOX3 did not stop within 30 seconds. The control panel will remain open.", "Shutdown still in progress",
                    MessageBoxButton.OK, MessageBoxImage.Warning);
                return;
            }
        }

        _allowExit = true;
        _trayIcon.Visible = false;
        _trayIcon.Dispose();
        Close();
    }

    private sealed record ControlPanelSettings(string? ExecutablePath);
    private sealed record IniHelp(string Description, string DefaultValue, string ApplyNote);
    private sealed record IniSettingEntry(string Name, string Value, string Section, int LineNumber, string Description, string DefaultValue, string ApplyNote)
    {
        public override string ToString() => $"{Name} = {Value}";
    }

    private static readonly Dictionary<string, IniHelp> IniHelpCatalog = new(StringComparer.OrdinalIgnoreCase)
    {
        ["SERVERNAME"] = new("Public name displayed for this shard.", "My UOX3 Shard", "Restart recommended so every server surface uses the new name."),
        ["EXTERNALIP"] = new("IP address advertised to connecting clients. Use the public address for Internet shards or a LAN address for private play.", "127.0.0.1", "Reload INI, then reconnect clients."),
        ["PORT"] = new("TCP port used for incoming UOX3 client connections. The router and Windows Firewall must allow the same port.", "2593", "Restart required. Update the firewall rule after changing it."),
        ["SECRETSHARDKEY"] = new("Optional secret used by supported shard-discovery or authentication features. Do not share a real key publicly.", "None", "Restart recommended."),
        ["SERVERLANGUAGE"] = new("Numeric language identifier used for server messages and dictionary selection.", "0", "Reload INI or restart."),
        ["NETRCVTIMEOUT"] = new("Maximum network receive wait before a connection is treated as unresponsive.", "3", "Restart recommended."),
        ["NETSNDTIMEOUT"] = new("Maximum network send wait before a connection is treated as unresponsive.", "3", "Restart recommended."),
        ["NETRETRYCOUNT"] = new("Number of network retries before UOX3 gives up on an operation.", "3", "Restart recommended."),
        ["CONSOLELOG"] = new("Writes server console output to log files. 0 = disabled; 1 = enabled.", "1", "Reload INI."),
        ["NETWORKLOG"] = new("Enables detailed network logging. This can produce large logs and may expose connection details.", "0", "Reload INI."),
        ["SPEECHLOG"] = new("Records player speech in the server logs. Consider player privacy before enabling it.", "0", "Reload INI."),
        ["COMMANDPREFIX"] = new("Character players or staff type before an in-game command.", "'", "Reload INI."),
        ["ANNOUNCEWORLDSAVES"] = new("Announces automatic world saves to connected players.", "1", "Reload INI."),
        ["JOINPARTMSGS"] = new("Shows player join and departure messages.", "1", "Reload INI."),
        ["BACKUPSENABLED"] = new("Creates periodic shard backups according to BACKUPSAVERATIO.", "0", "Reload INI. Verify BACKUPDIRECTORY before enabling."),
        ["BACKUPSAVERATIO"] = new("Number of world saves between automatic backups.", "5", "Reload INI."),
        ["SAVESTIMER"] = new("Seconds between automatic world saves.", "600", "Reload INI."),
        ["UOGENABLED"] = new("Enables UOGateway-compatible shard listing support.", "1", "Restart recommended."),
        ["FREESHARDSERVERPOLL"] = new("Allows polling by supported free-shard server-list services.", "1", "Restart recommended."),
        ["RANDOMSTARTINGLOCATION"] = new("Chooses a random configured starting location for newly created characters.", "0", "Reload INI."),
        ["ASSISTANTNEGOTIATION"] = new("Enables negotiation with supported client assistants.", "0", "Restart recommended."),
        ["KICKONASSISTANTSILENCE"] = new("Disconnects clients that fail the configured assistant-negotiation exchange.", "0", "Restart recommended."),
        ["CLASSICUOMAPTRACKER"] = new("Enables map tracking support used by compatible ClassicUO clients.", "1", "Restart recommended."),
        ["JSENGINESIZE"] = new("Memory allocation size for the JavaScript engine.", "256", "Restart required."),
        ["USEUNICODEMESSAGES"] = new("Uses Unicode-capable server messages when supported by the client.", "1", "Reload INI."),
        ["CONTEXTMENUS"] = new("Enables client context menus.", "1", "Reload INI."),
        ["SYSMESSAGECOLOUR"] = new("Numeric Ultima Online hue used for system messages.", "72", "Reload INI."),
        ["MAXCLIENTBYTESIN"] = new("Inbound client traffic threshold used to detect abusive or broken connections.", "50000", "Restart recommended."),
        ["MAXCLIENTBYTESOUT"] = new("Outbound client traffic threshold used to detect abusive or broken connections.", "200000", "Restart recommended."),
        ["NETTRAFFICTIMEBAN"] = new("Temporary ban duration applied after a network traffic limit is exceeded.", "30", "Restart recommended."),
        ["INTERNALACCOUNTCREATION"] = new("Allows clients to create accounts automatically. Disable this for invitation-only shards.", "1", "Reload INI."),
        ["PASSWORDHASHINGENABLED"] = new("Stores account passwords as hashes instead of readable plain text. Back up accounts before enabling migration.", "0", "Restart required; review existing account compatibility first."),
        ["LOGINTHROTTLEENABLED"] = new("Rate-limits repeated failed login attempts.", "1", "Restart recommended."),
        ["LOGINTHROTTLEMAXATTEMPTS"] = new("Failed attempts allowed within LOGINTHROTTLEWINDOW before delays are applied.", "5", "Restart recommended."),
        ["LOGINTHROTTLEWINDOW"] = new("Time window in seconds used to count failed login attempts.", "60", "Restart recommended."),
        ["LOGINTHROTTLEINITIALDELAY"] = new("Initial login delay in seconds after the throttle activates.", "2", "Restart recommended."),
        ["LOGINTHROTTLEMULTIPLIER"] = new("Multiplier used to increase the delay after repeated failures.", "2", "Restart recommended."),
        ["LOGINTHROTTLEMAXDELAY"] = new("Maximum throttling delay in seconds.", "300", "Restart recommended."),
        ["LOGINTHROTTLEENTRYTTL"] = new("Seconds before an inactive login-throttle record is discarded.", "3600", "Restart recommended."),
        ["ACCOUNTFLUSH"] = new("Interval used to flush account changes to disk.", "5", "Reload INI."),
        ["DATADIRECTORY"] = new("Location of the Ultima Online client data files required by UOX3.", "Client installation path", "Restart required."),
        ["DEFSDIRECTORY"] = new("Location of UOX3 DFN definition files.", "./dfndata/", "Restart required after changing the path."),
        ["SCRIPTSDIRECTORY"] = new("Location of UOX3 JavaScript server scripts.", "./js/", "Restart required after changing the path."),
        ["BACKUPDIRECTORY"] = new("Folder where automatic shard backups are stored.", "./archives/", "Reload INI; ensure the folder is writable."),
        ["LOGSDIRECTORY"] = new("Folder where server log files are written.", "./logs/", "Restart recommended."),
        ["CLIENTFEATURES"] = new("Bitmask of client feature flags advertised or enabled by the shard.", "Version dependent", "Restart required. Use a value supported by the selected client era.")
    };

    private sealed record AccountRecord(string Name, uint Flags, string LastIp)
    {
        public bool IsBanned => (Flags & 0x0001) != 0;
        public string StatusLabel => IsBanned ? "[BANNED]" : "[ACTIVE]";
        public string ActionLabel => IsBanned ? "Unban" : "Ban";
    }
    private sealed record MergeResolution(string UpstreamRevision, string LocalSha256);
    private sealed record UpdateRecord(string Path, string Status, string Category, bool Safe, bool AlreadyCurrent, bool Resolved)
    {
        public bool IsDataFile => Category is "JavaScript" or "DFN";
        public override string ToString()
        {
            var state = AlreadyCurrent ? "ALREADY INSTALLED" : Resolved ? "MERGED LOCALLY" : Status == "D" ? "REMOVAL — SKIPPED" : Safe ? "SAFE" : "LOCAL CONFLICT";
            return $"[{Category,-10}] [{state,-17}] {Status}  {Path}";
        }
    }

    private sealed class TerminalOutputParser
    {
        private enum EscapeState { Normal, Escape, Csi, Osc, OscEscape }

        private readonly StringBuilder _line = new();
        private int _cursor;
        private int _row;
        private EscapeState _escapeState;
        private readonly StringBuilder _csi = new();

        public string Process(ReadOnlySpan<char> input)
        {
            var completed = new StringBuilder();
            foreach (var character in input)
            {
                if (ConsumeEscape(character, completed)) continue;

                switch (character)
                {
                    case '\x1B':
                        _escapeState = EscapeState.Escape;
                        break;
                    case '\r':
                        // Console progress indicators redraw the current line.
                        _cursor = 0;
                        break;
                    case '\n':
                        AppendCompletedLine(completed);
                        _row++;
                        break;
                    case '\b':
                        if (_cursor > 0) _cursor--;
                        break;
                    case '\0':
                    case '\a':
                        break;
                    default:
                        if (!char.IsControl(character)) WriteCharacter(character);
                        break;
                }
            }
            return completed.ToString();
        }

        private bool ConsumeEscape(char character, StringBuilder completed)
        {
            switch (_escapeState)
            {
                case EscapeState.Normal:
                    return false;
                case EscapeState.Escape:
                    _escapeState = character switch
                    {
                        '[' => EscapeState.Csi,
                        ']' => EscapeState.Osc,
                        _ => EscapeState.Normal
                    };
                    if (_escapeState == EscapeState.Csi) _csi.Clear();
                    return true;
                case EscapeState.Csi:
                    if (character is >= '@' and <= '~')
                    {
                        ApplyCsi(character, _csi.ToString(), completed);
                        _escapeState = EscapeState.Normal;
                    }
                    else _csi.Append(character);
                    return true;
                case EscapeState.Osc:
                    if (character == '\a') _escapeState = EscapeState.Normal;
                    else if (character == '\x1B') _escapeState = EscapeState.OscEscape;
                    return true;
                case EscapeState.OscEscape:
                    _escapeState = EscapeState.Normal;
                    return true;
                default:
                    return false;
            }
        }

        private void ApplyCsi(char command, string arguments, StringBuilder completed)
        {
            var values = arguments.TrimStart('?').Split(';', StringSplitOptions.RemoveEmptyEntries)
                .Select(value => int.TryParse(value, out var number) ? number : 1).ToArray();
            var first = values.Length > 0 ? Math.Max(values[0], 1) : 1;
            switch (command)
            {
                case 'G':
                    _cursor = first - 1;
                    break;
                case 'C':
                    _cursor += first;
                    break;
                case 'D':
                    _cursor = Math.Max(0, _cursor - first);
                    break;
                case 'H':
                case 'f':
                    var targetRow = values.Length > 0 ? Math.Max(0, values[0] - 1) : 0;
                    if (targetRow != _row && _line.Length > 0) AppendCompletedLine(completed);
                    _row = targetRow;
                    if (values.Length > 1) _cursor = Math.Max(0, values[1] - 1);
                    break;
                case 'K':
                    if (_cursor < _line.Length) _line.Length = _cursor;
                    break;
            }
        }

        private void WriteCharacter(char character)
        {
            if (_cursor < _line.Length) _line[_cursor] = character;
            else
            {
                while (_line.Length < _cursor) _line.Append(' ');
                _line.Append(character);
            }
            _cursor++;
        }

        private void AppendCompletedLine(StringBuilder output)
        {
            var line = _line.ToString().TrimEnd();
            output.AppendLine(line);
            _line.Clear();
            _cursor = 0;
        }
    }
}
