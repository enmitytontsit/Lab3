#include "server.hpp"
#include <iostream>

ThreadPoolServer::ThreadPoolServer(boost::asio::io_context& io, 
                                 unsigned short port, 
                                 size_t thread_pool_size)
    : io_context_(io),
      acceptor_(io, boost::asio::ip::tcp::endpoint(boost::asio::ip::tcp::v4(), port)),
      strand_(boost::asio::make_strand(io)) {
    
    start_accept();
    
    threads_.reserve(thread_pool_size);
    for (size_t i = 0; i < thread_pool_size; ++i) {
        threads_.emplace_back([this]() {
            io_context_.run();
        });
    }
}

void ThreadPoolServer::run() {
    for (auto& thread : threads_) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

void ThreadPoolServer::start_accept() {
    auto socket = std::make_shared<boost::asio::ip::tcp::socket>(io_context_);
    
    acceptor_.async_accept(*socket,
        [this, socket](const boost::system::error_code& error) {
            boost::asio::post(strand_,
                [this, socket, error]() {
                    handle_accept(socket, error);
                });
            
            if (!error) {
                start_accept();
            }
        });
}

void ThreadPoolServer::handle_accept(std::shared_ptr<boost::asio::ip::tcp::socket> socket, 
                                   const boost::system::error_code& error) {
    if (!error) {
        std::cout << "New connection from: " << socket->remote_endpoint() << std::endl;
        
        auto buffer = std::make_shared<boost::asio::streambuf>();
        boost::asio::async_read_until(*socket, *buffer, '\n',
            boost::asio::bind_executor(strand_,
                [this, socket, buffer](const boost::system::error_code& ec, std::size_t length) {
                    if (!ec) {
                        std::istream is(buffer.get());
                        std::string line;
                        std::getline(is, line);
                        
                        std::string response = "Processed: " + line + "\n";
                        
                        boost::asio::async_write(*socket, boost::asio::buffer(response),
                            boost::asio::bind_executor(strand_,
                                [socket](const boost::system::error_code& ec, std::size_t) {
                                    if (ec) {
                                        std::cerr << "Write error: " << ec.message() << std::endl;
                                    }
                                }));
                    }
                }));
    }
}
