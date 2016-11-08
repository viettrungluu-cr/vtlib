#include <vtlib/character_decoder.h>

#include "src/ascii_character_decoder.h"

namespace vtlib {

// static
std::unique_ptr<CharacterDecoder> CharacterDecoder::Create(
    CharacterEncoding character_encoding) {
  switch (character_encoding) {
    case CharacterEncoding::ASCII:
      // TODO(C++14): Here and below, no make_unique in C++11. :(
      return std::unique_ptr<CharacterDecoder>(new AsciiCharacterDecoder());
    case CharacterEncoding::UTF_8:
      // TODO(vtl)
      break;
  }
  return nullptr;
}

}  // namespace vtlib
