#ifndef VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
#define VTLIB_SRC_ASCII_CHARACTER_DECODER_H_

#include <vtlib/character_decoder.h>

namespace vtlib {

// A trivial character decoder for (7-bit) ASCII (which also supports 8-bit C1
// control codes); it will replace other bytes (>= 160) with the Unicode
// replacement character, U+FFFD.
class AsciiCharacterDecoder : public CharacterDecoder {
 public:
  AsciiCharacterDecoder();
  ~AsciiCharacterDecoder() override;

  AsciiCharacterDecoder(const AsciiCharacterDecoder&) = delete;
  AsciiCharacterDecoder& operator=(const AsciiCharacterDecoder&) = delete;

  // |CharacterDecoder| implementation:
  bool Supports8bitC1() const override;
  void ProcessByte(uint8_t input_byte,
                   CodepointVector* output_codepoints) override;
  void Flush(CodepointVector* output_codepoints) override;
};

}  // namespace vtlib

#endif  // VTLIB_SRC_ASCII_CHARACTER_DECODER_H_
