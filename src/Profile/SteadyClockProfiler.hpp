#ifndef RUNTIMEPROFILE_STEADYCLOCKPROFILER_HPP
#define RUNTIMEPROFILE_STEADYCLOCKPROFILER_HPP

#include "Profile.hpp"

namespace Profile {
    using steady_clock_record = Profile::RecordProfile<std::chrono::steady_clock>;
}

#define STEADY_CLOCK_PROFILE_SCOPE_LINE_1(VAR, NAME, LINE) Profile::steady_clock_record VAR##LINE##__{NAME}
#define STEADY_CLOCK_PROFILE_SCOPE_LINE(VAR, NAME, LINE) STEADY_CLOCK_PROFILE_SCOPE_LINE_1(VAR, NAME, LINE)
#define STEADY_CLOCK_PROFILE_SCOPE(NAME) STEADY_CLOCK_PROFILE_SCOPE_LINE(profileTimer, NAME, __LINE__)
#define STEADY_CLOCK_PROFILE_FUNCTION() STEADY_CLOCK_PROFILE_SCOPE(__FUNCTION__)

#endif// RUNTIMEPROFILE_STEADYCLOCKPROFILER_HPP
