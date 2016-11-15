#ifndef VTLIB_INCLUDE_VTLIB_TERMINAL_H_
#define VTLIB_INCLUDE_VTLIB_TERMINAL_H_

#include <stdint.h>

#include <memory>

#include <vtlib/character_encoding.h>
#include <vtlib/display_updates.h>

namespace vtlib {

// Represesents the state of one terminal. The basic interface is that the
// caller provides a sequence of input bytes ("characters"), and for each byte
// the state of the terminal is updated.
//FIXME moar
class Terminal {
 public:
  struct Options {
//FIXME much moar

    // These can also be changed via escape sequences:
    bool accept_8bit_C1;
    CharacterEncoding character_encoding;
  };

  virtual ~Terminal() = default;

  Terminal(const Terminal&) = delete;
  Terminal& operator=(const Terminal&) = delete;

  static std::unique_ptr<Terminal> Create(const Options& options);

//FIXME
  virtual bool ProcessByte(uint8_t input_byte) = 0;

  virtual const Options& options() const = 0;
  virtual bool options_set_accept_8bit_C1(bool accept_8bit_C1) = 0;
  virtual bool options_set_character_encoding(
      CharacterEncoding character_encoding) = 0;

  virtual const DisplayUpdates& display_updates() const = 0;
  virtual void reset_display_updates() = 0;

 protected:
  Terminal() = default;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TERMINAL_H_
