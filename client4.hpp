#pragma once

#include <boost/asio.hpp>

class AsyncClient {
public:
    AsyncClient(boost::asio::io_context& io, const std::string& host, const std::string& port);
    void send_request(const std::string& request);
    
private:
    void handle_connect(const boost::system::error_code& error);
    void handle_write(const boost::system::error_code& error);
    void handle_read(const boost::system::error_code& error, std::size_t bytes_transferred);

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::socket socket_;
    boost::asio::ip::tcp::resolver resolver_;
    boost::asio::streambuf buffer_;
    std::string host_;
    std::string port_;
};
