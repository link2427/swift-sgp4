/*
 MIT License
 
 Copyright (c) 2025 Calogero Sanfilippo
 
 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.
 
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
 */

#include "Wrapper.hpp"
#include <CoordGeodetic.h>
#include <DateTime.h>
#include <Eci.h>
#include <SGP4.h>
#include <SatelliteException.h>
#include <Tle.h>
#include <TleException.h>
#include <exception>

namespace {
SGP4SatelliteResult failedResult(int32_t errorCode) {
    return {
        false,
        errorCode,
        0,
        0,
        0,
        0
    };
}
}

SGP4SatelliteResult sgp4SatelliteData(
    const char *title,
    const char *firstLine,
    const char *secondLine,
    int32_t year,
    int32_t month,
    int32_t day,
    int32_t hour,
    int32_t minute,
    int32_t second,
    int32_t microsecond
) {
    try {
        libsgp4::Tle tle(
            title == nullptr ? "" : title,
            firstLine == nullptr ? "" : firstLine,
            secondLine == nullptr ? "" : secondLine
        );
        libsgp4::SGP4 sgp4(tle);
        libsgp4::DateTime dateTime(year, month, day, hour, minute, second);
        dateTime.Initialise(year, month, day, hour, minute, second, microsecond);

        libsgp4::Eci eci = sgp4.FindPosition(dateTime);
        libsgp4::CoordGeodetic geo = eci.ToGeodetic();
        double velocity = eci.Velocity().Magnitude() * 3600.0;

        return {
            true,
            0,
            geo.latitude * 180.0 / M_PI,
            geo.longitude * 180.0 / M_PI,
            velocity,
            geo.altitude
        };
    } catch (libsgp4::TleException&) {
        return failedResult(1);
    } catch (libsgp4::SatelliteException&) {
        return failedResult(2);
    } catch (std::exception&) {
        return failedResult(3);
    }
}
