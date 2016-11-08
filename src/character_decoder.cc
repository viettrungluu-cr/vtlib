#include <vtlib/character_decoder.h>

namespace vtlib {

// static
std::unique_ptr<CharacterDecoder> CharacterDecoder::Create(
    CharacterEncoding character_encoding) {
  switch (character_encoding) {
    case CharacterEncoding::ASCII:
      // TODO(vtl)
      break;
    case CharacterEncoding::UTF_8:
      // TODO(vtl)
      break;
  }
  return nullptr;
}

}  // namespace vtlib
