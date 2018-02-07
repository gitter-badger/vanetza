#ifndef SIMPLE_POSITION_PROVIDER_HPP_12MUJV0K
#define SIMPLE_POSITION_PROVIDER_HPP_12MUJV0K

#include <vanetza/facilities/position_provider.hpp>

namespace vanetza
{
namespace facilities
{

/**
 * SimplePositionProvider is probably the simplest but still useful PositionProvider implementation.
 */
class SimplePositionProvider : public PositionProvider
{
public:
    const PositionFix& last_position_fix() override
    {
        return m_position;
    }

    void position_fix(const PositionFix& pos)
    {
        m_position = pos;
    }

private:
    PositionFix m_position;
};

} // namespace facilities
} // namespace vanetza

#endif /* SIMPLE_POSITION_PROVIDER_HPP_12MUJV0K */

