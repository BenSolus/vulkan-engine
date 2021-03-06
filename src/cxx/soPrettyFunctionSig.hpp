/*
 * Copyright (C) 2017-2018 by Bennet Carstensen
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

/**
 *  @file      cxx/soPrettyFunctionSig.hpp
 *  @author    Bennet Carstensen
 *  @date      2018
 *  @copyright Copyright (c) 2017-2018 Bennet Carstensen
 *
 *             Permission is hereby granted, free of charge, to any person
 *             obtaining a copy of this software and associated documentation
 *             files (the "Software"), to deal in the Software without
 *             restriction, including without limitation the rights to use,
 *             copy, modify, merge, publish, distribute, sublicense, and/or
 *             sell copies of the Software, and to permit persons to whom the
 *             Software is furnished to do so, subject to the following
 *             conditions:
 *
 *             The above copyright notice and this permission notice shall be
 *             included in all copies or substantial portions of the Software.
 *
 *             THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *             EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 *             OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 *             NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 *             HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 *             WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 *             FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 *             OTHER DEALINGS IN THE SOFTWARE.
 */

#pragma once

#include "soCompiler.hpp"
#include "soConstExpr.hpp"

#include <regex>
#include <string>

#if defined(__GNUC__) || defined(__clang__)

#define PRETTY_FUNCTION_SIG __PRETTY_FUNCTION__

#elif defined(_MSC_VER)

#define PRETTY_FUNCTION_SIG __FUNCSIG__

#elif defined(__func__)

#define PRETTY_FUNCTION_SIG __func__

#else

#define PRETTY_FUNCTION_SIG __FUNCTION__

#endif

#define PREPEND_FUNCTION_SIG_TO_STRING(target) \
  target.insert(0, PRETTY_FUNCTION_SIG);

namespace so {

template <class T>
std::string
getPrettyFunctionSig()
{
  std::string prettyFunctionSig{ PRETTY_FUNCTION_SIG };

  constExprIf<isGCCCompatible<Comp>> // if
  ([&]() // then
   {
     const std::regex rgx{ ".*T = (.*);.*" };

     std::smatch match;

     bool const foundMatch{ std::regex_search(prettyFunctionSig,
                                              match,
                                              rgx) };

     if(foundMatch)
     {
       prettyFunctionSig = match[1];
     }
     else
     {
       prettyFunctionSig = "";
     }
   },
   [&](){} // else
  );

  return prettyFunctionSig;
}

} // namespace so

