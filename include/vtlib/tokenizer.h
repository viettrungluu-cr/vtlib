#ifndef VTLIB_INCLUDE_VTLIB_TOKENIZER_H_
#define VTLIB_INCLUDE_VTLIB_TOKENIZER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>

#include <vtlib/character_decoder.h>
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
// Invalid escape sequence processing: If we encounter ESC c for some c outside
// the range 64..95, we simply forget about the ESC and process c as if it were
// a new character. E.g., if we encounter ESC c1 c2 c3 ... where c1 c2 is a
// valid two-byte UTF-8 encoding, we will decode c1 c2 c3 .... As far as I can
// tell, this is like screen, unlike XTerm (which will also drop the next
// graphical character, i.e., c1 c2), and unlike Gnome Terminal (which is
// confused).
//
// TODO(vtl): Moar.

// This class turns a sequence of input bytes into a sequence of |Token|s (each
// of which is either a Unicode codepoint or a control code).
class Tokenizer {
 public:
  Tokenizer(bool accept_8bit_C1,
            std::unique_ptr<CharacterDecoder> character_decoder);
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

 private:
  bool accept_8bit_C1_;  // Are 8-bit C1 control codes enabled?
  std::unique_ptr<CharacterDecoder> character_decoder_;

  // Last character was ESC (processing ESC c -> C1 transformation):
  bool got_ESC_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TOKENIZER_H_
