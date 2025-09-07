#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <unordered_map>
#include <limits>
#include <iomanip> // for std::fixed, std::setprecision

struct CityResult {
    double mean = 0.0;
    double min  = std::numeric_limits<double>::infinity();
    double max  = -std::numeric_limits<double>::infinity();
    int counter = 0;
    double sum  = 0.0;
};

int main() {
    std::unordered_map<std::string, CityResult> results;

    std::ifstream infile("test_sample.txt");
    if (!infile) {
        std::cerr << "Could not open input file\n";
        return 1;
    }

    std::string line;
    while (std::getline(infile, line)) {
        // if (line.empty()) continue;

        // Split on ';' (only once, city;value)
        auto sep_pos = line.find(';');
        // if (sep_pos == std::string::npos) continue;

        std::string city = line.substr(0, sep_pos);
        double value = std::stod(line.substr(sep_pos + 1));

        auto &cr = results[city]; // inserts default CityResult if not exists
        if (value < cr.min) cr.min = value;
        if (value > cr.max) cr.max = value;
        cr.sum += value;
        cr.counter += 1;
    }

    infile.close();

    std::ofstream outfile("test_sample_results_calculated.txt");
    if (!outfile) {
        std::cerr << "Could not open output file\n";
        return 1;
    }

    for (auto &[city, cr] : results) {
        cr.mean = cr.sum / cr.counter;
        outfile << city << ";"
            << std::fixed << std::setprecision(12)
            << cr.mean << ";" << cr.min << ";" << cr.max << "\n";
    }

    outfile.close();
    return 0;
}

