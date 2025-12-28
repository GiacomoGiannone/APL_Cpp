package main

import (
	"encoding/binary"
	"fmt"
	"io"
	"net"
	"sync"
)

// Configurazione
const (
	PORT = ":8080"
)

// Tipi di pacchetti (deve corrispondere a C++)
const (
	PACKET_LOGIN               = 1
	PACKET_MOVE                = 2
	PACKET_PLAYER_DISCONNECTED = 3
	PACKET_ENEMY_SPAWN         = 4
	PACKET_ENEMY_UPDATE        = 5
	PACKET_ENEMY_DAMAGE        = 6
	PACKET_ENEMY_DEATH         = 7
)

// Struttura Client: rappresenta un giocatore connesso
type Client struct {
	conn net.Conn
	id   uint32
}

// Stato globale del server
var (
	clients   = make(map[uint32]*Client)     // Mappa di tutti i client connessi
	clientsMu sync.Mutex                     // Mutex per evitare crash quando due goroutine scrivono sulla mappa
	nextID    uint32                     = 1 // Contatore per assegnare ID univoci
)

// HEADER: Deve essere identico alla struct C++ PacketHeader
// C++: uint32 type, uint32 packetSize
type PacketHeader struct {
	Type       uint32
	PacketSize uint32
}

func main() {
	// 1. Iniziamo ad ascoltare sulla porta TCP
	listener, err := net.Listen("tcp", PORT)
	if err != nil {
		fmt.Printf("Errore avvio server: %v\n", err)
		return
	}
	fmt.Printf("🚀 Server Go avviato su porta %s\n", PORT)

	// 2. Loop infinito: accetta nuove connessioni
	for {
		conn, err := listener.Accept()
		if err != nil {
			fmt.Printf("Errore connessione: %v\n", err)
			continue
		}

		// 3. Per ogni client, avvia una "Goroutine" (thread leggero) separata
		go handleClient(conn)
	}
}

func handleClient(conn net.Conn) {
	// Assegna un ID al nuovo client
	clientsMu.Lock() //Proteggiamo la mappa clients prima di scriverci dentro
	id := nextID
	nextID++
	client := &Client{conn: conn, id: id}
	clients[id] = client
	clientsMu.Unlock()

	fmt.Printf("➕ Nuovo Giocatore Connesso: ID %d (%s)\n", id, conn.RemoteAddr())

	// Assicurati di rimuovere il client quando la funzione finisce (disconnessione)
	defer func() {
		clientsMu.Lock()
		delete(clients, id)
		clientsMu.Unlock()
		conn.Close()
		fmt.Printf("➖ Giocatore Disconnesso: ID %d\n", id)
	}()

	// 4. Loop di lettura messaggi dal client
	for {
		// A. Leggi l'Header (8 Byte: 4 per Type + 4 per Size)
		// Usiamo LittleEndian perché i PC standard (x86/64) usano questo formato
		var header PacketHeader
		err := binary.Read(conn, binary.LittleEndian, &header)
		if err != nil {
			if err != io.EOF {
				fmt.Printf("Errore lettura header ID %d: %v\n", id, err)
			}
			return // Esci dal loop -> disconnessione
		}

		// B. Controllo di sicurezza sulla dimensione
		// (PacketSize include l'header stesso, quindi deve essere almeno 8)
		if header.PacketSize < 8 || header.PacketSize > 1024 {
			fmt.Printf("Pacchetto anomalo da ID %d: size %d\n", id, header.PacketSize)
			return
		}

		// C. Leggi il Corpo del pacchetto (Size - 8 bytes di header già letti)
		bodySize := header.PacketSize - 8
		body := make([]byte, bodySize)
		_, err = io.ReadFull(conn, body)
		if err != nil {
			fmt.Printf("Errore lettura body ID %d: %v\n", id, err)
			return
		}

		// D. Logica server: Qui potremmo modificare il pacchetto
		// Il server forza l'ID del pacchetto per sicurezza (prevenendo impersonificazioni)
		// (Il campo playerId è il primo campo (uint32) dopo l'header nel tuo MovePacket)
		if header.Type == PACKET_MOVE {
			// Sovrascriviamo i primi 4 byte del body con il vero ID del client
			binary.LittleEndian.PutUint32(body[0:4], id)
		}

		// Gestione pacchetti nemici
		if header.Type == PACKET_ENEMY_UPDATE {
			// Inoltra aggiornamento nemico a tutti gli altri client
			fmt.Printf("👾 Enemy Update ricevuto da ID %d\n", id)
		}

		if header.Type == PACKET_ENEMY_DAMAGE {
			// Inoltra danno nemico a tutti gli altri client
			fmt.Printf("⚔️ Enemy Damage ricevuto da ID %d\n", id)
		}

		// E. INOLTRO (Broadcasting)
		// Ricostruiamo il pacchetto completo (Header + Body) per mandarlo agli altri
		fullPacket := make([]byte, header.PacketSize)

		// Scrivi header nel buffer
		binary.LittleEndian.PutUint32(fullPacket[0:4], header.Type)
		binary.LittleEndian.PutUint32(fullPacket[4:8], header.PacketSize)
		// Copia il body
		copy(fullPacket[8:], body)

		broadcast(fullPacket, id)
	}
}

// Invia il pacchetto a TUTTI tranne al mittente (senderID)
func broadcast(data []byte, senderID uint32) {
	clientsMu.Lock()
	defer clientsMu.Unlock()

	for id, client := range clients {
		if id != senderID {
			// Scrittura non bloccante (in un server reale si userebbero canali)
			_, err := client.conn.Write(data)
			if err != nil {
				fmt.Printf("Errore invio a ID %d\n", id)
			}
		}
	}
}
