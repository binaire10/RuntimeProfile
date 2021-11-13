#ifndef RUNTIMEPROFILE_PROFILE_HPP
#define RUNTIMEPROFILE_PROFILE_HPP

#include <string_view>
#include <ostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <memory>
#ifdef HAS_BOOST
#    include <boost/asio.hpp>
#    include <utility>
#endif

namespace Profile
{
    class sink;

    using duration = std::chrono::duration<float, std::micro>;

    class NullMutex {
    public:
        constexpr void lock() {}
        constexpr void unlock() {}
    };

    struct ScopeProfile
    {
        std::string_view name;
        std::thread::id  threadId;
        duration         start;
        duration         elapsedTime;
    };

    class Logger
    {
    public:
        Logger(std::initializer_list<std::shared_ptr<sink>> &&list)
            : sinks{ std::move(list) }
        {}

        void submit(const ScopeProfile &profile);
        void register_sink(std::shared_ptr<sink> sink);

    private:
        std::vector<std::shared_ptr<sink>> sinks;
    };

    void submit(const ScopeProfile &profile);
    void register_sink(std::shared_ptr<sink> sink);

    class sink
    {
    public:
        virtual void capture(const Profile::ScopeProfile &) = 0;
    };

    template<typename MutexT>
    class synchronized_sink : public sink
    {
    public:
        void capture(const Profile::ScopeProfile &c) override
        {
            std::lock_guard<MutexT> guard{ m_mutex };
            capture_profile(c);
        }

    protected:
        virtual void capture_profile(const Profile::ScopeProfile &) = 0;

    private:
        MutexT m_mutex;
    };

    struct json_coder
    {
        static void begin(std::ostream &out);
        static void end(std::ostream &out);
        static void write(std::ostream &out, const Profile::ScopeProfile &result);
    };

    template<typename MutexT, typename CoderT>
    class stream_sink : public synchronized_sink<MutexT>
    {
    public:
        explicit stream_sink(std::ostream &sout)
            : out(&sout)
        {
            CoderT::begin(*out);
        }

        ~stream_sink() { CoderT::end(*out); }

    protected:
        void capture_profile(const Profile::ScopeProfile &result) override { CoderT::write(*out, result); }

    private:
        std::ostream *out;
    };

    template<typename MutexT>
    using stream_json_sink = stream_sink<MutexT, json_coder>;
#ifdef HAS_BOOST
    namespace net = boost::asio;

    template<typename MutexT, typename CoderT>
    class udp_stream : public synchronized_sink<MutexT>
    {
    public:
        udp_stream(net::io_service &service, net::ip::udp::endpoint endpoint)
            : m_socket{ service, net::ip::udp::v4() }
            , m_endpoint{ std::move(endpoint) }
        {
            m_socket.set_option(boost::asio::ip::multicast::enable_loopback(true));
            service.run();
        }

    protected:
        void capture_profile(const Profile::ScopeProfile &result) override
        {
            std::ostream s(&d);
            CoderT::write(s, result);
            std::string data = d.str();
            m_socket.send_to(boost::asio::buffer(data), m_endpoint);
            d.str(std::string{});
        }

    private:
        std::stringbuf         d;
        net::ip::udp::socket   m_socket;
        net::ip::udp::endpoint m_endpoint;
    };
#endif


    template<typename ChronoT>
    struct RecordProfile
    {
        using chrono_type = ChronoT;
        using time_point  = typename chrono_type::time_point;

        inline RecordProfile(Logger &logger, std::string_view name)
            : logger{logger}, start{ chrono_type::now() }
            , capture{ name, std::this_thread::get_id() }
        {}

        ~RecordProfile()
        {
            auto end = chrono_type::now();
            capture.start       = std::chrono::duration_cast<Profile::duration>(start.time_since_epoch());
            capture.elapsedTime = std::chrono::duration_cast<Profile::duration>(end - start);
            logger.submit(capture);
        }

        Logger &logger;
        time_point            start;
        Profile::ScopeProfile capture;
    };
}// namespace Profile


#endif// RUNTIMEPROFILE_PROFILE_HPP
