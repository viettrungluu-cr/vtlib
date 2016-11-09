#ifndef VTLIB_SRC_UTF8_CHARACTER_DECODER_H_
#define VTLIB_SRC_UTF8_CHARACTER_DECODER_H_

#include <stddef.h>
#include <stdint.h>

#include <vtlib/character_decoder.h>

namespace vtlib {

// A character decoder for UTF-8.
class Utf8CharacterDecoder : public CharacterDecoder {
 public:
  // The default replacement token is just the Unicode replacement character
  // U+FFFD.
  explicit Utf8CharacterDecoder(Token replacement_token = 0xfffd);
  ~Utf8CharacterDecoder() override;

  Utf8CharacterDecoder(const Utf8CharacterDecoder&) = delete;
  Utf8CharacterDecoder& operator=(const Utf8CharacterDecoder&) = delete;

  bool ProcessByte(uint8_t input_byte,
                   size_t* num_output_tokens,
                   Token* output_tokens) override;

  Token replacement_token() const { return replacement_token_; }

 private:
  // Helpers for |ProcessByte()|:
  bool ProcessLeadingByte(size_t n,
                          uint8_t input_byte,
                          size_t* num_output_tokens,
                          Token* output_tokens);
  bool ProcessContinuationByte(uint8_t input_byte,
                               size_t* num_output_tokens,
                               Token* output_tokens);

  const Token replacement_token_;

  // Number of bytes needed for the current encoded codepoint (valid values: 0,
  // 2, 3, 4). UTF-8 uses most 4 bytes per codepoint encoding and 1 is not
  // needed (since the byte can be processed immediately).
  size_t num_needed_ = 0u;

  // Number of bytes of data seen for the current encoded codepoint. Invariant:
  // |num_have_ < num_needed_|.
  size_t num_have_ = 0u;

  // The value of the current codepoint, as far as we've seen (only meaningful
  // if |num_needed_| is nonzero).
  Token current_value_ = 0;
};

}  // namespace vtlib

#endif  // VTLIB_SRC_UTF8_CHARACTER_DECODER_H_
