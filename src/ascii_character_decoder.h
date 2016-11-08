#ifndef VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
#define VTLIB_SRC_ASCII_CHARACTER_DECODER_H_

#include <vtlib/character_decoder.h>

namespace vtlib {

// A trivial character decoder for (7-bit) ASCII.
class AsciiCharacterDecoder : public CharacterDecoder {
 public:
  AsciiCharacterDecoder();
  ~AsciiCharacterDecoder() override;

  AsciiCharacterDecoder(const AsciiCharacterDecoder&) = delete;
  AsciiCharacterDecoder& operator=(const AsciiCharacterDecoder&) = delete;

  bool ProcessByte(uint8_t input_byte,
                   size_t* num_output_tokens,
                   Token* output_tokens) override;
};

}  // namespace vtlib

#endif  // VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
