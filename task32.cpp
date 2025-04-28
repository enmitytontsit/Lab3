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
            std::cout << "Введите команду (timer N или exit): ";
            std::string input;
            std::getline(std::cin, input);
            
            if (input == "exit") break;
            
            input += "\n";
            write(socket, buffer(input));
            
            // Читаем первый ответ (подтверждение)
            streambuf response;
            read_until(socket, response, '\n');
            std::istream is(&response);
            std::string line;
            std::getline(is, line);
            std::cout << "Сервер: " << line << "\n";
            
            // Если это была команда timer, ждем второй ответ
            if (input.find("timer") == 0) {
                streambuf done_response;
                read_until(socket, done_response, '\n');
                std::istream is_done(&done_response);
                std::string done_line;
                std::getline(is_done, done_line);
                std::cout << "Сервер: " << done_line << "\n";
            }
        }
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
    return 0;
}
