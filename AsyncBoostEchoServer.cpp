//importing libraries
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

using namespace boost::asio;


#define PORT 1234


class ConnectionHandler : public boost::enable_shared_from_this<ConnectionHandler> //SHARED POINTER
{
private:
    ip::tcp::socket sock;
    boost::asio::streambuf data; //BUFFER FOR READING DATA ON SOCKET
   
public:
   
    typedef boost::shared_ptr<ConnectionHandler> pointer; //Renaming coz the name is too long.... Hehehe!
   

    ConnectionHandler(boost::asio::io_service& io_service) : sock(io_service) //CONSTRUCTOR
    {
        //PURPOSELY EMPTY
    }
    
    //Creating the pointer to this Class
    static pointer create(boost::asio::io_service& io_service) //UTILITY FUNCTION
    {
        return pointer(new ConnectionHandler(io_service));
    }
   
    ip::tcp::socket& socket()   //Socket Getter
    {
        return sock;
    }

    void read() //READ FUNCTION
    {
        async_read_until(sock,data,"\0",
            boost::bind(&ConnectionHandler::handle_read,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    void write(std::string message) //WRITE FUNCTION
    {
        async_write(this->sock,
            boost::asio::buffer(message),
            boost::bind(&ConnectionHandler::handle_write,
            shared_from_this(),
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
    }

    std::string getMessageData()  
    {
        return(boost::asio::buffer_cast<const char*>(data.data())); //TYPE CASTING STREAM BUFFER TO STD::STRING
    }

    void handle_read(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) 
            std::cout << "Message From Client: " << this->getMessageData() << std::endl;

        else 
            std::cerr << "error: " << err.message() << std::endl;
    }

    void handle_write(const boost::system::error_code& err, size_t bytes_transferred)
    {
        if (!err) 
            std::cout << "Server's Response Sent Successfully!" << std::endl;
    
        else 
            std::cerr << "error: " << err.message() << std::endl;

    }

    ~ConnectionHandler()  //Destructor to close SOCKET
    {
        sock.close();
    }
};

class Server
{
private:
    ip::tcp::acceptor acceptor_;
    boost::asio::io_service *io_service;

    void start_accept()
    {
        std::cout << "Ready To Accept Connections:" << std::endl;

        //Client socket
        ConnectionHandler::pointer connection = ConnectionHandler::create(*io_service);

        //Asynchronous accept operation and wait for a new connection.
        
        
        acceptor_.async_accept(connection->socket(),
            boost::bind(&Server::handle_accept, this, connection,
            boost::asio::placeholders::error));        
    }

public:
    //Constructor for accepting connection from client
    
    Server(boost::asio::io_service& io_service) : acceptor_(io_service, ip::tcp::endpoint(ip::tcp::v4(), PORT))
    {
        this->io_service = &io_service; //Work Around as acceptor.getioservice() is not available anymore
        start_accept();
    }

    void handle_accept(ConnectionHandler::pointer connection, const boost::system::error_code& err)
    {
        if (!err) {
            connection->read();
            connection->write("Welcome To The DarkSide!");
        }
        start_accept();
    }

};

int main()
{
    try
    {
        boost::asio::io_service io_service;
        Server server(io_service);
        io_service.run();
    }

    catch (std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    return 0;
}

