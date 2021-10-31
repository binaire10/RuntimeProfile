#include <iostream>
#include <chrono>
#include <Profile/SteadyClockProfiler.hpp>

#define PROFILE_FUNCTION() STEADY_CLOCK_PROFILE_FUNCTION()

using namespace std::chrono_literals;

void foo() {
    PROFILE_FUNCTION();

    std::this_thread::sleep_for(1s);
}

int main() {
    Profile::register_sink(std::make_shared<Profile::stream_json_sink<std::mutex>>(std::cout));
    PROFILE_FUNCTION();

    std::this_thread::sleep_for(1s);
    foo();
}
