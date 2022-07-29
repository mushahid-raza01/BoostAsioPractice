#include <iostream>
#include <boost/asio.hpp>
#include <Windows.h>

using namespace boost::asio;
using ip::tcp;
using std::string;
using std::cout;
using std::endl;

int main() {

    cout << "CLIENT SIDE\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n\n";
    string msg="HELLO FROM MUSHAHID";
   
    boost::asio::io_service io_service;
    tcp::socket socket(io_service);
    socket.connect(tcp::endpoint(boost::asio::ip::address::from_string("10.0.12.107"), 1234));
    boost::system::error_code error;
    for(int i=0;i<5;i++)
    {
        
        msg += i;
        
        boost::asio::write(socket, boost::asio::buffer(msg), error);
        if (!error) {
            cout << "Message Sent Successfully!" << endl;
        }
        else {
            cout << "Sending failed: " << error.message() << endl;
        }
        
        // getting response from server
        boost::asio::streambuf receive_buffer;
        boost::asio::read_until(socket, receive_buffer,'\n', error);
        if (error && error != boost::asio::error::eof) {
            cout << "Receive Failed: " << error.message() << endl;
        }

        else {
            const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
            cout << "Response From Server: " << data << endl;
        }

        Sleep(1000);

    }


    return 0;
}