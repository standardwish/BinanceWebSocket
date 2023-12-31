﻿#include <boost/beast/core.hpp>
#include <boost/beast/ssl.hpp>
#include <boost/beast/websocket.hpp>
#include <boost/beast/websocket/ssl.hpp>
#include <boost/asio/connect.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/ssl/stream.hpp>
#include <sstream>  
#include <iomanip>  
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <atlstr.h>
#include <nlohmann/json.hpp>
#include <conio.h>

// Namespace aliases for Boost.Beast, Boost.Asio, and JSON library
namespace beast = boost::beast;
namespace http = beast::http;
namespace websocket = beast::websocket;
namespace net = boost::asio;
namespace ssl = boost::asio::ssl;
using tcp = boost::asio::ip::tcp;
using json = nlohmann::json;

// WebSocketClient class for managing WebSocket connections
class WebSocketClient {
public:
    WebSocketClient(net::io_context& ioc, ssl::context& ctx)
        : resolver_(ioc), ws_(ioc, ctx) {}

    // Method to establish a WebSocket connection
    void connect(const std::string& host, const std::string& port, const std::string& target) {
        auto const results = resolver_.resolve(host, port);
        net::connect(ws_.next_layer().next_layer(), results.begin(), results.end());
        ws_.next_layer().handshake(ssl::stream_base::client);

        // Set WebSocket options and perform handshake
        ws_.set_option(websocket::stream_base::decorator(
            [](websocket::request_type& req)
            {
                req.set(http::field::user_agent,
                std::string(BOOST_BEAST_VERSION_STRING) +
                " binance-websocket-client");
            }));
        ws_.handshake(host, target);
        std::cout << "CONNECTION SUCCESS" << std::endl;
    }

    // Method to send a message over the WebSocket connection
    void send(const std::string& message) {
        ws_.write(net::buffer(message));
    }

    // Method to receive data from the WebSocket connection
    void receive() {
        ws_.read(buffer_);
    }

    // Method to process and write received data to a file
    void write() {
        std::ofstream file("stream-data-binance.txt", std::ios_base::app);
        if (file.is_open()) {
            std::string bufferData_ = beast::buffers_to_string(buffer_.data());
            try {
                auto bufferData_parsed_ = json::parse(bufferData_);
                if (bufferData_parsed_.contains("p")) {
                    std::cout << "btcusdt@trade,p:" << bufferData_parsed_["p"] << ",q:" << bufferData_parsed_["q"] << std::endl;
                    file << "btcusdt@trade,p:" << bufferData_parsed_["p"] << ",q:" << bufferData_parsed_["q"] << '\n';
                }
                else if (bufferData_parsed_.contains("lastUpdateId")) {
                    std::cout << "btcusdt@orderbook," << "bids:" << bufferData_parsed_["bids"] << std::endl;
                    file << "btcusdt@orderbook," << "bids:" << bufferData_parsed_["bids"] << '\n';
                }
                else if (bufferData_parsed_.contains("result")) {
                    std::cout << "MESSAGE START" << std::endl;
                }
            }
            catch (const json::parse_error& e) {
                std::cout << "message: " << e.what() << '\n'
                    << "exception id: " << e.id << '\n'
                    << "byte position of error: " << e.byte << std::endl;
            }
            buffer_.clear();

        }
        else {
            std::cout << "File Error" << std::endl;
        }
        file.close();
    }

    // Method to close the WebSocket connection
    void close() {
        ws_.close(websocket::close_code::normal);
        std::cout << "CONNECTION CLOSE" << std::endl;
    }

private:
    boost::beast::flat_buffer buffer_;
    net::ip::tcp::resolver resolver_;
    websocket::stream<beast::ssl_stream<tcp::socket>> ws_;
};

// BinanceWebSocketClient class for managing Binance-specific WebSocket functionality
class BinanceWebSocketClient {
public:
    BinanceWebSocketClient(net::io_context& ioc, ssl::context& ctx, const std::string& host, const std::string& port, const std::string& param)
        : wsClient(ioc, ctx), host_(host), port_(port), param_(param), target_("/ws/" + param) {}

    // Method to run the WebSocket client
    void run() {
        try {
            wsClient.connect(host_, port_, target_);

            // Subscribe to Binance WebSocket channels
            const std::string subscribeMessage = R"({"method": "SUBSCRIBE", "params": ["btcusdt@depth5", "btcusdt@trade"], "id": 1})";
            wsClient.send(subscribeMessage);

            // Main loop for receiving and processing WebSocket data
            while (true) {
                wsClient.receive();
                wsClient.write();
            }

            // Close the WebSocket connection (unreachable in this code because of the infinite loop)
            wsClient.close();
        }
        catch (std::exception const& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }

private:
    WebSocketClient wsClient;
    std::string host_;
    std::string port_;
    std::string param_;
    std::string target_;
};

// Main function
int main()
{
    const std::string host = "stream.binance.com";
    const std::string port = "443";
    const std::string param = "btcusdt";

    // Create I/O context and SSL context for the WebSocket client
    net::io_context ioc;
    ssl::context ctx{ ssl::context::tlsv12_client };

    // Create and run the BinanceWebSocketClient
    BinanceWebSocketClient client(ioc, ctx, host, port, param);
    client.run();
}