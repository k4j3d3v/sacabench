/*******************************************************************************
 * Copyright (C) 2018 Christopher
 * Copyright (C) 2018 Marvin Böcker <marvin.boecker@tu-dortmund.de>
 *
 * All rights reserved. Published under the BSD-3 license in the LICENSE file.
 ******************************************************************************/

#pragma once

#include <util/assertions.hpp>
#include <util/bits.hpp>

namespace sacabench::util {

using uchar = unsigned char;

/// \brief represents a number type based on sa_index, but with special
/// operations for the first bits.
template <typename sa_index, uchar extra_bits>
class tagged_number {
private:
    sa_index memory;

    // Bitmask for 0011 1111 1111 1111 (if two extra bits are used)
    constexpr static sa_index MAX = static_cast<sa_index>(-1) >> extra_bits;

    template <uchar i>
    constexpr static sa_index BITMASK = static_cast<sa_index>(1) << (bits_of<sa_index> - 1 - i);

public:
    constexpr inline tagged_number() : memory(0) {
        DCHECK_LE(extra_bits, bits_of<sa_index>());
    }

    constexpr inline tagged_number(const sa_index m) : memory(m) {
        DCHECK_LE(extra_bits, bits_of<sa_index>);
    }

    constexpr inline sa_index number() const { return memory & MAX; }

    constexpr inline operator sa_index() const { return number(); }

    template <uchar i>
    constexpr inline bool get() const {
        DCHECK_LT(i, extra_bits);
        return memory & BITMASK<i>;
    }

    template <uchar i>
    constexpr inline void set(bool v) {
        // This is all ones.
        constexpr sa_index ones = static_cast<sa_index>(-1);

        // This is 0000100000 with a 1 only at the correct position.
        constexpr sa_index mask = BITMASK<i>;

        // This updates the memory to contain 0 at the position i.
        memory = memory & ~mask;

        // The right hand side is only a 1 at position 1.
        memory = memory | ((v * ones) & mask);
    }

    constexpr inline bool operator<(const tagged_number& rhs) const {
        return number() < rhs.number();
    }

    constexpr inline bool operator==(const tagged_number& rhs) const {
        return number() == rhs.number();
    }

    constexpr inline bool operator>(const tagged_number& rhs) const {
        return number() > rhs.number();
    }

    constexpr inline void operator++() {
        DCHECK_LT(memory, MAX);
        ++memory;
    }

    constexpr inline void operator--() {
        DCHECK_GT(memory, 0);
        --memory;
    }

    constexpr inline tagged_number operator+(const tagged_number& rhs) {
        return tagged_number(number() + rhs.number());
    }

    constexpr inline tagged_number operator-(const tagged_number& rhs) {
        DCHECK_GE(number(), rhs.number());
        return tagged_number(number() - rhs.number());
    }

    constexpr inline tagged_number operator*(const tagged_number& rhs) {
        return tagged_number(number() * rhs.number());
    }

    constexpr inline tagged_number operator/(const tagged_number& rhs) {
        return tagged_number(number() / rhs.number());
    }
};
} // namespace sacabench::util
