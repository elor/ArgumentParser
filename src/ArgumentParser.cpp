#include <ArgumentParser.h>
#include <ArgumentParserInternals.hpp>

ArgumentParser::ArgumentParser(const char *progname) :
  args(new ArgumentParserInternals(progname))
{
}

ArgumentParser::~ArgumentParser()
{
  delete args;
}

void ArgumentParser::Bool(const char *longKey, const char *comment,
    unsigned char shortKey, bool *target)
{
  args->Bool(longKey, comment, shortKey, target);
}

void ArgumentParser::Bool(const char *longKey, bool defaultValue,
    const char *comment, unsigned char shortKey, bool *target)
{
  args->Bool(longKey, defaultValue, comment, shortKey, target);
}

void ArgumentParser::Int(const char *longKey, const char *comment,
    unsigned char shortKey, int *target)
{
  args->Int(longKey, comment, shortKey, target);
}

void ArgumentParser::Int(const char *longKey, int defaultValue,
    const char *comment, unsigned char shortKey, int *target)
{
  args->Int(longKey, defaultValue, comment, shortKey, target);
}

void ArgumentParser::UInt(const char *longKey, const char *comment,
    unsigned char shortKey, unsigned int *target)
{
  args->UInt(longKey, comment, shortKey, target);
}

void ArgumentParser::UInt(const char *longKey, unsigned int defaultValue,
    const char *comment, unsigned char shortKey, unsigned int *target)
{
  args->UInt(longKey, defaultValue, comment, shortKey, target);
}

void ArgumentParser::Double(const char *longKey, const char *comment,
    unsigned char shortKey, double *target)
{
  args->Double(longKey, comment, shortKey, target);
}

void ArgumentParser::Double(const char *longKey, double defaultValue,
    const char *comment, unsigned char shortKey, double *target)
{
  args->Double(longKey, defaultValue, comment, shortKey, target);
}

void ArgumentParser::String(const char *longKey, const char *comment,
    char shortKey, char *target)
{
  args->String(longKey, comment, shortKey, target);
}

void ArgumentParser::String(const char *longKey, const char *defaultValue,
    const char *comment, char shortKey, char *target)
{
  args->String(longKey, defaultValue, comment, shortKey, target);
}

void ArgumentParser::Standalones(int maximum)
{
  args->Standalones(maximum);
}

// if one of the keys are encountered, a file is included (read in place)
void ArgumentParser::File(const char *longKey, const char *comment,
    unsigned char shortKey)
{
  args->File(longKey, comment, shortKey);
}

void ArgumentParser::registerShortKey(unsigned char shortKey,
    const char *longKey)
{
  args->registerShortKey(shortKey, longKey);
}

void ArgumentParser::registerComment(const char *longKey, const char *comment)
{
  args->registerComment(longKey, comment);
}

void ArgumentParser::registerTarget(const char *longKey, void *target)
{
  args->registerTarget(longKey, target);
}

bool ArgumentParser::keyExists(const char *longKey)
{
  return args->keyExists(longKey);
}

bool ArgumentParser::wasValueSet(const char *longKey, bool includeDefault)
{
  return args->wasValueSet(longKey, includeDefault);
}

bool ArgumentParser::shortKeyExists(unsigned char shortKey)
{
  return args->shortKeyExists(shortKey);
}

void ArgumentParser::getLongKey(unsigned char shortKey, char *output)
{
  args->getLongKey(shortKey, output);
}

bool ArgumentParser::allValuesSet(const char *errorFormat)
{
  return args->allValuesSet(errorFormat);
}

bool ArgumentParser::getBool(const char *longKey)
{
  return args->getBool(longKey);
}

int ArgumentParser::getInt(const char *longKey)
{
  return args->getInt(longKey);
}

unsigned int ArgumentParser::getUInt(const char *longKey)
{
  return args->getUInt(longKey);
}

double ArgumentParser::getDouble(const char *longKey)
{
  return args->getDouble(longKey);
}

void ArgumentParser::getString(const char *longKey, char *output)
{
  args->getString(longKey, output);
}

const char *ArgumentParser::getCString(const char *longKey)
{
  return args->getCString(longKey);
}

int ArgumentParser::getStandaloneCount()
{
  return args->getStandaloneCount();
}

void ArgumentParser::getStandalone(unsigned int index, char *output)
{
  args->getStandalone(index, output);
}

const char *ArgumentParser::getCStandalone(unsigned int index)
{
  return args->getCStandalone(index);
}

void ArgumentParser::set(const char *longKey, bool value)
{
  args->set(longKey, value);
}

void ArgumentParser::set(const char *longKey, int value)
{
  args->set(longKey, value);
}

void ArgumentParser::set(const char *longKey, unsigned int value)
{
  args->set(longKey, value);
}

void ArgumentParser::set(const char *longKey, double value)
{
  args->set(longKey, value);
}

void ArgumentParser::set(const char *longKey, const char *value)
{
  args->set(longKey, value);
}

void ArgumentParser::setProgName(const char *progname)
{
  args->setProgName(progname);
}

void ArgumentParser::parseFile(const char *filename)
{
  args->parseFile(filename);
}

void ArgumentParser::parseLine(const char *line)
{
  args->parseLine(line);
}

void ArgumentParser::parseArgs(int argc, char **argv)
{
  args->parseArgs(argc, argv);
}

void ArgumentParser::displayHelpMessage()
{
  args->displayHelpMessage();
}
