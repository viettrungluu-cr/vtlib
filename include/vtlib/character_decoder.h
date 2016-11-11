#ifndef VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_
#define VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include <vtlib/character_encoding.h>
#include <vtlib/token.h>

namespace vtlib {

constexpr size_t kMaxOutputTokensPerInputByte = 4u;

class CharacterDecoder {
 public:
  virtual ~CharacterDecoder() = default;

  // Static factory method. Returns null if a character encoding is not
  // supported.
  static std::unique_ptr<CharacterDecoder> Create(
      CharacterEncoding character_encoding);

  // |input_byte| is the input byte to be processed. |output tokens| should
  // point to a buffer with space for at least |kMaxOutputTokensPerInputByte|
  // tokens.
  //
  // Returns true if the byte was "accepted" or false if it was not, and should
  // possibly be processed as (e.g.) a control code. |*num_output_tokens| will
  // be set to the number of output tokens produced, and |output_tokens[0]|,
  // ..., |output_tokens[*num_output_tokens-1]| will be set to those output
  // tokens.
  //
  // Note that even if the byte was not accepted (i.e., the return value is
  // false), output tokens may still be produced. E.g., if the decoder was in
  // the middle of a multibyte sequence (e.g., a multibyte UTF-8 encoding) when
  // a control code is encountered, it may be desirable to produce output tokens
  // (e.g., producing a Unicode replacement character U+FFFD for each byte
  // already seen).
  //
  // This method *must* not accept input bytes in the range 0..31 (i.e., C0
  // control codes), except possibly when there was previous input (i.e., as a
  // non-first byte in a multibyte sequence). This *may* accept input bytes in
  // the range 128..159 (i.e., C1 control codes).
  virtual bool ProcessByte(uint8_t input_byte,
                           size_t* num_output_tokens,
                           Token* output_tokens) = 0;

  // Static helper functions to determine whether a given byte is a control
  // code.
  static bool is_C0_control_code(uint8_t b) { return b <= 31u; }
  static bool is_C1_control_code(uint8_t b) { return b >= 128u && b <= 159u; }
  static bool is_control_code(uint8_t b) {
    return is_C0_control_code(b) || is_C1_control_code(b);
  }

 protected:
  CharacterDecoder() {}
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_
