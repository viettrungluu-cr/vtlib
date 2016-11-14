#include "src/ascii_character_decoder.h"

#include <vtlib/codepoint.h>

namespace vtlib {

AsciiCharacterDecoder::AsciiCharacterDecoder() = default;

AsciiCharacterDecoder::~AsciiCharacterDecoder() = default;

void AsciiCharacterDecoder::ProcessByte(uint8_t input_byte,
                                        CodepointVector* output_codepoints) {
  output_codepoints->push_back((input_byte <= 159u)
                                   ? static_cast<Codepoint>(input_byte)
                                   : CODEPOINT_REPLACEMENT);
}

void AsciiCharacterDecoder::Flush(CodepointVector* output_codepoints) {
  // This decoder is stateless.
}

}  // namespace vtlib
