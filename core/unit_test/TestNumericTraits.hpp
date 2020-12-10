/*
//@HEADER
// ************************************************************************
//
//                        Kokkos v. 3.0
//       Copyright (2020) National Technology & Engineering
//               Solutions of Sandia, LLC (NTESS).
//
// Under the terms of Contract DE-NA0003525 with NTESS,
// the U.S. Government retains certain rights in this software.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// 1. Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright
// notice, this list of conditions and the following disclaimer in the
// documentation and/or other materials provided with the distribution.
//
// 3. Neither the name of the Corporation nor the names of the
// contributors may be used to endorse or promote products derived from
// this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY NTESS "AS IS" AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL NTESS OR THE
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
// Questions? Contact Christian R. Trott (crtrott@sandia.gov)
//
// ************************************************************************
//@HEADER
*/

#include <gtest/gtest.h>

#include <Kokkos_Core.hpp>
#include "Kokkos_ArithmeticTraits.hpp"
#include "Kokkos_ExecPolicy.hpp"

template <typename T>
struct extrema {
  static T min;
  static T max;
};

#define DEFINE_EXTREMA(T, m, M) \
  template <>                   \
  T extrema<T>::min = m;        \
  template <>                   \
  T extrema<T>::max = M

DEFINE_EXTREMA(char, CHAR_MIN, CHAR_MAX);
DEFINE_EXTREMA(signed char, SCHAR_MIN, SCHAR_MAX);
DEFINE_EXTREMA(unsigned char, 0, UCHAR_MAX);
DEFINE_EXTREMA(short, SHRT_MIN, SHRT_MAX);
DEFINE_EXTREMA(unsigned short, 0, USHRT_MAX);
DEFINE_EXTREMA(int, INT_MIN, INT_MAX);
DEFINE_EXTREMA(unsigned, 0U, UINT_MAX);
DEFINE_EXTREMA(long, LONG_MIN, LONG_MAX);
DEFINE_EXTREMA(unsigned long, 0UL, ULONG_MAX);
DEFINE_EXTREMA(long long, LLONG_MIN, LLONG_MAX);
DEFINE_EXTREMA(unsigned long long, 0ULL, ULLONG_MAX);

DEFINE_EXTREMA(float, -FLT_MAX, FLT_MAX);
DEFINE_EXTREMA(double, -DBL_MAX, DBL_MAX);
DEFINE_EXTREMA(long double, -LDBL_MAX, LDBL_MAX);

#undef DEFINE_EXTREMA

struct Infinity {};
struct Epsilon {};
struct FiniteMinMax {};

template <class Space, class T, class Tag>
struct TestNumericTraits {
  TestNumericTraits() { run(); }

  void run() const {
    int errors = 0;
    Kokkos::parallel_reduce(Kokkos::RangePolicy<Space, Tag>(0, 1), *this,
                            errors);
    ASSERT_EQ(errors, 0);
  }

  KOKKOS_FUNCTION void operator()(Infinity, int, int& e) const {
    auto const inf  = Kokkos::Experimental::infinity<T>::value;
    auto const zero = T(0);
    e += (int)!(inf + inf == inf);
    e += (int)!(inf != zero);
  }

  KOKKOS_FUNCTION void operator()(Epsilon, int, int& e) const {
    auto const eps = Kokkos::Experimental::epsilon<T>::value;
    auto const one = T(1);
    e += (int)!(one + eps != one);
    e += (int)!(one + eps / 2 == one);
  }

  KOKKOS_FUNCTION void operator()(FiniteMinMax, int, int& e) const {
    auto const min = Kokkos::Experimental::finite_min<T>::value;
    auto const max = Kokkos::Experimental::finite_max<T>::value;
    e += (int)!(min == extrema<T>::min);
    e += (int)!(max == extrema<T>::max);
  }
};

TEST(TEST_CATEGORY, numeric_traits_infinity) {
  TestNumericTraits<TEST_EXECSPACE, float, Infinity>();
  TestNumericTraits<TEST_EXECSPACE, double, Infinity>();
  TestNumericTraits<TEST_EXECSPACE, long double, Infinity>();
}

TEST(TEST_CATEGORY, numeric_traits_epsilon) {
  TestNumericTraits<TEST_EXECSPACE, float, Epsilon>();
  TestNumericTraits<TEST_EXECSPACE, double, Epsilon>();
  TestNumericTraits<TEST_EXECSPACE, long double, Epsilon>();
}

TEST(TEST_CATEGORY, numeric_traits_finite_min_max) {
  TestNumericTraits<TEST_EXECSPACE, char, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, signed char, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, unsigned char, FiniteMinMax>();

  TestNumericTraits<TEST_EXECSPACE, short, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, unsigned short, FiniteMinMax>();

  TestNumericTraits<TEST_EXECSPACE, int, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, unsigned int, FiniteMinMax>();

  TestNumericTraits<TEST_EXECSPACE, long, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, unsigned long, FiniteMinMax>();

  TestNumericTraits<TEST_EXECSPACE, long long, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, unsigned long long, FiniteMinMax>();

  TestNumericTraits<TEST_EXECSPACE, float, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, double, FiniteMinMax>();
  TestNumericTraits<TEST_EXECSPACE, long double, FiniteMinMax>();
}
