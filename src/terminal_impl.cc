#include "src/terminal_impl.h"

#include <assert.h>

namespace vtlib {

TerminalImpl::TerminalImpl(const Options& options)
    : options_(options),
      character_decoder_(CharacterDecoder::Create(options.character_encoding)) {
  codepoints_.reserve(10u);  // Pick a random number; 10 should be plenty.

//FIXME mark the whole viewport dirty
}

TerminalImpl::~TerminalImpl() = default;

bool TerminalImpl::ProcessByte(uint8_t input_byte) {
  assert(codepoints_.empty());
  character_decoder_->ProcessByte(input_byte, &codepoints_);
  return ProcessCodepoints();
}

bool TerminalImpl::options_set_character_encoding(
    CharacterEncoding character_encoding) {
  assert(codepoints_.empty());
  character_decoder_->Flush(&codepoints_);
  bool rv = ProcessCodepoints();
  options_.character_encoding = character_encoding;
  character_decoder_ = CharacterDecoder::Create(character_encoding);
  return rv;
}

bool TerminalImpl::ProcessCodepoints() {
  bool have_state_changes = false;
  for (auto codepoint : codepoints_)
    have_state_changes |= ProcessCodepoint(codepoint);
  codepoints_.clear();
  return have_state_changes;
}

bool TerminalImpl::ProcessCodepoint(Codepoint codepoint) {
//FIXME
  return false;
}

}  // namespace vtlib
