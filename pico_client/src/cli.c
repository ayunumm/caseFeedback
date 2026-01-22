#include "cli.h"

#include "pico/stdlib.h"
#include "pico/error.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

static char line_buf[CLI_MAX_LINE_LEN];
static size_t line_len = 0;

static const cli_command_t *commands[CLI_MAX_COMMANDS];
static size_t command_count = 0;

static void (*write_fn)(const char*) = NULL;

void cli_init(void (*write_cb)(const char*)) {
  write_fn = write_cb;
  cli_print("\r\n> ");
}

void cli_register(const cli_command_t *cmd) {
  if (command_count < CLI_MAX_COMMANDS) {
    commands[command_count++] = cmd;
  }
}

void cli_print(const char *fmt, ...) {
  char buf[128];
  va_list ap;
  va_start(ap, fmt);
  vsnprintf(buf, sizeof(buf), fmt, ap);
  va_end(ap);
  write_fn(buf);
}

static void cli_execute(char *line) {
  char *argv[CLI_MAX_ARGS];
  int argc = 0;

  char *token = strtok(line, " ");
  while (token && argc < CLI_MAX_ARGS) {
    argv[argc++] = token;
    token = strtok(NULL, " ");
  }

  if (argc == 0) return;

  for (size_t i = 0; i < command_count; i++) {
    if (strcmp(argv[0], commands[i]->name) == 0) {
      commands[i]->fn(argc, argv);
      return;
    }
  }

  cli_print("Unknown command\r\n");
}

void cli_process_char(char c) {
  if (c == '\r' || c == '\n') {
    cli_print("\r\n");
    line_buf[line_len] = 0;
    cli_execute(line_buf);
    line_len = 0;
    cli_print("> ");
    return;
  }

  if (c == 0x7F || c == 0x08) {
    if (line_len > 0) {
      line_len--;
      cli_print("\b \b");
    }
    return;
  }

  if (line_len < CLI_MAX_LINE_LEN - 1) {
    line_buf[line_len++] = c;
    char echo[2] = {c, 0};
    write_fn(echo);
  }
}

void cli_poll(void) {
  int c = getchar_timeout_us(0);
  if (c != PICO_ERROR_TIMEOUT) {
    cli_process_char((char)c);
  }
}
