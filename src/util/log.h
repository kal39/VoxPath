/**
 * @file log.h
 *
 * Header file for logging related functionality.
 */

#pragma once

#define FLUSH_AFTER_PRINT 1 ///< Whether to `fflush(stdout)` every log.
#define MAX_FILENAME_LEN 50 ///< Maximum length for filenames in the output.

/**
 * Prints a log (green) message to stdout.
 * @param ... The log message. Uses same format as printf().
 */
#define MESSAGE(...) _message(__FILE__, __LINE__, __VA_ARGS__)

/**
 * Prints a dbg (yellow) message to stdout if PRINT_DBG_LOGS is defined.
 * @param ... The debug message. Uses same format as printf().
 */
#define DEBUG(...) _debug(__FILE__, __LINE__, __VA_ARGS__)

/**
 * Prints a error (red) message to stdout.
 * @param ... The error message. Uses same format as printf().
 */
#define ERROR(...) _error(__FILE__, __LINE__, __VA_ARGS__)

/**
 * Function that gets called by LOG(). Dont call directly.
 * @param file The file where LOG() was called.
 * @param line  The line where LOG() was called.
 * @param format The format string.
 * @param ... The variables required in the format string.
 */
void _message(char *file, int line, char *format, ...);

/**
 * Function that gets called by DBG(). Dont call directly.
 * @param file The file where DBG() was called.
 * @param line  The line where DBG() was called.
 * @param format The format string.
 * @param ... The variables required in the format string.
 */
void _debug(char *file, int line, char *format, ...);

/**
 * Function that gets called by ERROR(). Dont call directly.
 * @param file The file where ERROR() was called.
 * @param line  The line where ERROR() was called.
 * @param format The format string.
 * @param ... The variables required in the format string.
 */
void _error(char *file, int line, char *format, ...);