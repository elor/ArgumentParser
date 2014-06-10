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
#include <cstdlib>

using namespace std;

ArgumentParserInternals::CallbackContainer::CallbackContainer(
  Callback _callback, void *_data) :
  callback(_callback), data(_data)
{
}

ArgumentParserInternals::ArgumentParserInternals(const char *_progname) :
  shortKeys(new char*[256]), maxStandalones(0), standaloneComment(NULL),
    standaloneHelpKey(strdup("argument"))
{
  progname = strdup(_progname);
  for (int i = 0; i < 256; ++i)
  {
    shortKeys[i] = NULL;
  }

  Bool("help", false, "display help message and exit", 'h', NULL);
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
    clearShortKey((unsigned char) shortKey);
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
    free(const_cast<char*>(it->first));
    arguments.erase(it);
  }
}

void ArgumentParserInternals::clearTargets()
{
  while (!targets.empty())
  {
    TargetMap::iterator it = targets.begin();
    free(const_cast<char*>(it->first));
    targets.erase(it);
  }
}

void ArgumentParserInternals::clearDefaults()
{
  while (!defaults.empty())
  {
    ArgumentMap::iterator it = defaults.begin();
    free(const_cast<char*>(it->first));
    defaults.erase(it);
  }
}

void ArgumentParserInternals::clearComments()
{
  while (!comments.empty())
  {
    CommentMap::iterator it = comments.begin();
    free(const_cast<char*>(it->first));
    free(const_cast<char*>(it->second));
    comments.erase(it);
  }
}

void ArgumentParserInternals::clearStandalones()
{
  for (StandaloneVector::iterator it = standalones.begin();
    it != standalones.end(); ++it)
  {
    free(const_cast<char*>(*it));
  }

  standalones.clear();
  if (standaloneHelpKey)
  {
    free(standaloneHelpKey);
    standaloneHelpKey = NULL;
  }
  if (standaloneComment)
  {
    free(standaloneComment);
    standaloneComment = NULL;
  }
}

void ArgumentParserInternals::clearCallbacks()
{
  while (!callbacks.empty())
  {
    CallbackMap::iterator it = callbacks.begin();
    free(const_cast<char*>(it->first));
    callbacks.erase(it);
  }
}

void ArgumentParserInternals::clearAll()
{
  clearShortKeys();
  clearArguments();
  clearTargets();
  clearDefaults();
  clearComments();
  clearStandalones();
  clearCallbacks();
}

void ArgumentParserInternals::lookForHelp()
{
  if (getBool("help"))
  {
    displayHelpMessage();
    exit(0);
  }
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
#ifdef DEBUG
    cerr << "ERROR: invalid key: '" << longKey << "'" << endl;
#endif
    return NULL;
    //    throw runtime_error("invalid args key");
  }

  ArgumentMap::iterator it = arguments.find(longKey);
  if (it == arguments.end())
  {
    pair<ArgumentMap::iterator, bool> ret;
    ret = arguments.insert(
      ArgumentMap::value_type(strdup(longKey), Argument(valueType)));

    if (ret.second != true)
    {
#ifdef DEBUG
      cerr << "argument already registered: " << longkey << endl;
#endif
      return NULL;
    }
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

    if (ret.second != true)
    {
#ifdef DEBUG
      cerr << "default value already exists: " << longkey << endl;
#endif
    }
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
  } else
  {
    return &it->second;
  }
}

#include<iostream>
void ArgumentParserInternals::addStandalone(const char *standalone)
{
  if (maxStandalones != -1 && standalones.size() >= (unsigned) maxStandalones)
  {
    //    throw runtime_error("maximum number of standalone arguments exceeded");
    return;
  }

  standalones.push_back(strdup(standalone));
  // call standalone callback!
  fireCallbacks("");
}

void ArgumentParserInternals::fireCallbacks(const char *longKey)
{
  CallbackRange range = callbacks.equal_range(longKey);
#ifdef DEBUG
  cout << "range for " << longKey << "' :"
  << (range.first == range.second ? "empty" : "full") << endl;
#endif

  for (CallbackMap::iterator it = range.first; it != range.second; ++it)
  {
#ifdef DEBUG
    cout << "firing '" << longKey << "'" << endl;
#endif

    Callback callback = it->second.callback;
    void *data = it->second.data;

    callback(data);
  }
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
    } else
    {
      return NULL;
    }
  } else
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
    if (longKey != NULL)
    {
      if (keyExists(longKey))
      {
        targets.insert(TargetMap::value_type(strdup(longKey), target));
      }
    }
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
    if (ret.second != true)
    {
#ifdef DEBUG
      cerr << "Comment already registered: " << longkey << endl;
#endif
      // TODO overwrite comment
      return;
    }
    it = ret.first;
  } else
  {
    free(const_cast<char*>(it->second));
    it->second = strdup(comment);
  }

}

