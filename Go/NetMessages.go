// package main

// import (
// 	"encoding/binary"
// 	"math"
// )

// type PacketType byte

// const (
// 	PacketTypeMove       PacketType = 1
// 	PacketTypeJoin       PacketType = 2
// 	PacketTypeLeave      PacketType = 3
// 	PacketTypeWelcome    PacketType = 4
// 	PacketTypePlayerList PacketType = 5
// )

// type PacketHeader struct {
// 	Type PacketType
// 	Size uint16
// }

// type PacketMove struct {
// 	Header        PacketHeader
// 	PlayerID      byte
// 	X             float32
// 	Y             float32
// 	VelocityX     float32
// 	VelocityY     float32
// 	IsFacingRight bool
// 	IsGrounded    bool
// }

// type PacketJoin struct {
// 	Header     PacketHeader
// 	PlayerID   byte
// 	PlayerName [16]byte // Nome fisso di 16 byte
// 	IsLocal    bool
// }

// type PacketWelcome struct {
// 	Header          PacketHeader
// 	PlayerID        byte
// 	ExistingPlayers byte // Numero di giocatori esistenti
// }

// type PlayerState struct {
// 	ID            byte
// 	Name          string
// 	X             float32
// 	Y             float32
// 	VelocityX     float32
// 	VelocityY     float32
// 	IsFacingRight bool
// 	IsGrounded    bool
// 	LastSeen      int64 // Timestamp Unix in millisecondi
// }

// // Serializzazione
// func (p *PacketMove) Serialize() []byte {
// 	buf := make([]byte, 1+2+1+4*4+2) // Header + PlayerID + 4 float32 + 2 bool
// 	buf[0] = byte(p.Header.Type)
// 	binary.LittleEndian.PutUint16(buf[1:3], p.Header.Size)
// 	buf[3] = p.PlayerID

// 	// Scrivi i float32
// 	offset := 4
// 	binary.LittleEndian.PutUint32(buf[offset:], math.Float32bits(p.X))
// 	offset += 4
// 	binary.LittleEndian.PutUint32(buf[offset:], math.Float32bits(p.Y))
// 	offset += 4
// 	binary.LittleEndian.PutUint32(buf[offset:], math.Float32bits(p.VelocityX))
// 	offset += 4
// 	binary.LittleEndian.PutUint32(buf[offset:], math.Float32bits(p.VelocityY))
// 	offset += 4

// 	// Scrivi i bool
// 	if p.IsFacingRight {
// 		buf[offset] = 1
// 	} else {
// 		buf[offset] = 0
// 	}
// 	offset += 1

// 	if p.IsGrounded {
// 		buf[offset] = 1
// 	} else {
// 		buf[offset] = 0
// 	}

// 	return buf
// }

// func DeserializeMovePacket(data []byte) (*PacketMove, error) {
// 	if len(data) < 1+2+1+4*4+2 {
// 		return nil, nil
// 	}

// 	packet := &PacketMove{}
// 	packet.Header.Type = PacketType(data[0])
// 	packet.Header.Size = binary.LittleEndian.Uint16(data[1:3])
// 	packet.PlayerID = data[3]

// 	offset := 4
// 	packet.X = math.Float32frombits(binary.LittleEndian.Uint32(data[offset:]))
// 	offset += 4
// 	packet.Y = math.Float32frombits(binary.LittleEndian.Uint32(data[offset:]))
// 	offset += 4
// 	packet.VelocityX = math.Float32frombits(binary.LittleEndian.Uint32(data[offset:]))
// 	offset += 4
// 	packet.VelocityY = math.Float32frombits(binary.LittleEndian.Uint32(data[offset:]))
// 	offset += 4

// 	packet.IsFacingRight = data[offset] == 1
// 	offset += 1
// 	packet.IsGrounded = data[offset] == 1

// 	return packet, nil
// }
