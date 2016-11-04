#ifndef VTLIB_INCLUDE_VTLIB_COLOR_H_
#define VTLIB_INCLUDE_VTLIB_COLOR_H_

#include <assert.h>
#include <stdint.h>

namespace vtlib {

// Represents a color in terminal output. Here, "color" means a color as
// indicated by some escape code (for each a foreground or background color); it
// is up to the renderer to decide how to map these to rendered colors. E.g.,
// this class does not indicate how the eight ANSI standard colors should be
// rendered. (For |Color::Type::RGB|, it is natural, but not required, to
// interpret them as, e.g., SRGB colors.)
//
// The color rendition may be affected by the attributes of the character being
// rendered. E.g., the foreground color may be rendered with greater intensity
// if the character has the bright/bold attribute (or the character may be
// rendered with a bold font).
//
// XTerm 88- and 256-color codes are either mapped to |Color::Type::ANSI_16|
// (for color indices 0..15) or directly to 24-bit RGB values
// (|Color::Type::RGB|). (Both the color codes consist of the extended ANSI
// codes, a small RGB cube, and range of grayscale values.)
class Color {
 public:
  enum class Type : uint8_t {
    // The 8 standard ANSI colors followed by high-intensity versions of them.
    ANSI_16,
    // Standard 24-bit RGB values, defined in ISO 8613-3 (reportedly):
    //   - foreground: ESC [38;2;{R};{G};{B}m
    //   - background: ESC [48;2;{R};{G};{B}m
    RGB,
  };

  explicit Color(Type type = Type::ANSI, uint32_t data = 0u) {
    set(type, data);
  }

  Type type() const {
    return (color_ & static_cast<uint32_t>(0xff000000u)) >> 24u;
  }

  // Color data is:
  //   - |Type::ANSI_16|: an index 0..255, encoded as follows:
  //       - 0..7: the standard ANSI colors (black, red, green, yellow, blue,
  //         magenta, cyan, white, respectively)
  //       - 8..15: corresponding to the high-intensity versions of the ANSI
  //         colors
  //   - |Type::RGB|: encoding a 256x256x256 RGB cube (0xrrbbgg, where rr, gg,
  //     bb are 0x00..0xff)
  uint32_t data() const {
    return color_ & static_cast<uint32_t>(0x00ffffffu);
  }

  void set(Type type, uint32_t data) {
    assert(!(data & ~static_cast<uint32_t>(0x00ffffffu)));
    color_ = (static_cast<uint32_t>(type) << 24u) | data;
  }

 private:
  // |Type| is stored in the high-order byte of |color_|. The three low-order
  // bytes store type-dependent color data (see |data()| above).
  uint32_t color_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_COLOR_H_
