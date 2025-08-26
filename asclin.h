#ifndef ASCLIN__H_
#define ASCLIN__H_

#include <string.h>
#include <stdio.h>   // vsnprintf
#include <stdarg.h>  // va_list, va_start, va_end

#define print(...) tx_uart_pc_debug(__VA_ARGS__)
#define ENDLINE "\n\r"


void init_ASCLIN_UART(void);
void tx_uart_pc_debug(const char *format, ...);

#endif
