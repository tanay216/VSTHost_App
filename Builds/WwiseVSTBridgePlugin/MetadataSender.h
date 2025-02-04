#pragma once

#include <boost/asio.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <iostream>
#include <mutex>

class MetadataSender {
public:
    MetadataSender() : m_io(), m_socket(m_io) {
        connect();
    }

    ~MetadataSender() {
        disconnect();
    }

    void send(AkPlayingID id, const std::string& name) {
        std::lock_guard<std::mutex> lock(m_mutex); // Ensure thread safety

        if (!m_connected) {
            std::cerr << "Not connected to the server. Attempting to reconnect..." << std::endl;
            connect();
            if (!m_connected) {
                std::cerr << "Failed to send metadata: No connection to server." << std::endl;
                return;
            }
        }

        try {
            std::string msg = std::to_string(id) + "|" + name + "\n";
            boost::asio::write(m_socket, boost::asio::buffer(msg));
        }
        catch (const std::exception& e) {
            std::cerr << "Error sending metadata: " << e.what() << std::endl;
            disconnect(); // Disconnect on error to allow reconnection
        }
    }

private:
    void connect() {
        try {
            if (m_socket.is_open()) {
                m_socket.close();
            }

            boost::asio::ip::tcp::endpoint endpoint(
                boost::asio::ip::make_address("127.0.0.1"), 8081
            );

            m_socket.connect(endpoint);
            m_connected = true;
            std::cout << "Connected to metadata server." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Connection error: " << e.what() << std::endl;
            m_connected = false;
        }
    }

    void disconnect() {
        try {
            if (m_socket.is_open()) {
                m_socket.close();
            }
            m_connected = false;
            std::cout << "Disconnected from metadata server." << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "Error disconnecting: " << e.what() << std::endl;
        }
    }

    boost::asio::io_context m_io;
    boost::asio::ip::tcp::socket m_socket;
    std::mutex m_mutex;
    bool m_connected = false;
};