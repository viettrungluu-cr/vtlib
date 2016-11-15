#ifndef VTLIB_SRC_TERMINAL_IMPL_H_
#define VTLIB_SRC_TERMINAL_IMPL_H_

#include <memory>

#include <vtlib/character_decoder.h>
#include <vtlib/codepoint.h>
#include <vtlib/terminal.h>

namespace vtlib {

class TerminalImpl : public Terminal {
 public:
  explicit TerminalImpl(const Options& options);
  ~TerminalImpl() override;

  TerminalImpl(const TerminalImpl&) = delete;
  TerminalImpl& operator=(const TerminalImpl&) = delete;

  bool ProcessByte(uint8_t input_byte) override;

  const Options& options() const override { return options_; }
  bool options_set_accept_8bit_C1(bool accept_8bit_C1) override {
    options_.accept_8bit_C1 = accept_8bit_C1;
    return false;
  }
  bool options_set_character_encoding(
      CharacterEncoding character_encoding) override;

  const DisplayUpdates& display_updates() const override {
    return display_updates_;
  }
  void reset_display_updates() override { display_updates_ = DisplayUpdates(); }

 private:
  // Helper for |ProcessByte()|, etc.
  bool ProcessCodepoints();
  bool ProcessCodepoint(Codepoint codepoint);

  Options options_;
  DisplayUpdates display_updates_;

  std::unique_ptr<CharacterDecoder> character_decoder_;

  // Used by |ProcessByte()|. This is here so we don't have to re-create it each
  // time.
  CodepointVector codepoints_;
};

}  // namespace vtlib

#endif  // VTLIB_SRC_TERMINAL_IMPL_H_
