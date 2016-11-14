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

  // |input_byte| is the input byte to be processed. Output (Unicode) codepoints
  // are *appended* to |*output_codepoints|.
  //
  // Requirements for the processing of control bytes:
  //   - Bytes/codepoints 0..31 are the C0 control codes (by definition) and
  //     128..159 are the (8-bit) C1 control codes.
  //   - Control codes that are accepted must be turned into their corresponding
  //     codepoints immediately. I.e., if |input_byte| is an accepted control
  //     code, then the last codepoint appended to |*output_codepoints| must be
  //     the codepoint with the same value as |input_byte|.
  //   - When a control code is accepted, there must be no lingering state.
  //     (I.e., calling |Flush()| afterwards should be a no-op, and result in no
  //     further codepoints. Nor should any input before the control code have
  //     an effect on the output after the control code.)
  //   - Only control code bytes may be mapped to control code codepoints. I.e.,
  //     there must be no other encoding of the control code codepoints.
  //   - All C0 control codes must be accepted. Consequently, the bytes 0..31
  //     must not be used in the encoding of any other codepoint.
  //   - Either all of the (8-bit) C1 control codes must be accepted (in which
  //     case the byes 128..159 must not be used in the encoding of any other
  //     codepoint), or none must be (in which case there must be no way to
  //     encode the C1 codepoints).
  virtual void ProcessByte(uint8_t input_byte,
                           CodepointVector* output_codepoints) = 0;

  // Resets the state of the character decoder, appending any resulting
  // codepoints to |*output_codepoints| (e.g., if the decoder was in the middle
  // of a multibyte encoding, it may wish to output replacement characters for
  // the already-seen bytes).
  virtual void Flush(CodepointVector* output_codepoints) = 0;

 protected:
  CharacterDecoder() {}
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_CHARACTER_DECODER_H_
