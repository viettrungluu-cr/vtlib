#include <vtlib/terminal.h>

#include <assert.h>

namespace vtlib {

Terminal::Terminal(const Options& options)
    : options_(options),
      character_decoder_(CharacterDecoder::Create(options.character_encoding)) {
  codepoints_.reserve(10u);  // Pick a random number; 10 should be plenty.

//FIXME mark the whole viewport dirty
}

Terminal::~Terminal() = default;

bool Terminal::ProcessByte(uint8_t input_byte) {
  assert(codepoints_.empty());
  character_decoder_->ProcessByte(input_byte, &codepoints_);
  return ProcessCodepoints();
}

bool Terminal::options_set_character_encoding(
    CharacterEncoding character_encoding) {
  assert(codepoints_.empty());
  character_decoder_->Flush(&codepoints_);
  bool rv = ProcessCodepoints();
  options_.character_encoding = character_encoding;
  character_decoder_ = CharacterDecoder::Create(character_encoding);
  return rv;
}

bool Terminal::ProcessCodepoints() {
  bool have_state_changes = false;
  for (auto codepoint : codepoints_)
    have_state_changes |= ProcessCodepoint(codepoint);
  codepoints_.clear();
  return have_state_changes;
}

bool Terminal::ProcessCodepoint(Codepoint codepoint) {
//FIXME
  return false;
}

}  // namespace vtlib
