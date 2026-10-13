#include "system/lathe_logger.h"
#include <stdarg.h>

void term_set_background_color(color_t color, FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[48;2;%d;%d;%dm", color.R, color.G, color.B);
}

void term_set_foreground_color(color_t color, FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[38;2;%d;%d;%dm", color.R, color.G, color.B);
}

void term_set_bold(FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[1m");
}

void term_set_no_bold(FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[22m");
}

void term_set_underline(FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[4m");
}

void term_set_no_underline(FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[24m");
}

void term_reset(FILE *out) {
  out = (out == nullptr) ? stderr : out;
  fprintf(out, "\033[0m");
}

void log_lvl_to_str(lathe_log_level lvl, char *log_lvl_buf) {
  switch (lvl) {
  case LOG_LEVEL_FATAL:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[FATAL] \U00002757");
    break;
  case LOG_LEVEL_ERROR:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[ERROR] \U0000274C");
    break;
  case LOG_LEVEL_WARNING:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[WARNING] \U000026A0");
    break;
  case LOG_LEVEL_INFO:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[INFO] \U00002139");
    break;
  case LOG_LEVEL_DEBUG:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[DEBUG] \U0001F41B");
    break;
  case LOG_LEVEL_TRACE:
    snprintf(log_lvl_buf, log_buffer_max, "%s", "[TRACE] \U0001F50D");
    break;
  case LOG_LEVEL_COUNT:
    break;
  }
}

void internal_log_mes(lathe_log_level lvl, FILE *out, const char *file,
                       int line, [[maybe_unused]] const char *time, const char *msg) {
  out = (out == nullptr) ? stderr : out;

  char log_str[log_buffer_max];
  log_lvl_to_str(lvl, log_str);
  
  switch (lvl) {
  case LOG_LEVEL_FATAL:
    term_set_bold(out);
    term_set_underline(out);
    term_set_foreground_color(LASER_RED, out);
    break;
  case LOG_LEVEL_ERROR:
    term_set_foreground_color(LASER_RED, out);
    break;
  case LOG_LEVEL_WARNING:
    term_set_foreground_color(FIRE_BUSH, out);
    break;
  case LOG_LEVEL_INFO:
    term_set_foreground_color(LIGHT_SEA_BLUE, out);
    break;
  case LOG_LEVEL_DEBUG:
    term_set_foreground_color(SOME_GREEN, out);
    break;
  case LOG_LEVEL_TRACE:
    term_set_foreground_color(YELLOW_GLITTER, out);
    break;
      case LOG_LEVEL_COUNT:
    break;
  }
  fprintf(out, "%s %s:%d %s \n", log_str, file, line, msg);
  term_reset(out);
}

void internal_log_fmt_mes(FILE *out,lathe_log_level lvl, const char *file,
                        int line, [[maybe_unused]] const char *time, const char *fmt, ...){
    out = (out == nullptr) ? stderr : out;
    
    char log_str[log_buffer_max];
    log_lvl_to_str(lvl, log_str);
    
    switch (lvl) {
        case LOG_LEVEL_FATAL:
        term_set_bold(out);
        term_set_underline(out);
        term_set_foreground_color(LASER_RED, out);
        break;
        case LOG_LEVEL_ERROR:
        term_set_foreground_color(LASER_RED, out);
        break;
        case LOG_LEVEL_WARNING:
        term_set_foreground_color(FIRE_BUSH, out);
        break;
        case LOG_LEVEL_INFO:
        term_set_foreground_color(LIGHT_SEA_BLUE, out);
        break;
        case LOG_LEVEL_DEBUG:
        term_set_foreground_color(SOME_GREEN, out);
        break;
        case LOG_LEVEL_TRACE:
        term_set_foreground_color(YELLOW_GLITTER, out);
        break;
        case LOG_LEVEL_COUNT:
        break;
    }
    
    fprintf(out, "%s", log_str);
    
    fprintf(out, " %s:%d ", file, line);
    
    va_list args;
    va_start(args, fmt);
    vfprintf(out, fmt, args);
    va_end(args);
    
    
    term_reset(out);
    //fprintf(out, "\n");
    fflush(out);
    }
