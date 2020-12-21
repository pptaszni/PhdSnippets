/**
 * /author Pawel Ptasznik
 */

#include "ROI.hpp"

bool operator==(const ROI& left, const ROI& right)
{
  return (left.upperLeftX == right.upperLeftX) &&
    (left.upperLeftY == right.upperLeftY) &&
    (left.bottomRightX == right.bottomRightX) &&
    (left.bottomRightY == right.bottomRightY);
}

bool operator!=(const ROI& left, const ROI& right)
{
  return !(left == right);
}

std::ostream& operator<<(std::ostream& stream, const ROI& roi)
{
  stream << "[ " << roi.upperLeftX << ", " << roi.upperLeftY
    << ", " << roi.bottomRightX << ", " << roi.bottomRightY << " ]";
    return stream;
}

QDebug& operator<<(QDebug& stream, const ROI& roi)
{
  stream << "[ " << roi.upperLeftX << ", " << roi.upperLeftY
    << ", " << roi.bottomRightX << ", " << roi.bottomRightY << " ]";
    return stream;
}
