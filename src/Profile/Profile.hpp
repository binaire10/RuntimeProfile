#ifndef RUNTIMEPROFILE_PROFILE_HPP
#define RUNTIMEPROFILE_PROFILE_HPP

#include <string_view>
#include <ostream>
#include <iomanip>
#include <thread>
#include <mutex>

namespace Profile
{
    class sink;

    using duration_type = std::chrono::duration<float, std::micro>;

    struct ScopeProfile
    {
        std::string_view name;
        std::thread::id  threadId;
        duration_type    start;
        duration_type    elapsedTime;
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

    template<typename MutexT>
    class stream_json_sink : public synchronized_sink<MutexT>
    {
    public:
        stream_json_sink(std::ostream &out)
            : out(out)
        {
            out << R"({"otherData":{},"traceEvents":[{})";
        }

        ~stream_json_sink() { out << "]}"; }

    protected:
        void capture_profile(const Profile::ScopeProfile &result) override
        {
            out << std::setprecision(3) << std::fixed;
            out << ",{";
            out << R"("cat":"function",)";
            out << R"("dur":)" << result.elapsedTime.count() << ',';
            out << R"("name":")" << result.name << "\",";
            out << R"("ph":"X",)";
            out << R"("pid":0,)";
            out << R"("tid":)" << result.threadId << ",";
            out << R"("ts":)" << result.start.count();
            out << "}";
        }

    private:
        std::ostream &out;
    };


    template<typename ChronoT>
    struct RecordProfile
    {
        using chrono_type = ChronoT;
        using time_point  = typename chrono_type::time_point;

        inline explicit RecordProfile(std::string_view name)
            : start{ chrono_type::now() }
            , capture{ name, std::this_thread::get_id() }
        {}

        ~RecordProfile()
        {
            capture.start       = start.time_since_epoch();
            capture.elapsedTime = std::chrono::duration_cast<Profile::duration_type>(chrono_type::now() - start);
            Profile::submit(capture);
        }

        time_point            start;
        Profile::ScopeProfile capture;
    };
}// namespace Profile


#endif// RUNTIMEPROFILE_PROFILE_HPP
