#ifndef VTLIB_INCLUDE_VTLIB_TERMINAL_H_
#define VTLIB_INCLUDE_VTLIB_TERMINAL_H_

#include <stdint.h>

#include <vtlib/character_encoding.h>
#include <vtlib/state_changes.h>
#include <vtlib/token.h>
#include <vtlib/tokenizer.h>

namespace vtlib {

//FIXME
class StateChanges;

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

  const StateChanges& state_changes() const { return state_changes_; }
  void reset_state_changes() { state_changes_ = StateChanges(); }

 private:
  // Helper for |ProcessByte()|.
  bool ProcessToken(Token token);

  Options options_;
  StateChanges state_changes_;

  Tokenizer tokenizer_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TERMINAL_H_
