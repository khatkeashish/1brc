// solution.cpp
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <string>
#include <string_view>
#include <unordered_map>
#include <limits>
#include <cstdint>

// ---------- City stats ----------
struct CityResult {
    double min  =  std::numeric_limits<double>::infinity();
    double max  = -std::numeric_limits<double>::infinity();
    int    counter = 0;
    double sum  = 0.0;
};

// ---------- Transparent hash/equal for heterogenous lookup ----------
struct SvHash {
    using is_transparent = void;  // enables heterogenous lookup
    std::size_t operator()(std::string_view s) const noexcept {
        // FNV-1a 64-bit
        std::uint64_t h = 1469598103934665603ull;
        for (unsigned char c : s) {
            h ^= c;
            h *= 1099511628211ull;
        }
        return static_cast<std::size_t>(h);
    }
};
struct SvEq {
    using is_transparent = void;
    bool operator()(std::string_view a, std::string_view b) const noexcept {
        return a == b;
    }
};

int main() {
    // ---------- Input ----------
    FILE* fp = std::fopen("test_sample.txt", "r");
    if (!fp) { std::perror("fopen"); return 1; }

    // Big input buffer (1 MiB)
    static char inbuf[1 << 20];
    std::setvbuf(fp, inbuf, _IOFBF, sizeof inbuf);

    std::unordered_map<std::string, CityResult, SvHash, SvEq> results;
    results.reserve(1 << 15); // heuristic: adjust if you know #cities

    char*  line = nullptr;  // getline buffer (malloc'ed by libc)
    size_t cap  = 0;
    ssize_t len;

    while ((len = getline(&line, &cap, fp)) != -1) {
        if (len <= 1) continue;

        // Trim trailing newline(s)
        while (len && (line[len-1] == '\n' || line[len-1] == '\r')) line[--len] = '\0';
        if (len == 0) continue;

        // Find first ';'
        char* sep = static_cast<char*>(std::memchr(line, ';', static_cast<size_t>(len)));
        if (!sep) continue;

        std::string_view city_sv{line, static_cast<size_t>(sep - line)};
        const char* val_begin = sep + 1;
        if (val_begin >= line + len) continue;

        // Parse value
        char* endp = nullptr;
        double v = std::strtod(val_begin, &endp);
        if (endp == val_begin) continue; // parse failure

        // Probe without allocation
        auto it = results.find(city_sv);
        if (it == results.end()) {
            // First occurrence: allocate key once
            std::string key(city_sv);
            CityResult cr;
            cr.min = cr.max = v;
            cr.sum = v;
            cr.counter = 1;
            results.emplace(std::move(key), cr);
        } else {
            CityResult& cr = it->second;
            if (v < cr.min) cr.min = v;
            if (v > cr.max) cr.max = v;
            cr.sum += v;
            cr.counter += 1;
        }
    }
    std::free(line);
    std::fclose(fp);

    // ---------- Output ----------
    FILE* out = std::fopen("test_sample_results_calculated.txt", "w");
    if (!out) { std::perror("fopen"); return 1; }

    static char outbuf[1 << 20];
    std::setvbuf(out, outbuf, _IOFBF, sizeof outbuf);

    char numbuf[64];

    for (const auto& kv : results) {
        const std::string& city = kv.first;
        const CityResult& cr = kv.second;
        const double mean = cr.counter ? cr.sum / static_cast<double>(cr.counter) : 0.0;

        // city;
        std::fwrite(city.data(), 1, city.size(), out);
        std::fputc(';', out);

        // mean;min;max with fixed 8 decimals
        int n = std::snprintf(numbuf, sizeof numbuf, "%.8f", mean);
        std::fwrite(numbuf, 1, static_cast<size_t>(n), out);
        std::fputc(';', out);

        n = std::snprintf(numbuf, sizeof numbuf, "%.8f", cr.min);
        std::fwrite(numbuf, 1, static_cast<size_t>(n), out);
        std::fputc(';', out);

        n = std::snprintf(numbuf, sizeof numbuf, "%.8f", cr.max);
        std::fwrite(numbuf, 1, static_cast<size_t>(n), out);
        std::fputc('\n', out);
    }

    std::fclose(out);
    return 0;
}
