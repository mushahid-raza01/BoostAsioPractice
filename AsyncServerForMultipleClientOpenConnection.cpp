#include <boost/asio.hpp>
#include <thread>
#include <atomic>
#include <memory>
#include <iostream>

#define PORT 1234
using namespace boost;


class Service 
{
public:
	Service(std::shared_ptr<asio::ip::tcp::socket> sock) : m_sock(sock)
	{
		//Intentionally Left Blank Please don't mind
	}
	
	void StartHandling() 
	{
		while(true)
		{ 
			std::cout << "Reading..." << std::endl;
			asio::async_read_until(*m_sock.get(),m_request,'\n',	//Reading msg from client in to stream buffer
				[this](const boost::system::error_code& ec, std::size_t bytes_transferred)
				{
					onRequestReceived(ec, bytes_transferred);
				});

			this->printMsg();	//PRINTING THE MSG FROM CLIENT ON SCREEN

			// Process the request.
			m_response = ProcessRequest(m_request);
		
			// Initiate asynchronous write operation.
			std::cout << "Sending Response..." << std::endl;
			
			asio::async_write(*m_sock.get(), asio::buffer(m_response),
				[this](const boost::system::error_code& ec, std::size_t bytes_transferred) // Params to LAMBDA FUNC
				{
					onResponseSent(ec, bytes_transferred);
				});
			Sleep(1000);	//FOR READIBILITY
		}
	}

private:
	void onRequestReceived(const boost::system::error_code& ec, std::size_t bytes_transferred) //Read Handler
	{
		std::cout << "In Request Received..." << std::endl;
		if (ec) 
		{
			std::cout << "Error occured! Error code = "
				<< ec.value()
				<< ". Message: " << ec.message();
			onFinish();
		}
	}

	void onResponseSent(const boost::system::error_code& ec, std::size_t bytes_transferred) //Write Handler
	{
		if (ec) 
		{
			std::cout << "Error occured! Error code = "
				<< ec.value()
				<< ". Message: " << ec.message();
			onFinish();
		}
		std::cout << "Response Sent Successfully..." << std::endl;
	}

	// Here we perform the cleanup/ Calling the destructor CAN BE IMPROVED VIA FLAGS
	void onFinish() 
	{
		std::cout << "Closing Acceptor Socket..." << std::endl;
		this->~Service();
	}

	std::string ProcessRequest(asio::streambuf& request)	//SETTING MSGS
	{
		std::cout << "IN PROCESS REQUEST..." << std::endl;
		// Prepare and return the response message.
		std::string response = "Hail Hydra! Avengers Assemble\n";
		return response;
	}

	void printMsg()
	{
		std::cout << "Message from Client: "
				  << boost::asio::buffer_cast<const char*>(m_request.data()) << std::endl;
	}

	~Service()
	{
		delete this;
	}

private:
	std::shared_ptr<asio::ip::tcp::socket> m_sock;
	std::string m_response;	//Sending Buffer
	asio::streambuf m_request; // Receiving Buffer
};


class Acceptor 
{
public:

	Acceptor(asio::io_service& ios, unsigned short port_num): m_ios(ios), m_acceptor(m_ios,
			asio::ip::tcp::endpoint(asio::ip::address_v4::any(), port_num)), m_isStopped(false)
	{	
		//Left Blank Intentionally
	}

	// Start accepting incoming connection requests.
	void Start() 
	{
		std::cout << "Server Is Listening on Acceptor..." << std::endl;
		m_acceptor.listen();	//Opening the socket for listening for incoming clients
		InitAccept();
	}

	// Stop accepting incoming connection requests.
	void Stop() 
	{
		m_isStopped.store(true);
	}

private:

	void InitAccept() 
	{
		std::cout << "Initialized Socket" << std::endl;
		std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(m_ios));
		
		m_acceptor.async_accept(*sock.get(),
			[this, sock](const boost::system::error_code& error)	//LAMBDA FUNCTION
				{
					onAccept(error, sock);	//CALLING FUNCTION TO CREATE NEW SOCKETS
				});
	}

	void onAccept(const boost::system::error_code& ec, std::shared_ptr<asio::ip::tcp::socket> sock)
	{
		std::cout << "Socket is ready to Read..." << std::endl;
		
		if (!ec) 
		{
			(new Service(sock))->StartHandling();	//Starting to Read & Write on NEW ACTIVE Sockets
		}

		else 
		{
			std::cout << "Error occured! Error code = "
				<< ec.value()
				<< ". Message: " << ec.message();
		}

		// Init next async accept operation if
		// acceptor has not been stopped yet.
		if (!m_isStopped.load()) //INCASE STOP() IS CALLED KILL THE SERVICE
		{
			InitAccept();
		}

		else 
		{
			// Stop accepting incoming connections
			// and free allocated resources.
			m_acceptor.close();
		}
	}

private:

	asio::io_service& m_ios;
	asio::ip::tcp::acceptor m_acceptor;
	std::atomic<bool>m_isStopped;
};


class Server {
public:
	Server() 
	{
		m_work.reset(new asio::io_service::work(m_ios));
	}
	
	// Start the server.
	void Start(unsigned short port_num,	unsigned int thread_pool_size) 
	{
		std::cout << "Server Starting..." << std::endl;
		assert(thread_pool_size > 0);

		// Create and start Acceptor.
		acc.reset(new Acceptor(m_ios, port_num));
				
		// Create specified number of threads and
		// add them to the pool.
		for (unsigned int i = 0; i < thread_pool_size; i++) 
		{
			//std::cout << "IS THIS EVEN RUNNING?..." << std::endl;
			std::unique_ptr<std::thread> th(
				new std::thread([this]() //this ptr passed so that we can access m_ios
					{
						acc->Start();
						m_ios.run(); //Blocking Threads to end the program together
					}));
			m_thread_pool.push_back(std::move(th)); //Storing Threads PTRs in Vector
		}
	}
	// Stop the server.
	void Stop() 
	{
		acc->Stop();
		m_ios.stop();
		for (auto& th : m_thread_pool) 
		{
			th->join();
		}
	}

private:
	asio::io_service m_ios;
	std::unique_ptr<asio::io_service::work>m_work;
	std::unique_ptr<Acceptor>acc;
	std::vector<std::unique_ptr<std::thread>>m_thread_pool;
};

const unsigned int DEFAULT_THREAD_POOL_SIZE = 2;

int main()
{
	try 
	{
		Server srv;
		unsigned int thread_pool_size = std::thread::hardware_concurrency() * 2;
		std::cout << "MAX THREADS: " << thread_pool_size << std::endl;

		if (thread_pool_size == 0)
		{
			thread_pool_size = DEFAULT_THREAD_POOL_SIZE;
		}

		srv.Start(PORT, thread_pool_size);
		std::this_thread::sleep_for(std::chrono::seconds(10));
		srv.Stop();

		std::cout << "TIME's UP KILLING THE SERVER" << std::endl;
	}

	catch (system::system_error& e) 
	{
		std::cout << "Error occured! Error code = "
			<< e.code() << ". Message: "
			<< e.what();
	}

	return 0;
}