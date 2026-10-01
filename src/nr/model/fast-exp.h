// Copyright (c) 2024 Leonid Moroz (moroz_lv@lp.edu.ua)
//
// SPDX-License-Identifier: MIT

#ifndef FAST_EXP_H
#define FAST_EXP_H

#include <iomanip>

/*
These functions return an approximation of exp(x) with a relative error <0.173%.
They are several times faster than std::exp().

The code assumes that values of type double are stored in the IEEE754
double precision floating point formats.

References:

Leonid Moroz, Volodymyr Samotyy, Zbigniew Kokosiński, Paweł Gepner.
“Simple multiple precision algorithms for exponential functions”,
IEEE Signal Processing Magazine 39(4):130 - 137 (2022).
(available at https://ieeexplore.ieee.org/document/9810030)
*/

inline double
exp21d(double x)
{
    if (x < -708.0)
    {
        return 0;
    }
    if (x > 709.0)
    {
        return std::numeric_limits<double>::infinity();
    }
    int64_t z = x * 0x00171547652B82FE + 0x3FF0000000000000;

    union {
        int64_t i;
        double f;
    } zii;

    zii.i = z & 0xfff0000000000000;
    int64_t zif = z & 0x000fffffffffffff;
    double d1 = 7.4871095977966e-17;
    double d2 = 8771752971182036 + zif;
    double d3 = 11827349474026 + zif;
    d2 = d1 * d2;
    zif = d2 * d3;
    zii.i |= zif;
    double y = zii.f;
    return y;
}

[[maybe_unused]] bool
testExp21dPower(double power)
{
    double ref = std::exp(power);
    double fast = exp21d(power);
    double absError = std::abs(fast - ref);
    double relError = absError / ref;
    if ((ref < 0 || ref > 0) && relError > 0.00173 && power > -708)
    {
        std::cout << std::setw(12) << power << "\t" << std::setw(8) << ref << "\t" << std::setw(8)
                  << fast << "\t" << std::setw(8) << absError << "\t" << std::setw(8) << relError
                  << std::endl;
        return false;
    }
    return true;
}

[[maybe_unused]] void
testExp21d()
{
    bool pass = true;
    std::cout << "x"
              << "\t\tref"
              << "\t\tfast"
              << "\t\tabsErr"
              << "\t\trelErr" << std::setprecision(3) << std::endl;
    // Tricky numbers from LLVM
    // https://github.com/llvm/llvm-project/blob/main/libc/test/src/math/exp_test.cpp
    std::vector<uint64_t> llvmTrickyNumbers = {0x3FD79289C6E6A5C0,
                                               0x3FD05DE80A173EA0,
                                               0xbf1eb7a4cb841fcc,
                                               0xbf19a61fb925970d,
                                               0x3fda7b764e2cf47a,
                                               0xc04757852a4b93aa,
                                               0x4044c19e5712e377,
                                               0xbf19a61fb925970d,
                                               0xc039a74cdab36c28,
                                               0xc085b3e4e2e3bba9,
                                               0xc086960d591aec34,
                                               0xc086232c09d58d91,
                                               0xc0874910d52d3051,
                                               0xc0867a172ceb0990};
    // Tricky numbers
    for (auto num : llvmTrickyNumbers)
    {
        union {
            uint64_t i;
            double d;
        } temp;

        temp.i = num;
        pass &= testExp21dPower(temp.d);
    }

    // Integer exponents
    for (int i = -1024; i < 1024; i++)
    {
        pass &= testExp21dPower(i);
    }

    // Negative and positive fractions
    for (int signal = -1; signal < 2; signal += 2)
    {
        for (int64_t i = 1; i < 1000000000000; i *= 10)
        {
            double power = signal * 1.0 / i;
            pass &= testExp21dPower(power);
        }
    }
    std::cout << (pass ? "PASS" : "FAIL") << std::endl;
    exit(!pass);
}

#endif
