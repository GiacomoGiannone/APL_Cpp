using System.IO;

namespace Dashboard.Network;

// Enum deve corrispondere a quello C++ (NetMessages.h)
public enum PacketType : uint
{
    None = 0,
    AssignID = 1,
    Move = 2,
    PlayerJoined = 3,
    // Aggiungi qui altri messaggi se necessario
}

public class GamePacket
{
    public PacketType Type { get; set; }
    public byte[] Data { get; set; } = Array.Empty<byte>();

    // Helper per creare pacchetti da inviare (se la dashboard dovrà dare comandi)
    public static byte[] Serialize(PacketType type, byte[] body)
    {
        uint bodySize = (uint)(body?.Length ?? 0);
        uint totalSize = 4 + 4 + bodySize; // 4 Type + 4 Size + Body

        using var ms = new MemoryStream();
        using var writer = new BinaryWriter(ms);
        
        writer.Write((uint)type);
        writer.Write(totalSize);
        if (body != null) writer.Write(body);
        
        return ms.ToArray();
    }
}