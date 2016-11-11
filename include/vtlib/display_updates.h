#ifndef VTLIB_INCLUDE_VTLIB_DISPLAY_UPDATES_H_
#define VTLIB_INCLUDE_VTLIB_DISPLAY_UPDATES_H_

#include <stdint.h>

#include <vtlib/coordinates.h>

namespace vtlib {

struct DisplayUpdates {
  DisplayUpdates() = default;

  // Number of times the bell should be "sounded".
  uint64_t bell_count = 0u;

  // Bounding rectangle for the area that has changed. (This may include areas
  // that are now outside the viewport, i.e., are "offscreen".)
  Rectangle dirty;

  bool needs_update() const { return bell_count || !dirty.is_empty(); }
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_DISPLAY_UPDATES_H_
