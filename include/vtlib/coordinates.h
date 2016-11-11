#ifndef VTLIB_INCLUDE_VTLIB_COORDINATES_H_
#define VTLIB_INCLUDE_VTLIB_COORDINATES_H_

#include <stdint.h>

namespace vtlib {

// Rows are assigned a unique (sequentially-increasing) row number. This is so
// that one can refer to rows in the scrollback sensibly (even after the screen
// has been updated and additional rows added), though it does mean that the row
// numbers for the current viewport are also always changing.
using RowNumber = uint64_t;

using ColumnNumber = uint32_t;

// Rectangles include the top and left, but not the bottom and right. I.e., a
// rectangle is the region |(row, column)| where |top <= row < bottom| and |left
// <= column < right|. (If |top >= bottom| or |left >= right|, then the
// rectangle is empty.)
struct Rectangle {
  Rectangle() = default;

  RowNumber top = 0u;
  RowNumber bottom = 0u;
  ColumnNumber left = 0u;
  ColumnNumber right = 0u;

  bool is_empty() const { return top >= bottom || left >= right; }
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_COORDINATES_H_
