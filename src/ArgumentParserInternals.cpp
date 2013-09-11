/*
 * ArgumentParser.cpp
 *
 *  Created on: Aug 23, 2013
 *      Author: elor
 */

#include <ArgumentParserInternals.hpp>
#include <debug.hpp>
#include <cctype>
#include <fstream>
#include <cstdio>
#include <cstdlib>

using namespace std;
ArgumentParserInternals::ArgumentParserInternals(const char *_progname) :
  shortKeys(new char*[256]), maxStandalones(0)
{
  progname = strdup(_progname);
  for (int i = 0; i < 256; ++i)
  {
    shortKeys[i] = NULL;
  }
}

ArgumentParserInternals::~ArgumentParserInternals()
{
  clearAll();
  delete[] shortKeys;
  free(progname);
}

void ArgumentParserInternals::clearShortKeys()
{
  for (int shortKey = 0; shortKey < 256; ++shortKey)
  {
    if (shortKeys[shortKey] != NULL)
    {
      delete[] shortKeys[shortKey];
      shortKeys[shortKey] = NULL;
    }
  }
}

void ArgumentParserInternals::clearShortKey(unsigned char shortKey)
{
  if (shortKeys[shortKey] != NULL)
  {
    free(shortKeys[shortKey]);
    shortKeys[shortKey] = NULL;
  }
}

void ArgumentParserInternals::clearArguments()
{
  while (!arguments.empty())
  {
    ArgumentMap::iterator it = arguments.begin();
    free(const_cast<char*> (it->first));
    arguments.erase(it);
  }
}

void ArgumentParserInternals::clearTargets()
{
  while (!targets.empty())
  {
    TargetMap::iterator it = targets.begin();
    free(const_cast<char*> (it->first));
    targets.erase(it);
  }
}

void ArgumentParserInternals::clearDefaults()
{
  while (!defaults.empty())
  {
    ArgumentMap::iterator it = defaults.begin();
    free(const_cast<char*> (it->first));
    defaults.erase(it);
  }
}

void ArgumentParserInternals::clearComments()
{
  while (!comments.empty())
  {
    CommentMap::iterator it = comments.begin();
    free(const_cast<char*> (it->first));
    free(const_cast<char*> (it->second));
    comments.erase(it);
  }
}

void ArgumentParserInternals::clearStandalones()
{
  for (StandaloneVector::iterator it = standalones.begin(); it
      != standalones.end(); ++it)
  {
    free(const_cast<char*> (*it));
  }

  standalones.clear();
}

void ArgumentParserInternals::clearAll()
{
  clearShortKeys();
  clearArguments();
  clearTargets();
  clearDefaults();
  clearComments();
  clearStandalones();
}

bool validateKey(const char *longKey)
{
  const char *keyEnd = longKey;
  while (isalnum(*keyEnd))
  {
    ++keyEnd;
  }

  return (keyEnd != longKey && *keyEnd == '\0');
}

Argument *ArgumentParserInternals::registerArgument(const char *longKey,
    Argument::ValueType valueType)
{
  if (!validateKey(longKey))
  {
    //    err << "ERROR: invalid key: '" << longKey << "'" << endl;
    throw runtime_error("invalid args key");
  }

  ArgumentMap::iterator it = arguments.find(longKey);
  if (it == arguments.end())
  {
    pair<ArgumentMap::iterator, bool> ret;
    ret = arguments.insert(
        ArgumentMap::value_type(strdup(longKey), Argument(valueType)));

    assert(ret.second == true);
    it = ret.first;
  }

  return &it->second;
}

Argument *ArgumentParserInternals::registerDefault(const char *longKey,
    Argument::ValueType valueType)
{
  ArgumentMap::iterator it = defaults.find(longKey);
  if (it == defaults.end())
  {
    pair<ArgumentMap::iterator, bool> ret;
    ret = defaults.insert(
        ArgumentMap::value_type(strdup(longKey), Argument(valueType)));

    assert(ret.second == true);
    it = ret.first;
  }

  return &it->second;
}

Argument *ArgumentParserInternals::fetchDefault(const char *longKey)
{
  ArgumentMap::iterator it = defaults.find(longKey);

  if (it == defaults.end())
  {
    return NULL;
  }
  else
  {
    return &it->second;
  }
}

