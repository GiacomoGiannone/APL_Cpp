using Dashboard.Network;
using System.Text;

namespace Dashboard;

public partial class MainPage : ContentPage
{
    private NetworkClient _client;
    
    // MEMORIA: Mantiene lo stato attuale di tutti i giocatori
    // La chiave è l'ID del player, il valore è una tupla (X, Y)
    private Dictionary<uint, (float X, float Y)> _playerPositions = new();
    
    // TIMER: Per aggiornare la UI a intervalli regolari
    private IDispatcherTimer _uiTimer;

    public MainPage()
    {
        InitializeComponent();
        _client = new NetworkClient();
        
        // Setup eventi rete
        _client.OnPacketReceived += HandlePacket;
        _client.OnLog += (msg) => Console.WriteLine($"[NET] {msg}"); // Solo console, non intasiamo la UI

        // Setup Timer UI (Aggiorna 10 volte al secondo = 100ms)
        // Questo impedisce alla dashboard di freezarsi
        _uiTimer = Dispatcher.CreateTimer();
        _uiTimer.Interval = TimeSpan.FromMilliseconds(100);
        _uiTimer.Tick += OnUpdateUiTick;
        _uiTimer.Start();
    }

    private async void OnConnect(object? sender, EventArgs e)
    {
        if (!_client.IsConnected)
        {
            await _client.ConnectAsync("127.0.0.1", 8080);
            StatusLabel.Text = "Stato: Connesso ✅";
            StatusLabel.TextColor = Colors.Green;
        }
        else
        {
            _client.Disconnect();
            StatusLabel.Text = "Stato: Disconnesso ❌";
            StatusLabel.TextColor = Colors.Red;
            lock (_playerPositions) { _playerPositions.Clear(); }
        }
    }

    private void OnSendMove(object? sender, EventArgs e)
    {
        // Placeholder per comandi futuri
    }

    // QUESTO METODO GIRA SUL THREAD DI RETE (VELOCISSIMO)
    private void HandlePacket(GamePacket packet)
    {
        if (packet.Type == PacketType.Move && packet.Data.Length >= 12)
        {
            uint id = BitConverter.ToUInt32(packet.Data, 0);
            float x = BitConverter.ToSingle(packet.Data, 4);
            float y = BitConverter.ToSingle(packet.Data, 8);

            // Aggiorniamo solo la memoria (operazione istantanea)
            // Usiamo il lock perché il timer della UI legge questa variabile contemporaneamente
            lock (_playerPositions)
            {
                _playerPositions[id] = (x, y);
            }
        }
        else if (packet.Type == PacketType.PlayerJoined)
        {
            // Loggare eventi rari va bene
            MainThread.BeginInvokeOnMainThread(() => 
                LogLabel.Text = $"[{DateTime.Now:HH:mm:ss}] Nuovo Player connesso!\n" + LogLabel.Text);
        }
    }

    // QUESTO METODO GIRA SUL THREAD PRINCIPALE (10 VOLTE AL SECONDO)
    private void OnUpdateUiTick(object? sender, EventArgs e)
    {
        // Se non siamo connessi o non ci sono dati, non fare nulla
        if (_playerPositions.Count == 0) return;

        var sb = new StringBuilder();
        sb.AppendLine("--- STATO IN TEMPO REALE ---");

        lock (_playerPositions)
        {
            // Ordina per ID per evitare che saltino nella lista
            foreach (var player in _playerPositions.OrderBy(p => p.Key))
            {
                sb.AppendLine($"👤 Player {player.Key}:  X: {player.Value.X:000.0}  |  Y: {player.Value.Y:000.0}");
            }
        }

        // Sovrascrivi il testo invece di appenderlo all'infinito
        LogLabel.Text = sb.ToString();
    }
}