#pragma once
#include <SFML/Network.hpp>
#include <iostream>

class NetMessages;

class NetworkClient 
{
    private:
        static NetworkClient* instance;
        sf::TcpSocket socket;
        bool connected;

        // Costruttore privato (Singleton)
        NetworkClient();

    public:
        // Accesso statico
        static NetworkClient* getInstance();
        
        // Connessione
        bool connect(const std::string& ip, unsigned short port);
        void disconnect();
        bool isConnected() const;

        // INVIO (Template per comodità)
        // Questa funzione magica accetta qualsiasi struct (Move, Login) e la spedisce
        template <typename T>
        void sendPacket(T& packet)
        {
            if (!connected) 
                return;

            // 1. Compila l'header automaticamente
            // (Calcola la dimensione totale della struct T)
            packet.header.packetSize = sizeof(T);
            
            // 2. Spedisci i byte grezzi
            // reinterpret_cast trasforma la struct in un array di char (byte)
            if (socket.send(&packet, sizeof(T)) != sf::Socket::Status::Done) 
            {
                std::cerr << "Errore invio pacchetto!" << std::endl;
            }
        }

        // RICEZIONE (Semplificata per ora)
        // Cerca di ricevere dati nel buffer
        sf::Socket::Status receive(void* data, std::size_t size, std::size_t& received);
        
        sf::TcpSocket& getSocket(); // Getter se serve accesso diretto
};