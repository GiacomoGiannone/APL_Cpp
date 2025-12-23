using System;
using System.IO;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace Dashboard.Network
{
    public class NetworkClient
    {
        TcpClient _client;
        NetworkStream _stream;

        public event Action<uint, byte[]> OnPacketReceived;

        public async Task ConnectAsync(string ip, int port)
        {
            _client = new TcpClient();
            await _client.ConnectAsync(ip, port);
            _stream = _client.GetStream();

            _ = Task.Run(ReadLoop);
        }

        async Task ReadLoop()
        {
            try
            {
                while (true)
                {
                    byte[] headerBytes = await ReadExactAsync(8);
                    uint type = BitConverter.ToUInt32(headerBytes, 0);
                    uint size = BitConverter.ToUInt32(headerBytes, 4);

                    byte[] body = await ReadExactAsync((int)size - 8);

                    OnPacketReceived?.Invoke(type, body);
                }
            }
            catch
            {
                // Disconnesso
            }
        }

        async Task<byte[]> ReadExactAsync(int size)
        {
            byte[] buffer = new byte[size];
            int read = 0;

            while (read < size)
                read += await _stream.ReadAsync(buffer, read, size - read);

            return buffer;
        }

        public async Task SendAsync(byte[] data)
        {
            await _stream.WriteAsync(data, 0, data.Length);
        }
    }
}
