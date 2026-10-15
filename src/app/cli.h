#ifndef APP_CLI_H
#define APP_CLI_H

#include <string>
#include <cstdint>
#include "palette.h"

struct CliArgs {
    PaletteId palette       = PaletteId::Thermal;
    uint32_t  max_iter      = 200;
    std::string landmark;    // empty = default view
    bool      smoke_test    = false;
    uint32_t  smoke_rows    = 20;
    uint32_t  smoke_cols    = 80;
};

/// Parse command-line arguments.  Returns false on error / --help.
bool parse_cli(int argc, char** argv, CliArgs& out);

#endif // APP_CLI_H
