#pragma once
#include <boost/asio.hpp>
#include <AK/SoundEngine/Common/AkTypes.h>
#include <thread>
#include <atomic>
#include <iostream>

class MetadataReceiver {
public:
    MetadataReceiver() : m_workGuard(boost::asio::make_work_guard(m_io)) {
        startListening();
    }

    ~MetadataReceiver() {
        stopListening();
    }

    void startListening() {
        m_running = true;
        m_listenerThread = std::thread([this]() {
            try {
                boost::asio::ip::tcp::acceptor acceptor(m_io,
                    boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), 8081));

                while (m_running) {
                    boost::asio::ip::tcp::socket socket(m_io);

                    // Use non-blocking accept with timeout
                    boost::asio::deadline_timer timer(m_io);
                    bool accepted = false;

                    acceptor.async_accept(socket, [&](const boost::system::error_code& ec) {
                        if (!ec) accepted = true;
                        timer.cancel();
                        });

                    timer.expires_from_now(boost::posix_time::milliseconds(100));
                    timer.async_wait([&](const boost::system::error_code&) {
                        acceptor.cancel();
                        });

                    m_io.run_one();

                    if (accepted) {
                        handleConnection(std::move(socket));
                    }
                }
            }
            catch (const std::exception& e) {
                std::cerr << "Listener error: " << e.what() << std::endl;
            }
            });
    }

    void stopListening() {
        m_running = false;
        m_io.stop();
        if (m_listenerThread.joinable()) {
            m_listenerThread.join();
        }
    }

private:
    void handleConnection(boost::asio::ip::tcp::socket socket) {
        try {
            boost::asio::streambuf buffer;
            boost::system::error_code ec;

            // Read until delimiter
            boost::asio::read_until(socket, buffer, '\n', ec);

            if (!ec) {
                std::istream is(&buffer);
                std::string msg;
                std::getline(is, msg);

                // Parse message
                size_t delimiterPos = msg.find('|');
                if (delimiterPos != std::string::npos) {
                    AkPlayingID id = static_cast<AkPlayingID>(std::stoull(msg.substr(0, delimiterPos)));
                    std::string name = msg.substr(delimiterPos + 1);

                    // Handle the parsed metadata
                    std::cout << "Received voice metadata - ID: " << id
                        << ", Name: " << name << std::endl;
                }
            }
        }
        catch (const std::exception& e) {
            std::cerr << "Connection error: " << e.what() << std::endl;
        }
    }

    boost::asio::io_context m_io;
    boost::asio::executor_work_guard<boost::asio::io_context::executor_type> m_workGuard;
    std::thread m_listenerThread;
    std::atomic<bool> m_running{ false };
};