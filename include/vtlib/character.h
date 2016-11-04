#ifndef VTLIB_INCLUDE_VTLIB_CHARACTER_H_
#define VTLIB_INCLUDE_VTLIB_CHARACTER_H_

#include <assert.h>
#include <stdint.h>

namespace vtlib {

//FIXME
class Character {
 public:
  // The Unicode codepoint is 21-bit (though note that only part of that range
  // is used), leaving us 11 bits for attribute flags. Things we currently don't
  // support: rapid blink (SGR 6), fonts (SGR 10-19), fraktur (SGR 20), framed
  // (SGR 51), encircled (SGR 52), overlined (SGR 53).
  enum class Attribute : uint32_t {
    NONE = 0u,
    BOLD = 1u << 0u,              // (SGR 1)
    FAINT = 1u << 1u,             // (SGR 2)
    ITALICIZED = 1u << 2u,        // (SGR 3)
    UNDERLINED = 1u << 3u,        // (SGR 4)
    BLINK = 1u << 4u,             // (SGR 5)
    INVERSE = 1u << 5u,           // a.k.a. reverse video (SGR 7)
    INVISIBLE = 1u << 6u,         // a.k.a. concealed, hidden (SGR 8)
    CROSSED_OUT = 1u << 7u,       // (SGR 9)
    DOUBLY_UNDERLINE = 1u << 8u,  // (SGR 21)
    RESERVED_1 = 1u << 9u,
    RESERVED_2 = 1u << 10u,
  };
  Attribute operator|(Attribute a, Attribute b) {
    return static_cast<Attribute>(static_cast<uint32_t>(a) |
                                  static_cast<uint32_t>(b));
  }
  Attribute operator&(Attribute a, Attribute b) {
    return static_cast<Attribute>(static_cast<uint32_t>(a) &
                                  static_cast<uint32_t>(b));
  }
  bool operator!(Attribute a) { return !static_cast<uint32_t>(a); }

  explicit Character(Attribute attribute = Attribute::NONE,
                     uint32_t codepoint = 0u) {
    set(attribute, codepoint);
  }

  Attribute attribute() const {
    return (character_ & static_cast<uint32_t>(0xffe00000u)) >> 21u;
  }

  // Returns the Unicode codepoint (from 0..0x10ffff) for the character.
  uint32_t codepoint() const {
    return character_ & static_cast<uint32_t>(0x001fffffu);
  }

  void set_attribute(Attribute attribute) {
    assert(!(static_cast<uint32_t>(attribute) &
             ~static_cast<uint32_t>(0x7ffu)));
    character_ = (character_ & static_cast<uint32_t>(0x001fffffu)) |
                 (static_cast<uint32_t>(attribute) << 21u);
  }

  void set_codepoint(uint32_t codepoint) {
    assert(codepoint <= 0x10ffffu);
    character_ = (character_ & ~static_cast<uint32_t>(0x001fffffu)) | codepoint;
  }

  void set(Attribute attribute, uint32_t codepoint) {
    assert(!(static_cast<uint32_t>(attribute) &
             ~static_cast<uint32_t>(0x7ffu)));
    assert(codepoint <= 0x10ffffu);
    character_ = (static_cast<uint32_t>(attribute) << 21u) | codepoint;
  }

 private:
  uint32_t character_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_CHARACTER_H_
