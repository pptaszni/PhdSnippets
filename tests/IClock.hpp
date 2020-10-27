/**
 * @copyright Copyright (C) Avoplc - All Rights Reserved
 * Unauthorized copying, printing, storing or transmission of this file,
 * via any medium is strictly prohibited
 * Proprietary and confidential
 */

#pragma once

#include <chrono>

namespace lcs {
namespace scs {
namespace procedures {
namespace fastrampup {

class IClock
{
public:
    virtual ~IClock() = default;
    virtual std::chrono::time_point<std::chrono::steady_clock> now() = 0;
    virtual void sleepFor(std::chrono::milliseconds ms) = 0;
};

} // namespace fastrampup
} // namespace procedures
} // namespace scs
} // namespace lcs