const char *ArgumentParserInternals::fetchComment(const char *longKey)
{
  CommentMap::iterator it = comments.find(longKey);

  if (it == comments.end())
  {
    return NULL;
  } else
  {
    return it->second;
  }
}

void ArgumentParserInternals::Bool(const char *longKey, const char *comment,
  unsigned char shortKey, bool *target)
{
  if (longKey == NULL)
    return;

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
  if (longKey == NULL)
    return;

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
  if (longKey == NULL)
    return;

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
  if (longKey == NULL)
    return;

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
  if (longKey == NULL)
    return;

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

void ArgumentParserInternals::registerCallback(const char *longKey,
  Callback callback, void* data)
{
  if (longKey)
  {
    if (keyExists(longKey))
    {
      callbacks.insert(
        CallbackMap::value_type(strdup(longKey),
          CallbackContainer(callback, data)));
    } else
    {
      // longKey does not exist
      // TODO freak out
    }
  } else
  {
    callbacks.insert(
      CallbackMap::value_type(strdup(""), CallbackContainer(callback, data)));
  }
}

void ArgumentParserInternals::Standalones(int maximum, const char *helpKey,
  const char *comment)
{
  if (standalones.size() != 0)
  {
#ifdef DEBUG
    cerr
    << "can't change maximum number of standalone arguments: arguments have already been read"
    << endl;
#endif
    return;
    //    throw runtime_error(
    //        "can't change maximum number of standalone arguments: arguments have already been read");
  }

  if (maximum < 0)
  {
    maxStandalones = -1;
  } else
  {
    maxStandalones = maximum;
  }

  if (standaloneHelpKey)
  {
    free(standaloneHelpKey);
    standaloneHelpKey = NULL;
  }
  if (helpKey != NULL)
  {
    // THIS is not ideal
    standaloneHelpKey = strdup(helpKey);
  }

  if (standaloneComment)
  {
    free(standaloneComment);
    standaloneComment = NULL;
  }
  if (comment)
  {
    standaloneComment = strdup(comment);
  }

}

void ArgumentParserInternals::File(const char *longKey, const char *comment,
  unsigned char shortKey)
{
  if (longKey == NULL)
    return;

  registerArgument(longKey, Argument::noType);
  registerShortKey(shortKey, longKey);
  registerComment(longKey, comment);
}

void ArgumentParserInternals::registerShortKey(unsigned char shortKey,
  const char *longKey)
{
  if (isgraph(shortKey))
  {
    if (shortKeys[shortKey] == NULL)
    {
      clearShortKey(shortKey);
      if (longKey != NULL)
      {
        shortKeys[shortKey] = strdup(longKey);
      }
    } else
    {
      cerr << "shortkey -" << shortKey << " is already registered as --"
        << shortKeys[shortKey] << endl;
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
  if (shortKeys[shortKey] == NULL)
  {
    cerr << "'" << shortKey << "' is no valid shortkey" << endl;
    return NULL;
  }
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
  } else
  {
    output[0] = '\0';
  }
}

bool ArgumentParserInternals::allValuesSet(const char *errorFormat)
{
  bool retval = true;
  for (ArgumentMap::iterator it = arguments.begin(); it != arguments.end();
    ++it)
  {
    if (!wasValueSet(it->first, true) && !it->second.hasType(Argument::noType))
    {
      if (errorFormat == NULL)
      {
        return false;
      } else
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
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    return false;
  } else if (!argument->hasType(Argument::boolType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'Bool'"
      << endl;
    return false;
  }
  return argument->getBool();
}

int ArgumentParserInternals::getInt(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    return 0;
  } else if (!argument->hasType(Argument::intType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'Int'"
      << endl;
    return 0;
  }
  return argument->getInt();
}

unsigned int ArgumentParserInternals::getUInt(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    return 0;
  } else if (!argument->hasType(Argument::uintType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'UInt'"
      << endl;
    return 0;
  }
  return argument->getUInt();
}

double ArgumentParserInternals::getDouble(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    return 0.0;
  } else if (!argument->hasType(Argument::doubleType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'Double'"
      << endl;
    return 0.0;
  }
  return argument->getDouble();
}

void ArgumentParserInternals::getString(const char *longKey, char *output)
{
  Argument *argument = fetchArgument(longKey, true);
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    strcpy(output, "");
  } else if (!argument->hasType(Argument::stringType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'String'"
      << endl;
    strcpy(output, "");
  }
  strcpy(output, argument->getString());
}

const char *ArgumentParserInternals::getCString(const char *longKey)
{
  Argument *argument = fetchArgument(longKey, true);
  if (argument == NULL)
  {
    cerr << "error: option '" << longKey << "' not defined" << endl;
    return NULL;
  } else if (!argument->hasType(Argument::stringType))
  {
    cerr << "error: option '" << longKey << "' does not have type 'String'"
      << endl;
    return NULL;
  }
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
    cerr << "getStandalone: invalid index" << endl;
    strcpy(output, "");
  }

  if (output == NULL)
  {
#ifdef DEBUG
    cerr << "getStandalone: output pointer is NULL" << endl;
#endif
    return;
  }

  strcpy(output, standalones[index]);
}

const char *ArgumentParserInternals::getCStandalone(unsigned int index)
{
  if (index == (unsigned int) -1)
  {
    if (getStandaloneCount() == 0)
    {
      cerr << "getCStandalone: list of standalones is empty" << endl;
      return NULL;
    }
    return standalones[getStandaloneCount() - 1];
  } else if (index >= standalones.size())
  {
    cerr << "getCStandalone: invalid index" << endl;
    return NULL;
    //    throw runtime_error("getCStandalone: invalid index");
  }

  return standalones[index];
}

void ArgumentParserInternals::set(const char *longKey, bool value)
{
  Argument *argument = fetchArgument(longKey);

  if (argument == NULL)
  {
    cerr << "'" << longKey << "' is no valid argument" << endl;
    return;
  }

  argument->set(value);

  setTargets(longKey);

  fireCallbacks(longKey);
}

void ArgumentParserInternals::set(const char *longKey, int value)
{
  Argument *argument = fetchArgument(longKey);

  if (argument == NULL)
  {
    cerr << "'" << longKey << "' is no valid argument" << endl;
    return;
  }

  argument->set(value);

  setTargets(longKey);

  fireCallbacks(longKey);
}

void ArgumentParserInternals::set(const char *longKey, unsigned int value)
{
  Argument *argument = fetchArgument(longKey);

  if (argument == NULL)
  {
    cerr << "'" << longKey << "' is no valid argument" << endl;
    return;
  }

  argument->set(value);

  setTargets(longKey);

  fireCallbacks(longKey);
}

void ArgumentParserInternals::set(const char *longKey, double value)
{
  Argument *argument = fetchArgument(longKey);

  if (argument == NULL)
  {
    cerr << "'" << longKey << "' is no valid argument" << endl;
    return;
  }

  argument->set(value);

  setTargets(longKey);

  fireCallbacks(longKey);
}

void ArgumentParserInternals::set(const char *longKey, const char *value)
{
  Argument *argument = fetchArgument(longKey);

  if (argument == NULL)
  {
    cerr << "'" << longKey << "' is no valid argument" << endl;
    return;
  }

  if (argument->getType() == Argument::noType)
  {
    parseFile(value);
  } else
  {
    argument->set(value);
  }

  setTargets(longKey);

#ifdef DEBUG
  cout << "fireCallbacks for key '" << longKey << "'" << endl;
#endif
  fireCallbacks(longKey);
}

void ArgumentParserInternals::setTarget(Argument *argument, void *target)
{
  if (argument && target)
  {
    switch (argument->getType())
    {
    case Argument::noType:
      cerr << "can not set target for file option. Use parseFile() instead"
        << endl;
      return;
    case Argument::boolType:
      *reinterpret_cast<bool*>(target) = argument->getBool();
      break;
    case Argument::intType:
      *reinterpret_cast<int*>(target) = argument->getInt();
      break;
    case Argument::uintType:
      *reinterpret_cast<unsigned int*>(target) = argument->getUInt();
      break;
    case Argument::doubleType:
      *reinterpret_cast<double*>(target) = argument->getDouble();
      break;
    case Argument::stringType:
      strcpy(reinterpret_cast<char*>(target), argument->getString());
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

  if (progname == NULL)
  {
    progname = strdup("NULL");
  } else
  {
    progname = strdup(_progname);
  }
}

void ArgumentParserInternals::parseFile(const char *filename)
{
  ifstream file(filename);
  if (!file.is_open())
  {
    cerr << "can't open file " << filename << endl;
    return;
  }

  char lineBuffer[1024];
  while (!file.eof())
  {
    file.getline(lineBuffer, 1024);
    parseLine(lineBuffer);
  }

  file.close();

  lookForHelp();
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
      cerr << "ArgumentParser::parseLine:"
        << " unexpected character at beginning of line '" << line << "'"
        << endl;
      return;
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
    //    cerr << "ArgumentParser::parseLine: missing '=' in line '" << line << "'"
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
    //    cerr
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
    //    cerr << "ArgumentParser::parseLine: unexpected symbol in value of line '"
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
  if (valueEnd <= valueStart)
  {
    cerr << "syntax error in line '" << line << "'" << endl;
    return;
  }

  char longKey[1024];
  memset(longKey, '\0', sizeof(longKey));
  memcpy(longKey, keyStart, keyEnd - keyStart);

  char value[1024];
  memset(value, '\0', sizeof(value));
  memcpy(value, valueStart, valueEnd - valueStart);

  set(longKey, value);

  lookForHelp();
}

void ArgumentParserInternals::parseArgs(int argc, char **argv)
{
  const char *lastKey = NULL; // always a long key or NULL

  if (argc <= 0)
  {
#ifdef DEBUG
    cerr << "parseArgs: too few arguments (argc too low)" << endl;
#endif
  }

  if (strlen(progname) == 0)
  {
#ifdef DEBUG
    cout << "setting progname to "<< argv[0] << endl;
#endif

    setProgName(argv[0]);
  }

  for (int i = 1; i < argc; ++i)
  {
#ifdef DEBUG
    cout << "parsing argument #" << i << ": '" << argv[i] << "'" << endl;
#endif
    if (argv[i][0] != '-')
    {
      // this is a value
      if (lastKey == NULL)
      {
#ifdef DEBUG
        cout << "adding standalone argument"<< endl;
#endif

        addStandalone(argv[i]);
      } else
      {
        set(lastKey, argv[i]);
        lastKey = NULL;
      }
    } else
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
            cerr << "missing key in option '" << arg << "'" << endl;
            return;
          } else
          {
            *eqpos = '\0';
            ++eqpos;
            set(arg, eqpos);
          }
        } else
        {
          lastKey = &(argv[i][2]);
        }
      } else
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
        } else if (eqpos - keys != 1)
        {
          cerr << "syntax error in option '" << argv[i] << "'" << endl;
          return;
        } else
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
#ifdef DEBUG
    cout << "setting last key to true" << endl;
#endif

    set(lastKey, true);
  }

#ifdef DEBUG
  cout << "done parsing"<< endl;
#endif

  lookForHelp();
}

bool ArgumentParserInternals::writeFile(const char *filename)
{
  ofstream file(filename);
  if (!file.is_open())
  {
    cerr << "can't open file '" << filename << "' for writing" << endl;
    //    throw runtime_error("can't read from file");
    return true;
  }

  for (ArgumentMap::iterator it = arguments.begin(); it != arguments.end();
    ++it)
  {
    if (wasValueSet(it->first, true) && !it->second.hasType(Argument::noType))
    {
      file << it->first << " = ";
      file.precision(16);
      switch (it->second.getType())
      {
      case Argument::boolType:
        file << (it->second.getBool() ? "true" : "false");
        break;
      case Argument::intType:
        file << it->second.getInt();
        break;
      case Argument::uintType:
        file << it->second.getUInt();
        break;
      case Argument::doubleType:
        file << it->second.getDouble();
        break;
      case Argument::stringType:
        file << it->second.getString();
        break;
      case Argument::noType:
        // already handled in condition block
        break;
      }
      file << endl;
    }
  }

  file.close();

  return false;
}

void ArgumentParserInternals::displayHelpMessage()
{
  printf("\nusage: %s [options]", progname);
  if (standaloneHelpKey == NULL)
  {
    cerr << "no standaloneHelpKey defined" << endl;
    standaloneHelpKey = strdup("argument");
  }

  switch (maxStandalones)
  {
  case 0:
    break;
  case -1:
    printf(" %s... (unlimited)", standaloneHelpKey);
    break;
  case 1:
    printf(" %s", standaloneHelpKey);
    break;
  default:
    printf(" %s... (up to %d)", standaloneHelpKey, maxStandalones);
    break;
  }

  if (maxStandalones != 0 && standaloneComment != NULL)
  {
    printf("\n\n%s:\n\t%s", standaloneHelpKey, standaloneComment);
  }

  printf("\n\nOptions:\n");

  for (ArgumentMap::iterator it = arguments.begin(); it != arguments.end();
    ++it)
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
#ifndef RELEASE
      assert(defaultValue->wasSet());
#endif
      printf("   (default = ");
      switch (defaultValue->getType())
      {
      case Argument::noType:
        cerr << "file options must not have default values" << endl;
        return;
      case Argument::boolType:
        if (defaultValue->getBool())
        {
          printf("true");
        } else
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
