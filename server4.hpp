#pragma once

#include <boost/asio.hpp>
#include <memory>
#include <thread>
#include <vector>

class ThreadPoolServer {
public:
    ThreadPoolServer(boost::asio::io_context& io, unsigned short port, size_t thread_pool_size);
    void run();
    
private:
    void start_accept();
    void handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, 
                      const boost::system::error_code& error);

    boost::asio::io_context& io_context_;
    boost::asio::ip::tcp::acceptor acceptor_;
    std::vector<std::thread> threads_;
    boost::asio::strand<boost::asio::io_context::executor_type> strand_;
};
