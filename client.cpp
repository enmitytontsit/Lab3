#include "client.hpp"
#include <iostream>

AsyncClient::AsyncClient(boost::asio::io_context& io, 
                       const std::string& host, 
                       const std::string& port)
    : io_context_(io),
      socket_(io),
      resolver_(io),
      host_(host),
      port_(port) {
    
    boost::asio::ip::tcp::resolver::query query(host, port);
    resolver_.async_resolve(query,
        [this](const boost::system::error_code& ec, 
               boost::asio::ip::tcp::resolver::results_type endpoints) {
            if (!ec) {
                boost::asio::async_connect(socket_, endpoints,
                    [this](const boost::system::error_code& ec, 
                           const boost::asio::ip::tcp::endpoint&) {
                        if (ec) {
                            std::cerr << "Connect error: " << ec.message() << std::endl;
                        }
                    });
            }
        });
}

void AsyncClient::send_request(const std::string& request) {
    boost::asio::async_write(socket_, boost::asio::buffer(request + "\n"),
        [this](const boost::system::error_code& ec, std::size_t) {
            if (!ec) {
                boost::asio::async_read_until(socket_, buffer_, '\n',
                    [this](const boost::system::error_code& ec, std::size_t bytes) {
                        if (!ec) {
                            std::istream is(&buffer_);
                            std::string line;
                            std::getline(is, line);
                            std::cout << "Server response: " << line << std::endl;
                        }
                    });
            }
        });
}
