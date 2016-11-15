#ifndef VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_
#define VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include <vtlib/character_encoding.h>
#include <vtlib/codepoint.h>

namespace vtlib {

class CharacterDecoder {
 public:
  virtual ~CharacterDecoder() = default;

  // Static factory method. Returns null if a character encoding is not
  // supported.
  static std::unique_ptr<CharacterDecoder> Create(
      CharacterEncoding character_encoding);

  // Returns true if the character encoding that this |CharacterDecoder| decodes
  // can support 8-bit C1 control characters (i.e., those in the range
  // 128..159). A consequence of supporting them is that these bytes may not be
  // used in the encoding of any characters (other than the C1 control codes).
  virtual bool Supports8bitC1() const = 0;

  // |input_byte| is the input byte to be processed. Output (Unicode) codepoints
  // are *appended* to |*output_codepoints|.
  //
  // |input_byte| will typically never be a C0 control code (byte in the range
  // 0..31); consequently, no encoding may use the bytes 0..31 (except to
  // represent C0 control codes). If |Supports8bitC1()| returns true, it will
  // also typically never be an 8-bit C1 control code (byte in the range
  // 128..159); consequently, no encoding supporting 8-bit C1 control codes may
  // use the bytes 128..159 (except to represent C1 control codes).
  //
  // The output codepoints must never include codepoints in these ranges, 0..31
  // and 128..159.
  virtual void ProcessByte(uint8_t input_byte,
                           CodepointVector* output_codepoints) = 0;

  // Resets the state of the character decoder, appending any resulting
  // codepoints to |*output_codepoints| (e.g., if the decoder was in the middle
  // of a multibyte encoding, it may wish to output replacement characters for
  // the already-seen bytes).
  virtual void Flush(CodepointVector* output_codepoints) = 0;

  // Static helper functions:
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
