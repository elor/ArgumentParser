#pragma once

/**
 * these functions convert a cstring to the respective output type. Not
 * necessarily threadsafe due to use of errno.
 *
 * @param str input string
 * @param out valid pointer to write the result to
 * @returns
 */
int convert(const char *str, bool *out);
int convert(const char *str, double *out);
int convert(const char *str, int *out);
int convert(const char *str, unsigned int *out);
int convert(const char *str, unsigned long long *out);
