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

        while (true) {
            std::cout << "Введите числа через пробел (или 'exit' для выхода): ";
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "exit") break;
            
            input += "\n";
            write(socket, buffer(input));
            
            streambuf response;
            read_until(socket, response, '\n');
            
            std::istream is(&response);
            std::string line;
            std::getline(is, line);
            std::cout << "Ответ сервера: " << line << "\n";
        }
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
    return 0;
}
