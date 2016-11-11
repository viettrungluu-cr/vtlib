#ifndef VTLIB_INCLUDE_VTLIB_TOKENIZER_H_
#define VTLIB_INCLUDE_VTLIB_TOKENIZER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include <vtlib/character_decoder.h>
#include <vtlib/character_encoding.h>
#include <vtlib/token.h>

namespace vtlib {

// TODO(vtl): Move this somewhere else.
// Input processing happens in several stages:
//
// 1. Character encoding and control code processing (done by |Tokenizer|): The
// initial input is a stream of bytes; the output is a stream of *tokens* that
// are either (graphical) characters (as Unicode codepoints) or "C0 and C1"
// control codes. This stage may map multiple (consecutive) input bytes to a
// single token. (A single input byte may also result in multiple output tokens,
// e.g., if it changes the interpretation of previously-received bytes.)
//
// C0 control codes are bytes in the range 0..31. 8-bit C1 control codes are
// bytes in the range 128..159. There is also a two-byte sequence for each C1
// control code of the form ESC c, where ESC = 27 and c is in the range 64..95,
// with ESC c equivalent to the 8-bit C1 code c+64.
//
// The first step (in general) is to pass an input byte to a |CharacterDecoder|
// (if the previous input byte was not ESC). The |CharacterDecoder| may then
// output tokens (typically for graphical characters), and indicates whether it
// "accepted" (consumed) the character or not. If it did not, then the character
// is processed as possibly being a C0 control code or (if enabled) an 8-bit C1
// control code.
//
// If the previous byte was ESC and if the current byte is in the range
// 128..159, then we emit a token for a C1 code as indicated above. Otherwise,
// we emit a token for ESC and start processing anew. How the resulting output
// is processed is a problem for a different layer. However, this does have a
// consequence: non-C1 escape sequences will be processed after passing through
// the character decoder, so are of the form ESC t where t is a token (possibly
// a Unicode codepoint) instead of ESC c for some ASCII byte c. This is largely
// not a problem since most (TODO(vtl): all supported?) character encodings
// encode ASCII codepoints in the trivial way, but it would be potentially
// problematic if this were not the case (or if there are alternate encodings
// for the ASCII codepoints).
//
// TODO(vtl): Moar.

// This class turns a sequence of input bytes into a sequence of |Token|s (each
// of which is either a Unicode codepoint or a control code).
class Tokenizer {
 public:
  // Constructs a |Tokenizer| with the given |CharacterDecoder|.
  Tokenizer(bool accept_8bit_C1,
            std::unique_ptr<CharacterDecoder> character_decoder);
  // Constructs a |Tokenizer| to handle the given |CharacterEncoding|.
  Tokenizer(bool accept_8bit_C1, CharacterEncoding character_encoding);
  ~Tokenizer();

  Tokenizer(const Tokenizer&) = delete;
  Tokenizer& operator=(const Tokenizer&) = delete;

  // Processes the input byte |input_byte|; |output_tokens| should point to a
  // buffer with space for at least |kMaxOutputTokensPerInputByte| tokens.
  // |*num_output_tokens| will be set to number of output tokens produced and
  // provided in |output_tokens[...]|.
  void ProcessByte(uint8_t input_byte,
                   size_t* num_output_tokens,
                   Token* output_tokens);

  bool accept_8bit_C1() const { return accept_8bit_C1_; }
  void set_accept_8bit_C1(bool accept_8bit_C1) {
    accept_8bit_C1_ = accept_8bit_C1;
  }

  CharacterDecoder* character_decoder() const {
    return character_decoder_.get();
  }
  // Returns the previous |CharacterDecoder|.
  std::unique_ptr<CharacterDecoder> set_character_decoder(
      std::unique_ptr<CharacterDecoder> character_decoder);
  // Convience form of |set_character_decoder()| (discards the previous
  // |CharacterDecoder|).
  void set_character_encoding(CharacterEncoding character_encoding);

 private:
  bool accept_8bit_C1_;  // Are 8-bit C1 control codes enabled?
  std::unique_ptr<CharacterDecoder> character_decoder_;

  // Last character was ESC (processing ESC c -> C1 transformation):
  bool got_ESC_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TOKENIZER_H_
