using Dashboard.Network;

namespace Dashboard;

public partial class MainPage : ContentPage
{
    NetworkClient _client = new();

    public MainPage()
    {
        InitializeComponent(); // ← ora ESISTE
        _client.OnPacketReceived += OnPacket;
    }

    async void OnConnect(object sender, EventArgs e)
    {
        await _client.ConnectAsync("127.0.0.1", 8080);
        Log("Connesso al server");
    }

    async void OnSendMove(object sender, EventArgs e)
    {
        var packet = PacketWriter.CreateMovePacket(10.5f, 20.2f);
        await _client.SendAsync(packet);
        Log("MOVE inviato");
    }

    void OnPacket(uint type, byte[] body)
    {
        if (type == (uint)PacketType.Move)
        {
            uint playerId = BitConverter.ToUInt32(body, 0);
            float x = BitConverter.ToSingle(body, 4);
            float y = BitConverter.ToSingle(body, 8);

            MainThread.BeginInvokeOnMainThread(() =>
                Log($"MOVE ricevuto → ID:{playerId} X:{x} Y:{y}")
            );
        }
    }

    void Log(string msg)
    {
        LogLabel.Text += msg + "\n"; // ← ora ESISTE
    }
}
