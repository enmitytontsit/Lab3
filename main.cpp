#include "server.hpp"
#include "client.hpp"
#include <boost/asio.hpp>
#include <thread>
#include <memory>

int main() {
    try {
        const unsigned short port = 1234;
        const size_t thread_pool_size = 4;
        
        boost::asio::io_context io;
        
        ThreadPoolServer server(io, port, thread_pool_size);
        
        std::thread client_thread([&io]() {
            boost::asio::io_context client_io;
            AsyncClient client(client_io, "127.0.0.1", "1234");
            
            std::this_thread::sleep_for(std::chrono::seconds(1));
            
            client.send_request("Hello");
            client.send_request("Multithreaded");
            client.send_request("Server");
            
            client_io.run();
        });
        
        server.run();
        client_thread.join();
        
    } catch (std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
    }
    
    return 0;
}