void ArgumentParserInternals::addStandalone(const char *standalone)
{
  if (maxStandalones != -1 && (unsigned) maxStandalones >= standalones.size())
  {
    throw runtime_error("maximum number of standalone arguments exceeded");
  }

  standalones.push_back(strdup(standalone));
}

Argument *ArgumentParserInternals::fetchArgument(const char *longKey,
    bool useDefault)
{
  ArgumentMap::iterator it = arguments.find(longKey);

  if (it == arguments.end())
  {
    if (useDefault)
    {
      return fetchDefault(longKey);
    }
    else
    {
      return NULL;
    }
  }
  else
  {
    if (useDefault && it->second.wasSet() == false)
    {
      return fetchDefault(longKey);
    }
    return &it->second;
  }
}

void ArgumentParserInternals::registerTarget(const char *longKey, void *target)
{
  if (target != NULL)
  {
    assert(longKey != NULL);
    targets.insert(TargetMap::value_type(strdup(longKey), target));
  }
}

void ArgumentParserInternals::registerComment(const char *longKey,
    const char *comment)
{
  if (comment == NULL)
  {
    return;
  }

  CommentMap::iterator it = comments.find(longKey);
  if (it == comments.end())
  {
    pair<CommentMap::iterator, bool> ret;
    ret = comments.insert(
        CommentMap::value_type(strdup(longKey), strdup(comment)));
    assert(ret.second == true);
    it = ret.first;
  }
  else
  {
    free(const_cast<char*> (it->second));
    it->second = strdup(comment);
  }

}

const char *ArgumentParserInternals::fetchComment(const char *longKey)
{
  CommentMap::iterator it = comments.find(longKey);

  if (it == comments.end())
  {
    return NULL;
  }
  else
  {
    return it->second;
  }
}

void ArgumentParserInternals::Bool(const char *longKey, const char *comment,
    unsigned char shortKey, bool *target)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::boolType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
  registerTarget(longKey, target);
}

void ArgumentParserInternals::Bool(const char *longKey, bool defaultValue,
    const char *comment, unsigned char shortKey, bool *target)
{
  Bool(longKey, comment, shortKey, target);
  Argument *argument = registerDefault(longKey, Argument::boolType);
  argument->set(defaultValue);
  setTarget(argument, target);
}

void ArgumentParserInternals::Int(const char *longKey, const char *comment,
    unsigned char shortKey, int *target)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::intType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
  registerTarget(longKey, target);
}

void ArgumentParserInternals::Int(const char *longKey, int defaultValue,
    const char *comment, unsigned char shortKey, int *target)
{
  Int(longKey, comment, shortKey, target);
  Argument *argument = registerDefault(longKey, Argument::intType);
  argument->set(defaultValue);
  setTarget(argument, target);
}

void ArgumentParserInternals::UInt(const char *longKey, const char *comment,
    unsigned char shortKey, unsigned int *target)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::uintType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
  registerTarget(longKey, target);
}

void ArgumentParserInternals::UInt(const char *longKey,
    unsigned int defaultValue, const char *comment, unsigned char shortKey,
    unsigned int *target)
{
  UInt(longKey, comment, shortKey, target);
  Argument *argument = registerDefault(longKey, Argument::uintType);
  argument->set(defaultValue);
  setTarget(argument, target);
}

void ArgumentParserInternals::Double(const char *longKey, const char *comment,
    unsigned char shortKey, double *target)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::doubleType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
  registerTarget(longKey, target);
}

void ArgumentParserInternals::Double(const char *longKey, double defaultValue,
    const char *comment, unsigned char shortKey, double *target)
{
  Double(longKey, comment, shortKey, target);
  Argument *argument = registerDefault(longKey, Argument::doubleType);
  argument->set(defaultValue);
  setTarget(argument, target);
}

void ArgumentParserInternals::String(const char *longKey, const char *comment,
    unsigned char shortKey, char *target)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::stringType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
  registerTarget(longKey, target);
}

void ArgumentParserInternals::String(const char *longKey,
    const char *defaultValue, const char *comment, unsigned char shortKey,
    char *target)
{
  String(longKey, comment, shortKey, target);
  Argument *argument = registerDefault(longKey, Argument::stringType);
  argument->set(defaultValue);
  setTarget(argument, target);
}

