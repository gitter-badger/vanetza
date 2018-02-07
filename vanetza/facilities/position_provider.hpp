#ifndef POSITION_PROVIDER_HPP_4CZDVALU
#define POSITION_PROVIDER_HPP_4CZDVALU

#include <vanetza/facilities/position_fix.hpp>

namespace vanetza
{
namespace facilities
{

/**
 * PositionProvider is a generic interface to retrieve positioning data.
 *
 * How the data has been gathered is not defined, i.e. it may come from an attached GNSS receiver or
 * something completely different.
 */
class PositionProvider
{
public:
    /**
     * Get current position fix
     * \return position with latitude and longitude
     */
    virtual const PositionFix& last_position_fix() = 0;
};

} // namespace facilities
} // namespace vanetza

#endif /* POSITION_PROVIDER_HPP_4CZDVALU */

