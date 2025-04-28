#include <boost/asio.hpp>
#include <iostream>
#include <memory>
#include <sstream>

using namespace boost::asio;
using namespace boost::asio::ip;

class Session : public std::enable_shared_from_this<Session> {
public:
    Session(tcp::socket socket) 
        : socket_(std::move(socket)), 
          timer_(socket_.get_executor()) {}

    void start() {
        do_read();
    }

private:
    void do_read() {
        auto self(shared_from_this());
        async_read_until(socket_, buffer_, '\n',
            [this, self](boost::system::error_code ec, std::size_t) {
                if (!ec) {
                    std::istream is(&buffer_);
                    std::string command;
                    std::getline(is, command);
                    process_command(command);
                }
            });
    }

    void process_command(const std::string& command) {
        auto self(shared_from_this());
        std::istringstream iss(command);
        std::string cmd;
        int delay_sec = 0;

        iss >> cmd >> delay_sec;

        if (cmd == "timer" && delay_sec > 0) {
            // Отправляем немедленный ответ
            std::string immediate_response = "Ready in " + std::to_string(delay_sec) + " sec\n";
            async_write(socket_, buffer(immediate_response),
                [this, self, delay_sec](boost::system::error_code ec, std::size_t) {
                    if (!ec) {
                        // Устанавливаем таймер
                        timer_.expires_after(std::chrono::seconds(delay_sec));
                        timer_.async_wait(
                            [this, self](const boost::system::error_code& ec) {
                                if (!ec) {
                                    // Отправляем отложенное сообщение
                                    async_write(socket_, buffer("Done!\n"),
                                        [this, self](boost::system::error_code ec, std::size_t) {
                                            if (!ec) {
                                                do_read(); // Ждем следующую команду
                                            }
                                        });
                                }
                            });
                    }
                });
        } else {
            // Обычная эхо-логика для других команд
            async_write(socket_, buffer("Unknown command\n"),
                [this, self](boost::system::error_code ec, std::size_t) {
                    if (!ec) {
                        do_read();
                    }
                });
        }
    }

    tcp::socket socket_;
    streambuf buffer_;
    steady_timer timer_;
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
