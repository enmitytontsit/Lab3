#include <boost/asio.hpp>
#include <iostream>
#include <vector>
#include <algorithm>
#include <sstream>

using namespace boost::asio;
using namespace boost::asio::ip;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) : socket_(std::move(socket)) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t length) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string line;
                    std::getline(is, line);
                    
                    // Асинхронная обработка
                    post(socket_.get_executor(), 
                        [this, self, line]() {
                            process_data(line);
                        });
                }
            });
    }

    void process_data(const std::string& data) {
        std::istringstream iss(data);
        std::vector<int> numbers;
        int num;
        
        while (iss >> num) {
            numbers.push_back(num);
        }
        
        auto self(shared_from_this());  // Добавлено получение shared_ptr
        
        if (!numbers.empty()) {
            int max_num = *std::max_element(numbers.begin(), numbers.end());
            std::string response = "Максимум: " + std::to_string(max_num) + "\n";
            
            async_write(socket_, buffer(response),
                [this, self](boost::system::error_code ec, std::size_t) {
                    if (!ec) {
                        do_read();
                    }
                });
        } else {
            std::string response = "Ошибка: нет чисел для обработки\n";
            async_write(socket_, buffer(response),
                [this, self](boost::system::error_code ec, std::size_t) {});
        }
    }

    tcp::socket socket_;
    streambuf buffer_;
};

class Server {
public:
    Server(io_context& io, short port)
        : acceptor_(io, tcp::endpoint(tcp::v4(), port)) {
        do_accept();
    }

private:
    void do_accept() {
        acceptor_.async_accept(
            [this](boost::system::error_code ec, tcp::socket socket) {
                if (!ec) {
                    std::make_shared<Session>(std::move(socket))->start();
                }
                do_accept();
            });
    }

    tcp::acceptor acceptor_;
};

int main() {
    try {
        io_context io;
        Server server(io, 1234);
        std::cout << "Сервер запущен на порту 1234\n";
        io.run();
    } catch (std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
    }
    return 0;
}
