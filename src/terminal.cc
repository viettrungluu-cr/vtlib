#include <vtlib/terminal.h>

#include <assert.h>

namespace vtlib {

Terminal::Terminal(const Options& options)
    : options_(options),
      tokenizer_(options.accept_8bit_C1, options.character_encoding) {
//FIXME mark the whole viewport dirty
}

Terminal::~Terminal() = default;

bool Terminal::ProcessByte(uint8_t input_byte) {
  assert(tokens_.empty());
  tokenizer_.ProcessByte(input_byte, &tokens_);

  bool have_state_changes = false;
  for (auto token : tokens_)
    have_state_changes |= ProcessToken(token);
  tokens_.clear();
  return have_state_changes;
}

bool Terminal::ProcessToken(Token token) {
  return false;
}

}  // namespace vtlib
