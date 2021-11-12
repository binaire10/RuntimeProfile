#include <iostream>
#include <Profile/Profile.hpp>
#include <boost/asio.hpp>

namespace net = boost::asio;
namespace ip  = net::ip;

int main(int argc, const char **argv)
{
    if(argc < 4)
    {
        std::cout << argv[0] << " <interface> <multicast> <port>\n";
        return -1;
    }
    net::io_service service;
    ip::udp::socket socket{ service };
    auto            interface = boost::asio::ip::make_address(argv[1]);
    auto            groupCast = boost::asio::ip::make_address(argv[2]);

    socket.open(ip::udp::v4());
    socket.set_option(ip::udp::socket::reuse_address(true));
    socket.set_option(ip::multicast::enable_loopback(true));
    socket.bind(ip::udp::endpoint{ interface, static_cast<uint16_t>(std::strtoul(argv[3], nullptr, 10)) });
    socket.set_option(ip::multicast::join_group(groupCast));
    service.run();

    Profile::json_coder::begin(std::cout);

    for(int i = 0; i < 10; ++i)
    {
        char              buffer[2048];
        ip::udp::endpoint addr;
        std::size_t       len = socket.receive_from(net::buffer(buffer), addr);
        std::cout.write(buffer, len);
    }
    Profile::json_coder::end(std::cout);
    socket.set_option(ip::multicast::leave_group(groupCast));
}