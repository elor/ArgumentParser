/*
 * Argument.cpp
 *
 *  Created on: Aug 23, 2013
 *      Author: elor
 */

#include <Argument.hpp>
#include <convert.hpp>
#include <debug.hpp>
#include <cstring>

Argument::Argument(ValueType _wantedType) :
  stringValue(NULL), defined(false), valueType(_wantedType)
{
}

Argument::~Argument()
{
  clear();
}

void Argument::clear()
{
  if (valueType == stringType && stringValue != NULL)
  {
    delete[] stringValue;
  }

  stringValue = NULL;
  defined = false;
}

void Argument::set(bool value)
{
  clear();
  assert(valueType == boolType);
  boolValue = value;
  defined = true;
}

void Argument::set(int value)
{
  clear();
  assert(valueType == intType);
  intValue = value;
  defined = true;
}

void Argument::set(unsigned int value)
{
  clear();
  assert(valueType == uintType);
  uintValue = value;
  defined = true;
}

void Argument::set(double value)
{
  clear();
  assert(valueType == doubleType);
  doubleValue = value;
  defined = true;
}

void Argument::set(const char *value)
{
  clear();
  assert(value != NULL);
  assert(valueType != noType);

  switch (valueType)
  {
  case noType:
    break;
  case boolType:
  {
    bool boolValue;
    if (convert(value, &boolValue) == 0)
    {
      set(boolValue);
    }
    else
    {
      //      err << "Argument::set: '" << value << " doesn't match type 'bool'"
      //          << endl;
    }
    break;
  }
  case intType:
  {
    int intValue;
    if (convert(value, &intValue) == 0)
    {
      set(intValue);
    }
    else
    {
      //      err << "Argument::set: '" << value << " doesn't match type 'int'" << endl;
    }
    break;
  }
  case uintType:
  {
    unsigned int uintValue;
    if (convert(value, &uintValue) == 0)
    {
      set(uintValue);

    }
    else
    {
      //      err << "Argument::set: '" << value << " doesn't match type 'uint'"
      //          << endl;
    }
    break;
  }
  case doubleType:
  {
    double doubleValue;
    if (convert(value, &doubleValue) == 0)
    {
      set(doubleValue);

    }
    else
    {
//      err << "Argument::set: '" << value << " doesn't match type 'double'"
//          << endl;
    }
    break;
  }
  case stringType:
  {
    stringValue = strdup(value);
    defined = true;
    break;
  }
  }
}

void Argument::setType(ValueType _wantedType)
{
  valueType = _wantedType;
}

Argument::ValueType Argument::getType()
{
  return valueType;
}

bool Argument::hasType(ValueType type) const
{
  return valueType == type;
}

bool Argument::wasSet() const
{
  return defined;
}

bool Argument::getBool() const
{
  if (hasType(boolType))
  {
    return boolValue;
  }
  else
  {
    return false;
  }
}

int Argument::getInt() const
{
  if (hasType(intType))
  {
    return intValue;
  }
  else if (hasType(uintType))
  {
    return (int) uintValue;
  }
  else
  {
    return 0;
  }
}

unsigned int Argument::getUInt() const
{
  if (hasType(uintType))
  {
    return uintValue;
  }
  else if (hasType(intType))
  {
    return (unsigned int) intValue;
  }
  else
  {
    return 0;
  }
}

double Argument::getDouble() const
{
  if (hasType(doubleType))
  {
    return doubleValue;
  }
  else
  {
    return 0.0;
  }
}

const char *Argument::getString() const
{
  if (hasType(stringType))
  {
    return stringValue;
  }
  else
  {
    return NULL;
  }
}
