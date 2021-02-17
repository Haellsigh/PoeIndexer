#pragma once

#include <algorithm>
#include <chrono>
#include <vector>

#include <spdlog/spdlog.h>

class RateLimiter {
    using clock   = std::chrono::high_resolution_clock;
    using seconds = std::chrono::duration<double>;

  public:
    RateLimiter(std::size_t request_limit = 1, double duration_s = 0)
        : request_limit_(request_limit), duration_(duration_s) {}

    void set_limits(std::size_t request_limit = 1, double duration_s = 0) {
        request_limit_ = request_limit;
        duration_      = seconds(duration_s);

        spdlog::info("Updating limits: {} requests/{}s", request_limit_, duration_s);
    }

    template <typename Functor, typename... Args> bool execute(Functor f, Args... args) {
        // Count the number of requests in the past interval_ seconds and compare with request_limit_
        const auto now = clock::now();
        std::erase_if(request_times_, [&](const clock::time_point &t) { return (now - t) > duration_; });

        if (request_times_.size() >= request_limit_) {
            // We're rate limited
            /*const auto time_until_next_request =
                duration_ - ((now - request_times_.back())) + std::chrono::milliseconds(500);*/
            spdlog::info("Sleeping for {} ms", std::chrono::duration_cast<seconds>(duration_).count() * 1000);

            std::this_thread::sleep_for(duration_);

            request_times_.push_back(clock::now());
            f(args...);

            return false;
        } else {
            request_times_.push_back(now);
            f(args...);
        }

        return true;
    }

  private:
    std::size_t request_limit_;
    seconds duration_;

    std::vector<clock::time_point> request_times_;
};
