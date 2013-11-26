#include <iostream>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

class tcp_server : public boost::enable_shared_from_this<tcp_server>
{
private:

	enum 
	{
		HEADER_SIZE = 4,
		DATA_SIZE   = 10,
		BUFFER_SIZE = 1024,
		MAX_STAMP   = 50
	};
	typedef boost::asio::ip::tcp tcp;

public:
	

public:
	tcp_server( boost::asio::io_service& p_Service, unsigned short p_Port) 
		:	m_Strand(p_Service),
			m_Acceptor(p_Service, tcp::endpoint( tcp::v4(), p_Port)),
			m_Socket(p_Service),
			m_Index(0){};

	void start();
	bool record_stamp();

private:
	void handle_accept( const boost::system::error_code& error);
	void read_header();
	void handle_read_header(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);
	void handle_read_data(const boost::system::error_code& p_Error, std::size_t p_BytesTransferred);

private:
  boost::asio::io_service::strand m_Strand;
  tcp::acceptor m_Acceptor;
  tcp::socket m_Socket;
  unsigned int m_Index;
};