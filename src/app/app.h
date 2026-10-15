#ifndef APP_APP_H
#define APP_APP_H

#include "cli.h"

/// Run the application with the given CLI arguments.
/// For smoke-test mode this renders a fixed frame to stdout and returns.
/// For interactive mode this enters the terminal loop.
int run_app(const CliArgs& args);

#endif // APP_APP_H
