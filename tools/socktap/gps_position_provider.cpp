#include "gps_position_provider.hpp"
#include <vanetza/units/angle.hpp>
#include <vanetza/units/velocity.hpp>
#include <cmath>

static_assert(GPSD_API_MAJOR_VERSION == 5 || GPSD_API_MAJOR_VERSION == 6, "libgps has incompatible API");

GpsPositionProvider::GpsPositionProvider() :
    GpsPositionProvider(gpsd::shared_memory, nullptr)
{
}

GpsPositionProvider::GpsPositionProvider(const std::string& hostname, const std::string& port)
{
    if (gps_open(hostname.c_str(), port.c_str(), &gps_data)) {
        throw gps_error(errno);
    }
    gps_stream(&gps_data, WATCH_ENABLE | WATCH_JSON, nullptr);
}

GpsPositionProvider::~GpsPositionProvider()
{
    gps_stream(&gps_data, WATCH_DISABLE, nullptr);
    gps_close(&gps_data);
}

GpsPositionProvider::gps_error::gps_error(int err) :
    std::runtime_error(gps_errstr(err))
{
}

const vanetza::facilities::PositionFix& GpsPositionProvider::last_position_fix()
{
    return position_fix;
}

void GpsPositionProvider::fetch_position_fix()
{
    if (gps_read(&gps_data) < 0) {
        throw gps_error(errno);
    }

    if (gps_data.status == STATUS_FIX && gps_data.fix.mode >= MODE_2D) {
        using namespace vanetza::units;
        static const TrueNorth north = TrueNorth::from_value(0.0);

        position_fix.timestamp = convert(gps_data.fix.time);
        position_fix.latitude = gps_data.fix.latitude * degree;
        position_fix.longitude = gps_data.fix.longitude * degree;
        position_fix.speed.assign(gps_data.fix.speed * si::meter_per_second, gps_data.fix.eps * si::meter_per_second);
        position_fix.course.assign(north + gps_data.fix.track * degree, north + gps_data.fix.epd * degree);
    }
}

vanetza::Clock::time_point GpsPositionProvider::convert(timestamp_t gpstime) const
{
    using namespace boost::gregorian;
    using namespace boost::posix_time;

    // gpsd's timestamp_t is UNIX time (UTC) with fractional seconds
    static date posix_epoch(1970, Jan, 1);
    timestamp_t gps_integral;
    timestamp_t gps_fractional = std::modf(gpstime, &gps_integral);
    auto posix_seconds = seconds(gps_integral);
    auto posix_milliseconds = milliseconds(gps_fractional * 1000.0);
    ptime posix_time { posix_epoch, posix_seconds + posix_milliseconds };

    // TAI has some seconds bias compared to UTC
    const auto tai_utc_bias = seconds(36); // 36 seconds since 1st July 2015
    return vanetza::Clock::at(posix_time + tai_utc_bias);
}
