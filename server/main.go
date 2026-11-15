package main

import (
	"encoding/hex"
	"flag"
	"fmt"
	"io"
	"log"
	"net"
	"net/http"
)

func main() {
	transport := flag.String("transport", "tcp", "transport type: tcp or http")
	addr := flag.String("addr", ":9000", "listen address")
	flag.Parse()

	switch *transport {
	case "tcp":
		serveTCP(*addr)
	case "http":
		serveHTTP(*addr)
	default:
		log.Fatalf("Unknown transport: %s (use tcp or http)", *transport)
	}
}

func serveTCP(addr string) {
	listener, err := net.Listen("tcp", addr)
	if err != nil {
		log.Fatal(err)
	}
	defer listener.Close()

	log.Printf("[TCP] Listening on %s", addr)

	for {
		conn, err := listener.Accept()
		if err != nil {
			log.Printf("Accept error: %v", err)
			continue
		}
		go handleTCPConnection(conn)
	}
}

func handleTCPConnection(conn net.Conn) {
	defer conn.Close()
	log.Printf("[TCP] Connected: %s", conn.RemoteAddr())

	buffer := make([]byte, 4096)
	for {
		n, err := conn.Read(buffer)
		if n > 0 {
			log.Printf("[TCP] %s received %d bytes:\n  hex: %s\n  raw: %s",
				conn.RemoteAddr(), n,
				hex.EncodeToString(buffer[:n]),
				string(buffer[:n]),
			)
		}
		if err != nil {
			if err != io.EOF {
				log.Printf("[TCP] Read error from %s: %v", conn.RemoteAddr(), err)
			}
			log.Printf("[TCP] Disconnected: %s", conn.RemoteAddr())
			return
		}
	}
}

func serveHTTP(addr string) {
	mux := http.NewServeMux()

	mux.HandleFunc("/", func(w http.ResponseWriter, r *http.Request) {
		body, err := io.ReadAll(r.Body)
		if err != nil {
			log.Printf("[HTTP] Error reading body: %v", err)
			http.Error(w, "read error", 500)
			return
		}
		defer r.Body.Close()

		log.Printf("[HTTP] %s %s from %s (%d bytes)",
			r.Method, r.URL.Path, r.RemoteAddr, len(body))

		if len(body) > 0 {
			log.Printf("[HTTP] Body hex: %s", hex.EncodeToString(body))
			log.Printf("[HTTP] Body raw: %s", string(body))
		}

		for k, v := range r.Header {
			log.Printf("[HTTP] Header: %s = %v", k, v)
		}

		w.Header().Set("Content-Type", "application/octet-stream")
		fmt.Fprintf(w, "OK")
	})

	log.Printf("[HTTP] Listening on %s", addr)
	if err := http.ListenAndServe(addr, mux); err != nil {
		log.Fatal(err)
	}
}
