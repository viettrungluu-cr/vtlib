#include <vtlib/terminal.h>

namespace vtlib {

Terminal::Terminal(const Options& options)
    : options_(options),
      tokenizer_(options.accept_8bit_C1, options.character_encoding) {
//FIXME mark the whole viewport dirty
}

Terminal::~Terminal() = default;

bool Terminal::ProcessByte(uint8_t input_byte) {
//FIXME don't re-create this vector on every byte
  TokenVector tokens;

  tokens.clear();
  tokenizer_.ProcessByte(input_byte, &tokens);

  bool have_state_changes = false;
  for (auto token : tokens)
    have_state_changes |= ProcessToken(token);
  return have_state_changes;
}

bool Terminal::ProcessToken(Token token) {
  return false;
}

}  // namespace vtlib
