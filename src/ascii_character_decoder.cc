#include "src/ascii_character_decoder.h"

namespace vtlib {

AsciiCharacterDecoder::AsciiCharacterDecoder() = default;

AsciiCharacterDecoder::~AsciiCharacterDecoder() = default;

bool AsciiCharacterDecoder::ProcessByte(uint8_t input_byte,
                                        TokenVector* output_tokens) {
  if (is_C0_control_code(input_byte) || input_byte >= 128u)
    return false;
  output_tokens->push_back(static_cast<Token>(input_byte));
  return true;
}

}  // namespace vtlib
