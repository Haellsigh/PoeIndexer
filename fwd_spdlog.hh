#pragma once

#include <memory>

namespace spdlog {
namespace sinks {
class sink;
}
class logger;
using sink_ptr        = std::shared_ptr<sinks::sink>;
using sinks_init_list = std::initializer_list<sink_ptr>;
} // namespace spdlog
