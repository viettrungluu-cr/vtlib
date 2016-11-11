#ifndef VTLIB_INCLUDE_VTLIB_TERMINAL_H_
#define VTLIB_INCLUDE_VTLIB_TERMINAL_H_

#include <stdint.h>

#include <vtlib/character_encoding.h>
#include <vtlib/display_updates.h>
#include <vtlib/token.h>
#include <vtlib/tokenizer.h>

namespace vtlib {

// Represesents the state of one terminal. The basic interface is that the
// caller provides a sequence of input bytes ("characters"), and for each byte
// the state of the terminal is updated.
//FIXME moar
class Terminal {
 public:
  struct Options {
//FIXME much moar
    bool accept_8bit_C1;
    CharacterEncoding character_encoding;
  };

  explicit Terminal(const Options& options);
  ~Terminal();

  Terminal(const Terminal&) = delete;
  Terminal& operator=(const Terminal&) = delete;

//FIXME
  bool ProcessByte(uint8_t input_byte);

  const Options& options() const { return options_; }
  void options_set_accept_8bit_C1(bool accept_8bit_C1) {
    tokenizer_.set_accept_8bit_C1(accept_8bit_C1);
  }
  void options_set_character_encoding(CharacterEncoding character_encoding) {
    tokenizer_.set_character_encoding(character_encoding);
  }

  const DisplayUpdates& display_updates() const { return display_updates_; }
  void reset_display_updates() { display_updates_ = DisplayUpdates(); }

 private:
  enum class State : int;

  // Helper for |ProcessByte()|.
  bool ProcessToken(Token token);

  Options options_;
  DisplayUpdates display_updates_;

  Tokenizer tokenizer_;

  State state_;

  // Used by |ProcessByte()|. This is here so we don't have to re-create it each
  // time.
  TokenVector tokens_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TERMINAL_H_
