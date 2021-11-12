#include <iostream>
#include <chrono>
#include <Profile/SteadyClockProfiler.hpp>


Profile::Logger *logger_;

#define PROFILE_FUNCTION() STEADY_CLOCK_PROFILE_FUNCTION(*logger_)

using namespace std::chrono_literals;

void foo()
{
    PROFILE_FUNCTION();

    std::this_thread::sleep_for(1s);
}

int main()
{
    boost::asio::io_service service;
    Profile::Logger         logger{
        std::make_shared<Profile::stream_json_sink<Profile::NullMutex>>(std::cout)
#ifdef HAS_BOOST
            ,
        std::make_shared<Profile::udp_stream<Profile::NullMutex, Profile::json_coder>>(
            service, boost::asio::ip::udp::endpoint{ boost::asio::ip::make_address("239.255.43.21"), 55000 })
#endif
    };
    logger_ = &logger;

    PROFILE_FUNCTION();

    std::this_thread::sleep_for(1s);
    foo();
}
