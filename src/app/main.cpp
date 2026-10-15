#include "cli.h"
#include "app.h"
#include <iostream>

int main(int argc, char** argv) {
    CliArgs args;
    if (!parse_cli(argc, argv, args)) {
        return 1;
    }
    return run_app(args);
}
