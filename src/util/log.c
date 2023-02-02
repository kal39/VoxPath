/**
 * @file log.c
 *
 * Source file for logging related functionality.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"

/**
 * Macro used to parse arguments given to the log fuctions.
 *
 * @param dest Buffer to write the formatted string to. Must call free() on it.
 * @param format The format string.
 */

#define _parse_message(dest, format)                                           \
	{                                                                          \
		va_list args;                                                          \
		va_start(args, format);                                                \
		int len = vsnprintf(NULL, 0, format, args) + 1;                        \
		va_end(args);                                                          \
		dest = malloc(len);                                                    \
		va_start(args, format);                                                \
		vsnprintf(dest, len, format, args);                                    \
		va_end(args);                                                          \
	}

/**
 * Takes in a filename and line number and returns a string with the format
 * file:line. The string is guaranteed ot be shorter than MAX_FILENAME_LENGTH.
 *
 * @param file The file name.
 * @param line The line number.
 * @returns A formatted string. Must call free() on it.
 */
static char *_get_formatted_filename(char *file, int line) {
	int str_len = snprintf(NULL, 0, "%s:%d", file, line) + 1;
	char *fileName = malloc(str_len);
	snprintf(fileName, str_len, "%s:%d", file, line);

	if (str_len - 1 < MAX_FILENAME_LEN) {
		fileName = realloc(fileName, MAX_FILENAME_LEN + 1);
		for (int i = str_len - 1; i < MAX_FILENAME_LEN; i++) fileName[i] = ' ';
		fileName[MAX_FILENAME_LEN] = '\0';

	} else if (str_len - 1 > MAX_FILENAME_LEN) {
		fileName = realloc(fileName, MAX_FILENAME_LEN + 1);
		for (int i = 0; i < 3; i++) fileName[MAX_FILENAME_LEN - 3 + i] = '.';
		fileName[MAX_FILENAME_LEN] = '\0';
	}

	return fileName;
}

/**
 * Takes in a message string and makes sure that when there is a '\n' character
 * in it, the start of the new line will align with the rest of the text.
 *
 * @param message The unformatted string.
 * @returns A formatted string. Must call free() on it.
 */
static char *_get_formatted_message(char *message) {
	int padding = MAX_FILENAME_LEN + 11;

	char *formatted_message = malloc(strlen(message) + 1);
	memcpy(formatted_message, message, strlen(message) + 1);

	for (int i = 0; formatted_message[i] != '\0'; i++) {
		if (formatted_message[i] == '\n') {
			int old_len = strlen(formatted_message);
			int new_len = old_len + padding;

			formatted_message = realloc(formatted_message, new_len + 1);
			memcpy(
				&formatted_message[i + 1 + padding],
				&formatted_message[i + 1],
				old_len - i
			);

			for (int j = 0; j < padding; j++)
				formatted_message[i + 1 + j] = ' ';
			formatted_message[new_len] = '\0';
		}
	}

	return formatted_message;
}

/**
 * Prints the message type, file name, line, and message.
 *
 * @param type The message type.
 * @param color The color to use when printing the message type.
 * @param file The file name.
 * @param line The line number.
 * @param message The message.
 */
static void
_print_message(char *type, int color, char *file, int line, char *message) {
	char *formatted_filename = _get_formatted_filename(file, line);
	char *formatted_message = _get_formatted_message(message);

	printf(
		" \e[1m\e[%dm%s\e[0m ┃ \e[1m%s\e[0m ┃ %s\n",
		color,
		type,
		formatted_filename,
		formatted_message
	);

	if (FLUSH_AFTER_PRINT) fflush(stdout);

	free(formatted_filename);
	free(formatted_message);
}

void _message(char *file, int line, char *format, ...) {
	char *message;
	_parse_message(message, format);
	_print_message("LOG", 32, file, line, message);
	free(message);
}

void _debug(char *file, int line, char *format, ...) {
	(void)file;
	(void)line;
	(void)format;

#ifdef PRINT_DBG_LOGS
	char *message;
	_parse_message(message, format);
	_print_message("DBG", 33, file, line, message);
	free(message);
#endif
}

void _error(char *file, int line, char *format, ...) {
	char *message;
	_parse_message(message, format);
	_print_message("ERR", 31, file, line, message);
	free(message);
}