#include "src/ascii_character_decoder.h"

#include <vtlib/codepoint.h>

namespace vtlib {

AsciiCharacterDecoder::AsciiCharacterDecoder() = default;

AsciiCharacterDecoder::~AsciiCharacterDecoder() = default;

bool AsciiCharacterDecoder::Supports8bitC1() const {
  return true;
}

void AsciiCharacterDecoder::ProcessByte(uint8_t input_byte,
                                        CodepointVector* output_codepoints) {
  output_codepoints->push_back((input_byte <= 0x7f)
                                   ? static_cast<Codepoint>(input_byte)
                                   : CODEPOINT_REPLACEMENT);
}

void AsciiCharacterDecoder::Flush(CodepointVector* output_codepoints) {
  // This decoder is stateless.
}

}  // namespace vtlib
