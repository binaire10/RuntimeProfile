#include "Profile.hpp"

#include <iostream>
#include <mutex>
#include <vector>

void Profile::Logger::register_sink(std::shared_ptr<sink> sink) { sinks.push_back(std::move(sink)); }

void Profile::Logger::submit(const Profile::ScopeProfile &profile)
{
    for(auto &sink : sinks)
        sink->capture(profile);
}

void Profile::json_coder::begin(std::ostream &out) { out << R"({"otherData":{},"traceEvents":[{})"; }

void Profile::json_coder::write(std::ostream &out, const Profile::ScopeProfile &result)
{

    out << std::setprecision(3) << std::fixed << ",{"
        << R"("cat":"function",)"
        << R"("dur":)" << result.elapsedTime.count() << ',' << R"("name":")" << result.name << "\","
        << R"("ph":"X",)"
        << R"("pid":0,)"
        << R"("tid":)" << result.threadId << ","
        << R"("ts":)" << result.start.count() << "}";
}

void Profile::json_coder::end(std::ostream &out) { out << "]}"; }
