/**
 * /author Pawel Ptasznik
 */

#pragma once

#include <QDebug>

#include <iostream>

struct ROI
{
  float upperLeftX;
  float upperLeftY;
  float bottomRightX;
  float bottomRightY;
};
bool operator==(const ROI& left, const ROI& right);
bool operator!=(const ROI& left, const ROI& right);
std::ostream& operator<<(std::ostream& stream, const ROI& roi);
QDebug& operator<<(QDebug& os, const ROI& roi);
