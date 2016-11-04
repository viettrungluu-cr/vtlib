#ifndef VTLIB_INCLUDE_VTLIB_CHARACTER_ENCODING_H_
#define VTLIB_INCLUDE_VTLIB_CHARACTER_ENCODING_H_

#include <stdint.h>

namespace vtlib {

enum class CharacterEncoding : uint8_t {
  ASCII = 0,
  UTF8 = 1,
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_CHARACTER_ENCODING_H_
