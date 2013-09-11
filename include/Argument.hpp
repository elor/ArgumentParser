/*
 * Argument.h
 *
 *  Created on: Aug 23, 2013
 *      Author: elor
 */

#ifndef ARGUMENT_H_
#define ARGUMENT_H_

class Argument
{
public:
  enum ValueType
  {
    noType, boolType, intType, uintType, doubleType, stringType
  };

private:
  union
  {
    bool boolValue;
    int intValue;
    unsigned int uintValue;
    double doubleValue;
    const char *stringValue;
  };

  bool defined;
  ValueType valueType;

  Argument();

public:
  Argument(ValueType type);
  virtual ~Argument();

  void clear();

  void set(bool value);
  void set(int value);
  void set(unsigned int value);
  void set(double value);
  void set(const char *value);

  void setType(ValueType _wantedType);
  ValueType getType();
  bool hasType(ValueType type) const;

  bool wasSet() const;

  bool getBool() const;
  int getInt() const;
  unsigned int getUInt() const;
  double getDouble() const;
  const char *getString() const;
};
#endif /* ARGUMENT_H_ */
