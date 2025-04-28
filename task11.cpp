#include <boost/asio.hpp>
#include <iostream>
#include <algorithm>
#include <string>

using namespace boost::asio;
using namespace boost::asio::ip;

void process_client(tcp::socket& socket) {
    try {
        streambuf buffer;
        read_until(socket, buffer, '\n');
        
        std::istream is(&buffer);
        std::string input;
        std::getline(is, input);
        
        std::string output = input;
        std::transform(output.begin(), output.end(), output.begin(), ::toupper);
        output = std::to_string(output.length()) + ": " + output + "\n";
        
        write(socket, boost::asio::buffer(output));
    } catch (const std::exception& e) {
        std::cerr << "Ошибка обработки клиента: " << e.what() << std::endl;
    }
}

int main() {
    try {
        io_context io;
        tcp::acceptor acceptor(io, tcp::endpoint(tcp::v4(), 1234));
        
        std::cout << "Сервер запущен на порту 1234" << std::endl;
        
        while (true) {
            tcp::socket socket(io);
            acceptor.accept(socket);
            std::cout << "Новое подключение: " 
                      << socket.remote_endpoint() << std::endl;
            
            process_client(socket);
        }
    } catch (const std::exception& e) {
        std::cerr << "Ошибка сервера: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
