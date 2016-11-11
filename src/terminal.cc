#include <vtlib/terminal.h>

namespace vtlib {

Terminal::Terminal(const Options& options)
    : options_(options),
      tokenizer_(options.accept_8bit_C1, options.character_encoding) {
//FIXME mark the whole viewport dirty
}

Terminal::~Terminal() = default;

bool Terminal::ProcessByte(uint8_t input_byte) {
  size_t num_tokens = 0u;
  Token tokens[kMaxOutputTokensPerInputByte];
  tokenizer_.ProcessByte(input_byte, &num_tokens, tokens);

  bool have_state_changes = false;
  for (size_t i = 0u; i < num_tokens; i++)
    have_state_changes |= ProcessToken(tokens[i]);
  return have_state_changes;
}

bool Terminal::ProcessToken(Token token) {
  return false;
}

}  // namespace vtlib
