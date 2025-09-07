#include <charconv>
#include <fstream>
#include <iomanip>
#include <limits>
#include <string>
#include <string_view>
#include <unordered_map>
#include <cstdlib>   // strtod

struct CityResult {
    double min  =  std::numeric_limits<double>::infinity();
    double max  = -std::numeric_limits<double>::infinity();
    int    counter = 0;
    double sum  = 0.0;
};

int main() {
    std::ifstream infile("test_sample.txt");
    if (!infile) return 1;

    static char inbuf[1 << 20];
    infile.rdbuf()->pubsetbuf(inbuf, sizeof inbuf);

    std::unordered_map<std::string, CityResult> results;
    results.reserve(1 << 15);

    std::string line;
    line.reserve(256);

    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        const size_t sep = line.find(';');
        if (sep == std::string::npos) continue;

        std::string_view city_sv(line.data(), sep);

        const size_t val_off = sep + 1;
        if (val_off >= line.size()) continue;

        // Make the buffer NUL-terminated for strtod without copying
        const size_t old_size = line.size();
        line.reserve(old_size + 1);   // avoid reallocation after taking pointers
        line.push_back('\0');         // sentinel

        const char* val_begin = line.data() + val_off;
        char* endp = nullptr;
        double value = std::strtod(val_begin, &endp);
        line.pop_back();              // restore original string

        if (endp == val_begin) continue; // parse error

        auto [it, inserted] = results.try_emplace(std::string(city_sv));
        CityResult& cr = it->second;
        if (inserted) {
            cr.min = value; cr.max = value; cr.sum = value; cr.counter = 1;
        } else {
            if (value < cr.min) cr.min = value;
            if (value > cr.max) cr.max = value;
            cr.sum += value; cr.counter += 1;
        }
    }

    std::ofstream outfile("test_sample_results_calculated.txt");
    if (!outfile) return 1;
    static char outbuf[1 << 20];
    outfile.rdbuf()->pubsetbuf(outbuf, sizeof outbuf);

    outfile << std::fixed << std::setprecision(8);
    for (auto& [city, cr] : results) {
        const double mean = cr.counter ? cr.sum / cr.counter : 0.0;
        outfile << city << ';' << mean << ';' << cr.min << ';' << cr.max << '\n';
    }
    return 0;
}
