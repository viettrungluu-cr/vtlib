#include "src/ascii_character_decoder.h"

namespace vtlib {

AsciiCharacterDecoder::AsciiCharacterDecoder() {}
AsciiCharacterDecoder::~AsciiCharacterDecoder() {}

bool AsciiCharacterDecoder::ProcessByte(uint8_t input_byte,
                                        size_t* num_output_tokens,
                                        Token* output_tokens) {
  if (is_C0_control_code(input_byte) || input_byte >= 128u) {
    *num_output_tokens = 0u;
    return false;
  }
  output_tokens[0] = static_cast<Token>(input_byte);
  *num_output_tokens = 1u;
  return true;
}

}  // namespace vtlib
