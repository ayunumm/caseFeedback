#ifndef _CLI_H_
#define ifndef _CLI_H_

#include <stddef.h>

#define CLI_MAX_LINE_LEN 64
#define CLI_MAX_ARGS 8
#define CLI_MAX_COMMANDS 16

typedef void (*cli_cmd_fn)(int argc, char** argv);

typedef struct {
  const char *name;
  const char *help;
  cli_cmd_fn fn;
} cli_command_t;

void cli_init(void (*write_cb)(const char*));
void cli_register(const cli_command_t *cmd);
void cli_process_char(char c);
void cli_poll(void);
void cli_print(const char *fmt, ...);

#endif
