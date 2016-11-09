#ifndef VTLIB_SRC_UTF8_CHARACTER_DECODER_H_
#define VTLIB_SRC_UTF8_CHARACTER_DECODER_H_

#include <stddef.h>
#include <stdint.h>

#include <vtlib/character_decoder.h>

namespace vtlib {

// A character decoder for UTF-8.
//
// Invalid encodings/codepoints:
//
// If a *byte* is accepted but cannot decoded as a part of a valid codepoint,
// then a replacement token (by default, U+FFFD, the Unicode replacement
// character) will be emitted for it, and the state of the decoder reset.
//
// For sequences that may appear to be valid UTF-8 encodings, but do not
// actually encode a valid codepoint, this is done as soon as possible.
//
// For example, the following sequence of bytes encodes the invalid codepoint
// U+D800: 0xed, 0xa0, 0x80.
//   - After receiving 0xed, nothing is known -- it may be the start of an
//     encoding for a valid codepoint (e.g., U+D000).
//   - But upon receiving 0xa0, it is known that this can only encode an invalid
//     codepoint, in the range U+D000 to U+D7FF; at this point, we emit *two*
//     replacement tokens (one for each byte), and reset the decoder.
//   - On receiving 0x80, we emit another replacement token, since we are not
//     expecting a continuation byte.
//   - Thus in all, we emit three replacement tokens.
//   - This behavior is consistent-ish with Gnome Terminal
//   - However, Gnome Terminal only emits the three replacement characters after
//     receiving all three bytes.
//   - This behavior is inconsistent with XTerm and screen (both of which simply
//     emit one, after receiving all three bytes).
//
// Similarly, for the truncated sequence for an invalid codepoint 0xed, 0xa0,
// 'X':
//   - We emit two replacement tokens, followed by 'X'.
//   - The observed behavior is consistent with Gnome Terminal, except that
//     Gnome Terminal only does this after receiving the third byte.
//   - Again, XTerm and screen behave differently: they emit one replacement
//     character, followed by 'X' (only emitting anything after receiving the
//     third byte).
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
