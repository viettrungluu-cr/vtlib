#include <vtlib/internal/input.h>

namespace vtlib {
namespace internal {

size_t Tokenizer::OnInput(uint8_t input_byte, Token* tokens) {
  if (num_buffered_bytes_)
    return OnInputContinued(input_byte, tokens);

  if (got_ESC_) {
    got_ESC_ = false;
  }

  if (input_byte < 32u) {
  }
//FIXME

  return 0u;
}

void Tokenizer::SetAllow8bitC1(bool allow_8bit_C1) {
  allow_8bit_C1_ = allow_8bit_C1;
}

void Tokenizer::SetCharacterEncoding(CharacterEncoding character_encoding) {
  if (character_encoding_ == character_encoding)
    return;

  // Flush buffered bytes. TODO(vtl): Is this right? (Or maybe, e.g., if the
  // previous encoding was UTF-8, we should emit replacement characters? This
  // would require being able to emit characters here.)
  num_buffered_bytes_ = 0u;
  character_encoding_ = character_encoding;
}

size_t Tokenizer::OnInputContinued(uint8_t input_byte, Token* tokens) {
//FIXME

  return 0u;
}

}  // namespace internal
}  // namespace vtlib
