#include <boost/asio.hpp>
#include <iostream>

using namespace boost::asio;
using namespace boost::asio::ip;

int main() {
    try {
        io_context io;
        tcp::socket socket(io);
        tcp::resolver resolver(io);
        connect(socket, resolver.resolve("127.0.0.1", "1234"));
     
        std::string message;
        std::cout << "Введите строку: ";
        std::getline(std::cin, message);
        message += "\n"; 

        write(socket, buffer(message));

        streambuf response;
        read_until(socket, response, '\n');
        
        std::cout << "Ответ сервера: ";
        std::cout << &response;
    } catch (const std::exception& e) {
        std::cerr << "Ошибка клиента: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