void ArgumentParserInternals::Standalones(int maximum)
{
  if (standalones.size() != 0)
  {
    throw runtime_error(
        "can't change maximum number of standalone arguments: arguments have already been read");
  }

  if (maximum < 0)
  {
    maxStandalones = -1;
  }
  else
  {
    maxStandalones = maximum;
  }
}

void ArgumentParserInternals::File(const char *longKey, const char *comment,
    unsigned char shortKey)
{
  assert(longKey != NULL);
  registerArgument(longKey, Argument::noType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
}

void ArgumentParserInternals::registerShortKey(unsigned char shortKey,
    const char *longKey)
{
  if (isgraph(shortKey))
  {
    clearShortKey(shortKey);
    if (longKey != NULL)
    {
      shortKeys[shortKey] = strdup(longKey);
    }
  }
}

bool ArgumentParserInternals::keyExists(const char *longKey)
{
  return (fetchArgument(longKey)) != NULL;
}

bool ArgumentParserInternals::wasValueSet(const char *longKey,
    bool includeDefault)
{
  Argument *argument = fetchArgument(longKey, includeDefault);
  if (argument == NULL)
  {
    return false;
  }

  return argument->wasSet();
}

bool ArgumentParserInternals::shortKeyExists(unsigned char shortKey)
{
  return (shortKeys[shortKey] != NULL);
}

const char *ArgumentParserInternals::getLongKey(unsigned char shortKey)
{
  return shortKeys[shortKey];
}

void ArgumentParserInternals::getLongKey(unsigned char shortKey, char *output)
{
  if (output == NULL)
  {
    return;
  }

  if (shortKeyExists(shortKey))
  {
    strcpy(output, shortKeys[shortKey]);
  }
  else
  {
    output[0] = '\0';
  }
}

bool ArgumentParserInternals::allValuesSet(const char *errorFormat)
{
  bool retval = true;
  for (ArgumentMap::iterator it = arguments.begin(); it != arguments.end(); ++it)
  {
    if (!wasValueSet(it->first, true) && !it->second.hasType(Argument::noType))
    {
      if (errorFormat == NULL)
      {
        return false;
      }
      else
      {
        fprintf(stderr, errorFormat, it->first);
        retval = false;
      }
    }
  }

  return retval;
}

bool ArgumentParserInternals::getBool(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::boolType));
  return argument->getBool();
}

int ArgumentParserInternals::getInt(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::intType));
  return argument->getInt();
}

unsigned int ArgumentParserInternals::getUInt(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::uintType));
  return argument->getUInt();
}

double ArgumentParserInternals::getDouble(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::doubleType));
  return argument->getDouble();
}

void ArgumentParserInternals::getString(const char *longKey, char *output)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::stringType));
  strcpy(output, argument->getString());
}

const char *ArgumentParserInternals::getCString(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  assert(argument != NULL);
  assert(argument->hasType(Argument::stringType));
  return argument->getString();
}

int ArgumentParserInternals::getStandaloneCount()
{
  return standalones.size();
}

void ArgumentParserInternals::getStandalone(unsigned int index, char *output)
{
  if (index >= standalones.size())
  {
    throw runtime_error("getStandalone: invalid index");
  }
  assert(output != NULL);

  strcpy(output, standalones[index]);
}

const char *ArgumentParserInternals::getCStandalone(unsigned int index)
{
  if (index >= standalones.size())
  {
    throw runtime_error("getStandalone: invalid index");
  }

  return standalones[index];
}

void ArgumentParserInternals::set(const char *longKey, bool value)
{
  Argument *argument = fetchArgument(longKey);

  assert(argument != NULL);

  argument->set(value);

  setTargets(longKey);
}

void ArgumentParserInternals::set(const char *longKey, int value)
{
  Argument *argument = fetchArgument(longKey);

  assert(argument != NULL);

  argument->set(value);

  setTargets(longKey);
}

void ArgumentParserInternals::set(const char *longKey, unsigned int value)
{
  Argument *argument = fetchArgument(longKey);

  assert(argument != NULL);

  argument->set(value);

  setTargets(longKey);
}

