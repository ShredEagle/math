#pragma once

#include "Vector.h"

namespace ad {
namespace math {


template <class T_number>
struct Rectangle
{
    T_number & x()
    { return mPosition.x(); }
    T_number & y()
    { return mPosition.y(); }

    T_number x() const
    { return mPosition.x(); }
    T_number y() const
    { return mPosition.y(); }

    T_number & width()
    { return mDimension.width(); }
    T_number & height()
    { return mDimension.height(); }

    T_number width() const
    { return mDimension.width(); }
    T_number height() const
    { return mDimension.height(); }

    T_number area() const
    { return mDimension.area(); }

    Position<2, T_number> originCorner() const
    { return mPosition; }

    Position<2, T_number> diagonalCorner() const
    { return mPosition + static_cast<Vec<2, T_number>>(mDimension); }

    bool operator==(const Rectangle &aRhs) const
    { return mPosition == aRhs.mPosition && mDimension == aRhs.mDimension; }
    bool operator!=(const Rectangle &aRhs) const
    { return !(*this == aRhs); }

    template <class T_positionValue>
    bool contains(Position<2, T_positionValue> aPosition) const;

    Position<2, T_number>  mPosition;
    Size<2, T_number> mDimension;
};


template <class T_number>
template <class T_positionValue>
bool Rectangle<T_number>::contains(Position<2, T_positionValue> aPosition) const
{
    return (aPosition.x() >= mPosition.x())
        && (aPosition.y() >= mPosition.y())
        && (aPosition.x() <= mPosition.x()+mDimension.width())
        && (aPosition.y() <= mPosition.y()+mDimension.height());
}

}} // namespace ad::math
