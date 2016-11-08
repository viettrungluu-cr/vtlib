#ifndef VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
#define VTLIB_SRC_ASCII_CHARACTER_DECODER_H_

#include <vtlib/character_decoder.h>

namespace vtlib {

// A trivial character decoder for (7-bit) ASCII.
class AsciiCharacterDecoder : public CharacterDecoder {
 public:
  AsciiCharacterDecoder() {}
  ~AsciiCharacterDecoder() override {}

  AsciiCharacterDecoder(const AsciiCharacterDecoder&) = delete;
  AsciiCharacterDecoder& operator=(const AsciiCharacterDecoder&) = delete;

  bool ProcessByte(uint8_t input_byte,
                   size_t* num_output_tokens,
                   Token* output_tokens) override {
    if (is_C0_control_code(input_byte) || input_byte >= 128u) {
      *num_output_tokens = 0u;
      return false;
    }
    output_tokens[0] = static_cast<Token>(input_byte);
    *num_output_tokens = 1u;
    return true;
  }
};

}  // namespace vtlib

#endif  // VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
