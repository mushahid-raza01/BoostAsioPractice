#include <iostream>
#include <boost/asio.hpp>

#define IP_ADDR "10.0.12.107"
#define PORT 1234

class ServerSide
{
public:
    ServerSide()
    {
        this->acceptor_ = new boost::asio::ip::tcp::acceptor(io_service, boost::asio::ip::tcp::endpoint
                                                            (boost::asio::ip::address::from_string
                                                            (IP_ADDR), PORT));

        this->socket_ = new boost::asio::ip::tcp::socket(io_service);
        this->acceptor_->accept(*socket_);
        std::cout << "System Initialized\n";
    }

    std::string receive() 
    {
       boost::asio::streambuf buf;
       boost::asio::read_until(*socket_, buf, "\0"); // '\0' is Delimiter
       return (boost::asio::buffer_cast<const char*>(buf.data()));
    }

    void send(const std::string& message)
    {
        const std::string msg = message + "\n";
        boost::asio::write(*socket_, boost::asio::buffer(message));
        std::cout << "Message Sent To Client Successfully\n";
    }

private:
    boost::asio::io_service io_service;
    boost::asio::ip::tcp::socket* socket_;
    boost::asio::ip::tcp::acceptor* acceptor_;
};

int main() 
{
    std::cout << "SERVER SIDE\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    ServerSide server;
    std::cout << "Msg From Client: " << server.receive() << "\n";
    server.send("Hello From Mushahid");
    return 0;
}