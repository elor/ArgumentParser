/*
 * ArgumentParser.h
 *
 *  Created on: Aug 23, 2013
 *      Author: elor
 */

#ifndef ARGUMENTPARSERINTERNALS_H_
#define ARGUMENTPARSERINTERNALS_H_

#include <map>
#include <Argument.hpp>
#include <cstring>
#include <vector>

class ArgumentParserInternals
{
private:
  struct cmp_str
  {
    bool operator()(const char *a, const char* b)
    {
      return std::strcmp(a, b) < 0;
    }
  };

  typedef std::map<const char *, Argument, cmp_str> ArgumentMap;
  typedef std::multimap<const char *, void*, cmp_str> TargetMap;
  typedef std::pair<TargetMap::iterator, TargetMap::iterator> TargetRange;
  typedef std::map<const char *, const char *, cmp_str> CommentMap;
  typedef std::vector<const char *> StandaloneVector;

  char **shortKeys;
  ArgumentMap arguments;
  ArgumentMap defaults;
  TargetMap targets;
  CommentMap comments;
  StandaloneVector standalones;
  int maxStandalones;
  char *standaloneComment;
  char *standaloneHelpKey;
  char *progname;

  void clearShortKeys();
  void clearShortKey(unsigned char shortKey);
  void clearArguments();
  void clearTargets();
  void clearDefaults();
  void clearComments();
  void clearStandalones();
  void clearAll();

  void lookForHelp();

  Argument
  *registerArgument(const char *longKey, Argument::ValueType valueType);
  Argument *fetchArgument(const char *longKey, bool useDefault = false);
  const char *fetchComment(const char *longKey);
  Argument *registerDefault(const char *longKey, Argument::ValueType valueType);
  Argument *fetchDefault(const char *longKey);
  void addStandalone(const char *standalone);

  void setTarget(Argument *argument, void *target);
  void setTargets(const char *longKey);
  void setAllTargets();

  const char *getLongKey(unsigned char shortKey);

public:
  ArgumentParserInternals(const char *_progname = "");
  virtual ~ArgumentParserInternals();

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
  void Bool(const char *longKey, const char *comment, unsigned char shortKey,
      bool *target);
  void Bool(const char *longKey, bool defaultValue, const char *comment,
      unsigned char shortKey, bool *target);
  void
  Int(const char *longKey, const char *comment, unsigned char shortKey,
      int *target);
  void Int(const char *longKey, int defaultValue, const char *comment,
      unsigned char shortKey, int *target);
  void UInt(const char *longKey, const char *comment, unsigned char shortKey,
      unsigned int *target);
  void UInt(const char *longKey, unsigned int defaultValue,
      const char *comment, unsigned char shortKey, unsigned int *target);
  void Double(const char *longKey, const char *comment, unsigned char shortKey,
      double *target);
  void
  Double(const char *longKey, double defaultValue, const char *comment,
      unsigned char shortKey, double *target);
  void String(const char *longKey, const char *comment, unsigned char shortKey,
      char *target);
  void String(const char *longKey, const char *defaultValue,
      const char *comment, unsigned char shortKey, char *target);

  void Standalones(int maximum = -1, const char *helpKey = "argument",
      const char *comment = NULL);

  // if one of the keys are encountered, a file is included (read in place)
  void File(const char *longKey, const char *comment, unsigned char shortKey);

  void registerShortKey(unsigned char shortKey, const char *longKey);
  void registerComment(const char *longKey, const char *comment);
  void registerTarget(const char *longKey, void *target);

  bool keyExists(const char *longKey);
  bool wasValueSet(const char *longKey, bool includeDefault);
  bool shortKeyExists(unsigned char shortKey);
  void getLongKey(unsigned char shortKey, char *output);
  bool allValuesSet(const char *errorFormat);

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

  void setProgName(const char *_progname);

  void parseFile(const char *filename);
  void parseLine(const char *line);
  void parseArgs(int argc, char **argv);

  void displayHelpMessage();
};

#endif /* ARGUMENTPARSERINTERNALS_H_ */
