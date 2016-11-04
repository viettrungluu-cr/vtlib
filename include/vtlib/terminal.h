#ifndef VTLIB_INCLUDE_VTLIB_TERMINAL_H_
#define VTLIB_INCLUDE_VTLIB_TERMINAL_H_

//#include <assert.h>
#include <stdint.h>

namespace vtlib {

//FIXME
class StateChanges;

// Represesents the state of one terminal. The basic interface is that the
// caller provides a sequence of input bytes ("characters"), and for each byte
// the state of the terminal is updated. Moreover, the caller is informed of
// what kind of state (e.g., display) changes resulted from each input byte,
// together with any "immediate" actions which need to be taken (e.g., ringing a
// bell).
//
// The caller may choose to "accumulate" state changes and defer acting on them
// as appropriate (e.g., defer updating the display) and allowable (e.g., a
// given input byte may result in the requirement that a particular string of
// input bytes be inserted into the input stream; this must be done
// "synchronously").
//FIXME moar
class Terminal {
 public:
//FIXME

  void OnInput(uint8_t input_byte, StateChanges* state_changes);

 private:

//FIXME disallow copy and assign
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TERMINAL_H_
