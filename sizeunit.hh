#pragma once

#include <cstdint>
#include <string>

#include <fmt/format.h>

struct SizeUnit {
    enum Unit : uint64_t
    {
        Byte     = static_cast<uint64_t>(1e0),
        KiloByte = static_cast<uint64_t>(1e3),
        MegaByte = static_cast<uint64_t>(1e6),
        GigaByte = static_cast<uint64_t>(1e9),
        TeraByte = static_cast<uint64_t>(1e12)
    };

    SizeUnit(uint64_t bytes) {
        if (bytes >= TeraByte) {
            unit = TeraByte;
            size = bytes / static_cast<double>(TeraByte);
        } else if (bytes >= GigaByte) {
            unit = GigaByte;
            size = bytes / static_cast<double>(GigaByte);
        } else if (bytes >= MegaByte) {
            unit = MegaByte;
            size = bytes / static_cast<double>(MegaByte);
        } else if (bytes >= KiloByte) {
            unit = KiloByte;
            size = bytes / static_cast<double>(KiloByte);
        }
    }

    double size;
    Unit unit = Byte;
};

template <> struct fmt::formatter<SizeUnit> {
    constexpr auto parse(format_parse_context &ctx) { return ctx.begin(); }

    template <typename FormatContext> auto format(const SizeUnit &s, FormatContext &ctx) {
        std::string unit = "Unknown unit";
        switch (s.unit) {
        case SizeUnit::Byte:
            unit = "B";
            break;
        case SizeUnit::KiloByte:
            unit = "kB";
            break;
        case SizeUnit::MegaByte:
            unit = "MB";
            break;
        case SizeUnit::GigaByte:
            unit = "GB";
            break;
        case SizeUnit::TeraByte:
            unit = "TB";
            break;
        }

        return format_to(ctx.out(), "{} {}", s.size, unit);
    }
};
