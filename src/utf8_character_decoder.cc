#include "src/utf8_character_decoder.h"

#include <assert.h>

#include <vtlib/codepoint.h>

// Notes:
//
// 1 byte:  0xxxxxxx                            - 7 bits
//                                                U+0 to U+7F
// 2 bytes: 110xxxxx 10xxxxxx                   - 6+5 = 11 bits
//                                                U+80 to U+7FF
// 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx          - 4+6+6 = 16 bits
//                                                U+800 to U+D7FF,
//                                                U+E000 to U+FFFF
// 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx - 3+6+6+6 = 21 bits
//                                                U+10000 to U+10FFFF
//
// Overlong encodings are not allowed. This means:
//   - 2-byte encodings: 1100000x are not valid first bytes.
//   - 3-byte encodings: All bytes are valid first bytes, but if the first byte
//     is 11100000, then 100xxxxx are not valid second bytes.
//   - 4-byte encodings: All bytes are valid first bytes (but not really -- see
//     below), but if the first byte is 11110000, then 1000xxxx are not valid
//     second bytes.
//
// Invalid codepoints:
//   - U+D800 (0b1101100000000000) to U+DFFF (0b1101111111111111) are not valid.
//     Thus for 3-byte encodings, if the first byte is 11101101, then 101xxxxx
//     are not valid second bytes. (Checking this suffices.)
//   - Codepoints greater than U+10FFFF (0b100001111111111111111) are not valid.
//     Thus for 4-byte encodings, 11110101, 11110110, and 11110111 are not valid
//     first bytes. If the first byte is 11110100, then 1001xxxx, 1010xxxx, and
//     1011xxxx are not valid second bytes. (Checking these suffice.)

namespace vtlib {
namespace {

// Returns 0 if this is a continuation byte in a UTF-8 encoding, the total
// number of bytes (1 to 4) in the encoding if it is a valid leading byte in the
// encoding, or "-1" otherwise (i.e., if it is 0b11111xxx).
size_t IsLeadingByte(uint8_t b) {
  if ((b & 0xc0u) == 0x80u)  // 0b10xxxxxx: continuation.
    return 0u;
  if ((b & 0x80u) == 0x00u)  // 0b0xxxxxxx: 1 byte.
    return 1u;
  if ((b & 0xe0u) == 0xc0u)  // 0b110xxxxx: 2 byte.
    return 2u;
  if ((b & 0xf0u) == 0xe0u)  // 0b1110xxxx: 3 byte.
    return 3u;
  if ((b & 0xf8u) == 0xf0u)  // 0b11110xxx: 4 byte.
    return 4u;
  return static_cast<size_t>(-1);
}

}  // namespace

void Utf8CharacterDecoder::ProcessByte(uint8_t input_byte,
                                       CodepointVector* output_codepoints) {
  // Regardless, if we see a leading byte, we resynchronize.
  if (size_t n = IsLeadingByte(input_byte))
    ProcessLeadingByte(n, input_byte, output_codepoints);
  else
    ProcessContinuationByte(input_byte, output_codepoints);
}

void Utf8CharacterDecoder::Flush(CodepointVector* output_codepoints) {
  // Output replacement characters for any buffered bytes.
  for (size_t i = 0u; i < num_have_; i++)
    output_codepoints->push_back(CODEPOINT_REPLACEMENT);

  num_needed_ = 0u;
  num_have_ = 0u;
}

void Utf8CharacterDecoder::ProcessLeadingByte(
    size_t n,
    uint8_t input_byte,
    CodepointVector* output_codepoints) {
  // If we see a leading byte, we resynchronize.
  Flush(output_codepoints);

  switch (n) {
    case 1u:
      output_codepoints->push_back(static_cast<Codepoint>(input_byte));
      break;
    case 2u: {
      uint8_t data = input_byte & 0x1fu;
      if (data < 0x02u) {  // Overlong encoding.
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
      } else {
        num_needed_ = 2u;
        num_have_ = 1u;
        current_value_ = static_cast<Codepoint>(data) << 6;
      }
      break;
    }
    case 3u:
      num_needed_ = 3u;
      num_have_ = 1u;
      current_value_ = static_cast<Codepoint>(input_byte & 0x0fu) << 12;
      break;
    case 4u: {
      uint8_t data = input_byte & 0x07u;
      if (data >= 0x05u) {  // Invalid codepoint (greater than U+10FFFF).
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
      } else {
        num_needed_ = 4u;
        num_have_ = 1u;
        current_value_ = static_cast<Codepoint>(data) << 18;
      }
      break;
    }
    case static_cast<size_t>(-1):  // Invalid byte.
      output_codepoints->push_back(CODEPOINT_REPLACEMENT);
      break;
  }
}

void Utf8CharacterDecoder::ProcessContinuationByte(
    uint8_t input_byte,
    CodepointVector* output_codepoints) {
  uint8_t data = input_byte & 0x3fu;
  switch (num_needed_) {
    case 0u:  // Unexpected continuation.
      output_codepoints->push_back(CODEPOINT_REPLACEMENT);
      return;
    case 2u:
      break;
    case 3u:
      // Overlong encoding or invalid codepoint (U+D800 to U+DFFF).
      if (num_have_ == 1u && ((!current_value_ && data < 0x20u) ||
                              (current_value_ == 0xd000u && data >= 0x20u))) {
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
        num_needed_ = 0u;
        num_have_ = 0u;
        return;
      }
      break;
    case 4u:
      // Overlong encoding or invalid codepoint (greater than U+10FFFF).
      if (num_have_ == 1u && ((!current_value_ && data < 0x10u) ||
                              (current_value_ >= 0x100000 && data >= 0x10u))) {
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
        output_codepoints->push_back(CODEPOINT_REPLACEMENT);
        num_needed_ = 0u;
        num_have_ = 0u;
        return;
      }
      break;
    default:
      assert(false);
      return;
  }

  assert(num_have_ < num_needed_);
  num_have_++;
  current_value_ |= static_cast<Codepoint>(data)
                    << ((num_needed_ - num_have_) * 6);

  if (num_have_ == num_needed_) {
    assert(!(current_value_ >= 0xd800 && current_value_ <= 0xdfff));
    assert(current_value_ <= 0x10ffff);
    output_codepoints->push_back(current_value_);
    num_needed_ = 0u;
    num_have_ = 0u;
  }
}

}  // namespace vtlib