void ArgumentParserInternals::set(const char *longKey, double value)
{
  Argument *argument = fetchArgument(longKey);

  assert(argument != NULL);

  argument->set(value);

  setTargets(longKey);
}

void ArgumentParserInternals::set(const char *longKey, const char *value)
{
  Argument *argument = fetchArgument(longKey);

  assert(argument != NULL);

  if (argument->getType() == Argument::noType)
  {
    parseFile(value);
  }
  else
  {
    argument->set(value);
  }

  setTargets(longKey);
}

void ArgumentParserInternals::setTarget(Argument *argument, void *target)
{
  if (argument && target)
  {
    switch (argument->getType())
    {
    case Argument::noType:
      assert(argument->getType() != Argument::noType);
      break;
    case Argument::boolType:
      *reinterpret_cast<bool*> (target) = argument->getBool();
      break;
    case Argument::intType:
      *reinterpret_cast<int*> (target) = argument->getInt();
      break;
    case Argument::uintType:
      *reinterpret_cast<unsigned int*> (target) = argument->getUInt();
      break;
    case Argument::doubleType:
      *reinterpret_cast<double*> (target) = argument->getDouble();
      break;
    case Argument::stringType:
      strcpy(reinterpret_cast<char*> (target), argument->getString());
      break;
    }
  }
}

void ArgumentParserInternals::setTargets(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);

  if (argument == NULL)
  {
    return;
  }

  TargetRange range = targets.equal_range(longKey);

  for (TargetMap::iterator it = range.first; it != range.second; ++it)
  {
    setTarget(argument, it->second);
  }
}

void ArgumentParserInternals::setAllTargets()
{
  for (TargetMap::iterator it = targets.begin(); it != targets.end(); ++it)
  {
    setTarget(fetchArgument(it->first, true), it->second);
  }
}

void ArgumentParserInternals::setProgName(const char *_progname)
{
  free(progname);
  assert(_progname != NULL);
  progname = strdup(_progname);
}

void ArgumentParserInternals::parseFile(const char *filename)
{
  ifstream file(filename);
  if (!file.is_open())
  {
    //    err << "can't open file " << filename << endl;
    throw runtime_error("can't read from file");
  }

  char lineBuffer[1024];
  while (!file.eof())
  {
    file.getline(lineBuffer, 1024);
    parseLine(lineBuffer);
  }

  file.close();
}

void ArgumentParserInternals::parseLine(const char *line)
{
  // line format (regex): /^\s*\([a-zA-Z0-9]*\)\s*=\s*\(\S*\)\s*$/

  if (line == NULL || line[0] == '\0')
  {
    return;
  }

  const char *keyStart = line;
  // strip leading blanks
  while (isblank(*keyStart))
  {
    ++keyStart;
  }

  // abort on \0 or !alnum
  if (!isalnum(*keyStart))
  {
    switch (keyStart[0])
    {
    case '\0':
    case '%':
    case '#':
    case '"':
    case '/':
    case '!':
      break;
    default:
    {
      //      err
      //          << "ArgumentParser::parseLine: unexpected character at beginning of line '"
      //          << line << "'" << endl;
    }
    }
    return;
  }

  const char *keyEnd = keyStart;
  // find end of key
  while (isalnum(*keyEnd))
  {
    ++keyEnd;
  }

  // strip blanks in front of '='
  const char *ptr = keyEnd;
  while (isblank(*ptr))
  {
    ++ptr;
  }

  // validate existing '='
  if (*ptr != '=')
  {
    //    err << "ArgumentParser::parseLine: missing '=' in line '" << line << "'"
    //        << endl;
    return;
  }

  const char *valueStart = ptr + 1;
  // strip blanks in front of value
  while (isblank(*valueStart))
  {
    ++valueStart;
  }

  // validate valid value
  if (!isprint(*valueStart))
  {
    //    err
    //        << "ArgumentParser::parseLine: missing value or unexpected symbol in line '"
    //        << line << "'" << endl;
    return;
  }

  // search for unexpected symbols in value
  const char *valueEnd = valueStart;
  while (isprint(*valueEnd))
  {
    ++valueEnd;
  }
  // if everything's correct, we're at the end of the string
  if (*valueEnd != '\0')
  {
    //    err << "ArgumentParser::parseLine: unexpected symbol in value of line '"
    //        << line << "'" << endl;
    return;
  }

  // strip trailing blanks
  --valueEnd;
  while (isblank(*valueEnd))
  {
    --valueEnd;
  }

  ++valueEnd;
  assert(valueEnd > valueStart);

  char longKey[1024];
  memset(longKey, '\0', sizeof(longKey));
  memcpy(longKey, keyStart, keyEnd - keyStart);

  char value[1024];
  memset(value, '\0', sizeof(value));
  memcpy(value, valueStart, valueEnd - valueStart);

  set(longKey, value);
}

