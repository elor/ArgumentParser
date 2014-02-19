/**
 * @author Erik Lorenz <elor@hrz.tu-chemnitz.de>
 *
 * This header contains general debugging (and runtime error) definitions.
 * For a release build, simply #define RELEASE
 */

#pragma once

#include <stdexcept>
#include <cassert>

#ifndef RELEASE
#include <iostream>
#endif
