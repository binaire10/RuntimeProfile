#include "Profile.hpp"

#include <iostream>
#include <mutex>
#include <vector>

namespace
{
    std::vector<std::shared_ptr<Profile::sink>> consumers;
}// namespace

void Profile::submit(const ScopeProfile &profile)
{
    for(auto &consumer : consumers) consumer->capture(profile);
}

void Profile::register_sink(std::shared_ptr<sink> sink) { consumers.emplace_back(std::move(sink)); }
