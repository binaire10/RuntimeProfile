# Runtime Profiler

Library to profile runtime readable with chrome tracing([edge](edge://tracing), [chrome](chrome://tracing)). Profile::
Register_sink and Profile::submit is not thread safe.
By default, no sink is set;

## Example of profiled function

```cpp
#include <Profile/SteadyClockProfiler.hpp>

#define PROFILE_FUNCTION() STEADY_CLOCK_PROFILE_FUNCTION()

void foo() {
    PROFILE_FUNCTION();
}
```

## Minimal code to record result

```cpp
#include <Profile/SteadyClockProfiler.hpp>

int main() {
    // register an output sink to save the captured
    Profile::register_sink(
            std::make_shared<Profile::stream_json_sink<std::mutex>>(std::cout)
            );
}
```