void ArgumentParserInternals::parseArgs(int argc, char **argv)
{
  const char *lastKey = NULL; // always a long key

  for (int i = 1; i < argc; ++i)
  {
    if (argv[i][0] != '-')
    {
      // this is a value
      if (lastKey == NULL)
      {
        addStandalone(argv[i]);
      }

      set(lastKey, argv[i]);
      lastKey = NULL;
    }
    else
    {
      if (lastKey)
      {
        // must be boolean (i.e. true)
        set(lastKey, true);
        lastKey = NULL;
      }

      if (argv[i][1] == '-')
      {
        // this is a long key

        char *arg = &(argv[i][2]);
        char *eqpos = strchr(arg, '=');
        if (eqpos != NULL)
        {
          if (eqpos == arg)
          {
            //            err << "missing key in option'" << arg << "'" << endl;
            throw runtime_error("missing key");
          }

          *eqpos = '\0';
          ++eqpos;
          set(arg, eqpos);
        }
        else
        {
          lastKey = &(argv[i][2]);
        }
      }
      else
      {
        char *keys = &(argv[i][1]);
        int size = strlen(keys) - 1;

        char *eqpos = strchr(keys, '=');
        if (eqpos == NULL)
        {
          for (int j = 0; j < size; ++i)
          {
            // must be bool
            set(getLongKey(keys[j]), true);
          }

          lastKey = getLongKey(keys[size]);
        }
        else if (eqpos - keys != 1)
        {
          //          err << "syntax error in option " << argv[i] << endl;
          throw runtime_error("arguments: syntax error");
        }
        else
        {
          ++eqpos;
          set(getLongKey(keys[0]), eqpos);
        }

        // this is a short key (or sequence thereof)
      }
    }
  }

  if (lastKey != NULL)
  {
    set(lastKey, true);
  }
}

void ArgumentParserInternals::displayHelpMessage()
{
  printf("\nusage: %s [options]\n\nOptions:\n", progname);

  for (ArgumentMap::iterator it = arguments.begin(); it != arguments.end(); ++it)
  {
    const char *longKey = it->first;
    const char *comment = fetchComment(longKey);
    Argument *defaultValue = fetchDefault(longKey);
    Argument *argument = fetchArgument(longKey);
    // print line with keys:
    for (int i = 0; i < 256; ++i)
    {
      if (shortKeys[i] != NULL && strcmp(shortKeys[i], longKey) == 0)
      {
        printf("-%c, ", char(i));
      }
    }

    printf("--%s", longKey);

    switch (argument->getType())
    {
    case Argument::intType:
      printf("  [int]");
      break;
    case Argument::uintType:
      printf("  [uint]");
      break;
    case Argument::doubleType:
      printf("  [double]");
      break;
    case Argument::stringType:
      printf("  [string]");
      break;
    default:
      break;
    }

    if (defaultValue != NULL)
    {
      assert(defaultValue->wasSet());
      printf("   (default = ");
      switch (defaultValue->getType())
      {
      case Argument::noType:
        assert(defaultValue->getType() != Argument::noType);
        break;
      case Argument::boolType:
        if (defaultValue->getBool())
        {
          printf("true");
        }
        else
        {
          printf("false");
        }
        break;
      case Argument::intType:
        printf("%d", defaultValue->getInt());
        break;
      case Argument::uintType:
        printf("%u", defaultValue->getUInt());
        break;
      case Argument::doubleType:
        printf("%lg", defaultValue->getDouble());
        break;
      case Argument::stringType:
        printf("'%s'", defaultValue->getString());
        break;
      }

      printf(")");
    }
    if (comment == NULL)
    {
      printf("\n");
      continue;
    }

    printf(":\n\t%s\n", comment);
  }

  printf("\n");
}
