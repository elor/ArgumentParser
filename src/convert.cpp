#include <convert.hpp>
#include <cstdlib>
#include <cctype>
#include <cerrno>
#include <cstring>

int convert(const char *str, bool *out)
{
  char *lowerStr = strdup(str);
  char *p = lowerStr;
  while (*p != '\0')
  {
    *p = tolower(*p);
    ++p;
  }

  char *begin = lowerStr;
  while (!isgraph(*begin))
  {
    ++begin;
  }

  if (*begin == '\0')
  {
    return 1;
  }

  char *end = begin;
  while (isgraph(*end))
  {
    ++end;
  }
  *end = '\0';

  if (strcmp(begin, "false") == 0)
  {
    *out = false;
  }
  else if (strcmp(begin, "true") == 0)
  {
    *out = true;
  }
  else
  {
    return 1;
  }

  delete[] lowerStr;

  return 0;
}

int convert(const char *str, double *out)
{
  errno = 0;
  char *p;
  double tmp = strtod(str, &p);

  if (errno != 0 || str == p)
  {
    return 1;
  }

  *out = tmp;
  return 0;
}

int convert(const char *str, int *out)
{
  errno = 0;
  char *p;
  int tmp = strtol(str, &p, 10);

  if (errno != 0 || str == p)
  {
    return 1;
  }

  *out = tmp;
  return 0;
}

int convert(const char *str, unsigned int *out)
{
  errno = 0;
  char *p;
  unsigned int tmp = strtoul(str, &p, 10);

  if (errno != 0 || str == p)
  {
    return 1;
  }

  *out = tmp;
  return 0;
}

int convert(const char *str, unsigned long long *out)
{
  errno = 0;
  char *p;
  unsigned long long tmp = strtoull(str, &p, 10);

  if (errno != 0 || str == p)
  {
    return 1;
  }

  *out = tmp;
  return 0;
}
