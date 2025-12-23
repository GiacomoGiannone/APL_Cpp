using System;
using System.IO;

namespace Dashboard.Network
{
    public enum PacketType : uint
    {
        Join = 1,
        Move = 2
    }

    public struct PacketHeader
    {
        public uint Type;
        public uint PacketSize;
    }

    public static class PacketWriter
    {
        public static byte[] CreateMovePacket(float x, float y)
        {
            using var ms = new MemoryStream();
            using var bw = new BinaryWriter(ms);

            uint type = (uint)PacketType.Move;
            uint size = 8 + 4 + 4 + 4; // header + playerId + x + y

            bw.Write(type);
            bw.Write(size);

            bw.Write(0u); // playerId (server lo imposta)
            bw.Write(x);
            bw.Write(y);

            return ms.ToArray();
        }
    }
}
