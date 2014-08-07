/*
 * ArgumentParser.h
 *
 *  Created on: Sep 11, 2013
 *      Author: elor
 */

#ifndef ARGUMENTPARSER_H_
#define ARGUMENTPARSER_H_

#include <cstddef>

class ArgumentParserInternals;

class ArgumentParser
{
public:
  typedef void (*Callback)(void*);

private:
  ArgumentParserInternals *args;

public:
  ArgumentParser(const char *progname = "");
  virtual ~ArgumentParser();

  /*
   * longKey: long key as used in CLI (--longKey) and in files (longKey = ...)
   * shortKey: short key as used in CLI (-k). One char only, '\0': leave blank
   * target: if defined, write value directly to (*target). This eliminates
   * the need to call the get functions
   *
   * You may call the functions multiple times to set additional targets and
   * shortKeys and to change the comment. For adding other shortKeys, you can
   * use registerShortKey() instead.
   */
  void Bool(const char *longKey, const char *comment = NULL,
    unsigned char shortKey = '\0', bool *target = NULL);
  void Bool(const char *longKey, bool defaultValue, const char *comment = NULL,
    unsigned char shortKey = '\0', bool *target = NULL);
  void Int(const char *longKey, const char *comment = NULL,
    unsigned char shortKey = '\0', int *target = NULL);
  void Int(const char *longKey, int defaultValue, const char *comment = NULL,
    unsigned char shortKey = '\0', int *target = NULL);
  void UInt(const char *longKey, const char *comment = NULL,
    unsigned char shortKey = '\0', unsigned int *target = NULL);
  void UInt(const char *longKey, unsigned int defaultValue,
    const char *comment = NULL, unsigned char shortKey = '\0',
    unsigned int *target = NULL);
  void Double(const char *longKey, const char *comment = NULL,
    unsigned char shortKey = '\0', double *target = NULL);
  void
  Double(const char *longKey, double defaultValue, const char *comment = NULL,
    unsigned char shortKey = '\0', double *target = NULL);
  void String(const char *longKey, const char *comment = NULL, char shortKey =
    '\0', char *target = NULL);
  void String(const char *longKey, const char *defaultValue,
    const char *comment = NULL, char shortKey = '\0', char *target = NULL);

  void registerCallback(const char *longKey, Callback callback, void* data =
  NULL);

  void Standalones(int maximum = -1, const char *helpKey = "argument",
    const char *comment = NULL);

  // if one of the keys are encountered, a file is included (read in place)
  void File(const char *longKey, const char *comment = NULL,
    unsigned char shortKey = '\0');

  void registerShortKey(unsigned char shortKey, const char *longKey);
  void registerComment(const char *longKey, const char *comment);
  void registerTarget(const char *longKey, void *target);

  bool keyExists(const char *longKey);
  bool wasValueSet(const char *longKey, bool includeDefault = false);
  bool shortKeyExists(unsigned char shortKey);
  void getLongKey(unsigned char shortKey, char *output);
  bool allValuesSet(const char *errorFormat = NULL);

  bool getBool(const char *longKey);
  int getInt(const char *longKey);
  unsigned int getUInt(const char *longKey);
  double getDouble(const char *longKey);
  void getString(const char *longKey, char *output);
  const char *getCString(const char *longKey);

  int getStandaloneCount();
  void getStandalone(unsigned int index, char *output);
  const char *getCStandalone(unsigned int index);

  void set(const char *longKey, bool value);
  void set(const char *longKey, int value);
  void set(const char *longKey, unsigned int value);
  void set(const char *longKey, double value);
  void set(const char *longKey, const char *value);

  void setProgName(const char *progname);

  void parseFile(const char *filename);
  void parseLine(const char *line);
  void parseArgs(int argc, char **argv);

  bool writeFile(const char *filename); // false on success, true on failure

  void displayHelpMessage();
};

#endif /* ARGUMENTPARSER_H_ */
