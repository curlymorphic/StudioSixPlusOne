/*
 * Copyright (c) 2019 Dave French <contact/dot/dave/dot/french3/at/googlemail/dot/com>
 *
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program (see COPYING); if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 *
 */

#pragma once

#include <memory>
#include <cmath>

template<typename T>
inline T linearInterpolate (const T v0, const T v1, const T frac) noexcept
{
    return frac * (v1 - v0) + v0;
}

template <typename T>
class CircularBuffer
{
public:
    CircularBuffer() 
    {
        reset (4096);
    }
    CircularBuffer(const unsigned int minBufferSize)
    {
        reset (minBufferSize);
    }

    void reset(const unsigned int minBufferSize)
    {
        writeIndex = 0;
        bufferLength = static_cast<unsigned int>(std::pow(2, std::ceil(std::log(minBufferSize) / std::log(2))));
        wrapBits = bufferLength - 1;
        buffer.reset (new T[bufferLength]);
        clear();
    }

    void clear() 
    {
        for (auto i = 0; i < static_cast<int> (bufferLength); ++i)
        {
            buffer[i] = 0;
        }
    }

    void writeBuffer(const T newValue) 
    {
        buffer[writeIndex] = std::isnan (newValue) || std::isinf (newValue) ? 0 : newValue;
        writeIndex++;
        writeIndex &= wrapBits;
    }

    T readBuffer(const int delaySamples) const 
    {
        return buffer[(writeIndex - delaySamples) & wrapBits];
    }

    T readBuffer(const float delaySamples, const bool interpolate = true) const
    {
        auto y1 = readBuffer(static_cast<int> (delaySamples));
        if (!interpolate)
        {
            return y1;
        }
        auto y2 = readBuffer(static_cast<int> (delaySamples) + 1);
        auto fract = delaySamples - static_cast<int> (delaySamples);
        return linearInterpolate (y1, y2, fract);
    }

private:
    std::unique_ptr<T[]> buffer{nullptr};
    unsigned int writeIndex{0};
    unsigned int bufferLength{0};
    unsigned int wrapBits{0};
};
