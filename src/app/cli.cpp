#include "cli.h"
#include <cstring>
#include <cstdlib>
#include <iostream>

static void print_usage(const char* prog) {
    std::cerr << "Usage: " << prog
              << " [--smoke] [-p palette] [-i iterations] [-l landmark]\n"
              << "  --smoke          Non-interactive smoke test (40x20 render)\n"
              << "  -p <name>        Palette: thermal, electric, grayscale, ocean\n"
              << "  -i <n>           Max iterations (default 200)\n"
              << "  -l <name>        Starting landmark (not yet implemented)\n"
              << "  -h               Show this help\n";
}

static PaletteId parse_palette(const char* name) {
    if (std::strcmp(name, "thermal")   == 0) return PaletteId::Thermal;
    if (std::strcmp(name, "electric")  == 0) return PaletteId::Electric;
    if (std::strcmp(name, "grayscale") == 0) return PaletteId::Grayscale;
    if (std::strcmp(name, "ocean")     == 0) return PaletteId::Ocean;
    return PaletteId::Thermal;
}

bool parse_cli(int argc, char** argv, CliArgs& out) {
    for (int i = 1; i < argc; ++i) {
        const char* arg = argv[i];

        if (std::strcmp(arg, "--smoke") == 0) {
            out.smoke_test = true;
        } else if (std::strcmp(arg, "-p") == 0 && i + 1 < argc) {
            out.palette = parse_palette(argv[++i]);
        } else if (std::strcmp(arg, "-i") == 0 && i + 1 < argc) {
            out.max_iter = static_cast<uint32_t>(std::strtoul(argv[++i], nullptr, 10));
            if (out.max_iter == 0) out.max_iter = 200;
        } else if (std::strcmp(arg, "-l") == 0 && i + 1 < argc) {
            out.landmark = argv[++i];
        } else if (std::strcmp(arg, "-h") == 0 || std::strcmp(arg, "--help") == 0) {
            print_usage(argv[0]);
            return false;
        } else {
            std::cerr << "Unknown option: " << arg << "\n";
            print_usage(argv[0]);
            return false;
        }
    }
    return true;
}
