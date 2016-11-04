#ifndef VTLIB_INCLUDE_VTLIB_INTERNAL_INPUT_H_
#define VTLIB_INCLUDE_VTLIB_INTERNAL_INPUT_H_

#include <stddef.h>
#include <stdint.h>

#include <vtlib/character_encoding.h>

namespace vtlib {
namespace internal {

// Input processing happens in several stages:
//
//FIXME
// 1. Control code and UTF-8 processing: The initial input is a stream of bytes;
// the output is a stream of *tokens* that are either (graphical) characters (as
// Unicode codepoints >= 32) or "C0 and C1" control codes. This stage may map
// multiple (consecutive) input bytes to a single token.
//
// C0 control codes are bytes in the range 0..31. C1 control codes are bytes in
// the range 128..159 that are NOT a part of a UTF-8 character encoding (note
// that this means that determining whether a byte is a C1 control code is
// stateful, and errors can occur if processing is started midstream in a UTF-8
// sequence).
//
// Moreover, each C1 control code has an equivalent two-byte sequence, all of
// the form ESC c, where ESC = 27 and c is in the range 64..95, with ESC c
// simply being mapped to the C1 code c+64.
//
// In this first stage, we extract C0 and C1 control codes, including
// translating valid ESC c sequences (c in the range 64..95) into C1 control
// codes, and also translate UTF-8 sequences into Unicode codepoints.
//
// The UTF-8 encoding for codepoints in the range 0..127 is just as single bytes
// of identical values. Thus there is no ambiguity about whether to accept a
// UTF-8 encoding of a codepoint 0..31 as a C0 control code, nor is there any
// ambiguity about accepting ESC c' where c' is a UTF-8 encoding of a codepoint
// 64..95 as an encoding of a C1 control code. On the other hand, UTF-8
// encodings for the codepoints 128..159 are NOT considered to be C1 control
// codes (indeed, they are graphical characters).
//
// Errors: According to standards, once the processing of a control sequence has
// begun it must continue until either the sequence is complete or a byte that
// is not permitted in that sequence is encountered. In particular, if the
// sequence ESC c is encountered for some c outside the range 64..95, then the
// ESC MUST be ignored; c MAY also be ignored (which, following XTerm, we do).
// (TODO(vtl): What do the standards say exactly? ECMA-48, ISO/IEC 6429, ISO/IEC
// 2022.)
//
// However, for UTF-8 sequences, we proceed somewhat differently. E.g., if c1 c2
// c3 is encountered with c1 c2 the first two bytes of a valid 3- (or 4-) byte
// UTF-8 sequence and c3 NOT a valid third byte, then the codepoint for the
// Unicode replacement character (U+FFFD) is output twice (substituting for c1
// and c2) and processing started anew for c3 (e.g., c3 may the start of a new
// UTF-8 sequence or a C0/C1 control code, and it will be treated as such).
//FIXME check behavior for overlong encodings, invalid codepoints

// We encode control codes as non-positive values (i.e., values <= 0), and the
// remaining Unicode codepoints as strictly positive values (i.e., values > 0,
// indeed >= 32). The former are as encoded as minus their C0/C1 byte value.
using Token = int32_t;

// Names for the control code tokens.
enum TokenNames : int32_t {
  // C0 control codes:
  TOKEN_NUL = 0,
  TOKEN_SOH = -1,
  TOKEN_STX = -2,
  TOKEN_ETX = -3,
  TOKEN_EOT = -4,
  TOKEN_ENQ = -5,
  TOKEN_ACK = -6,
  TOKEN_BEL = -7,
  TOKEN_BS = -8,
  TOKEN_HT = -9,
  TOKEN_LF = -10,
  TOKEN_VT = -11,
  TOKEN_FF = -12,
  TOKEN_CR = -13,
  TOKEN_SO = -14,
  TOKEN_SI = -15,
  TOKEN_DLE = -16,
  TOKEN_DC1 = -17,
  TOKEN_DC2 = -18,
  TOKEN_DC3 = -19,
  TOKEN_DC4 = -20,
  TOKEN_NAK = -21,
  TOKEN_SYN = -22,
  TOKEN_ETB = -23,
  TOKEN_CAN = -24,
  TOKEN_EM = -25,
  TOKEN_SUB = -26,
  TOKEN_ESC = -27,
  TOKEN_FS = -28,
  TOKEN_GS = -29,
  TOKEN_RS = -30,
  TOKEN_US = -31,
  // C1 control codes:
  TOKEN_PAD = -128,
  TOKEN_HOP = -129,
  TOKEN_BPH = -130,
  TOKEN_NBH = -131,
  TOKEN_IND = -132,
  TOKEN_NEL = -133,
  TOKEN_SSA = -134,
  TOKEN_ESA = -135,
  TOKEN_HTS = -136,
  TOKEN_HTJ = -137,
  TOKEN_VTS = -138,
  TOKEN_PLD = -139,
  TOKEN_PLU = -140,
  TOKEN_RI = -141,
  TOKEN_SS2 = -142,
  TOKEN_SS3 = -143,
  TOKEN_DCS = -144,
  TOKEN_PU1 = -145,
  TOKEN_PU2 = -146,
  TOKEN_STS = -147,
  TOKEN_CCH = -148,
  TOKEN_MW = -149,
  TOKEN_SPA = -150,
  TOKEN_EPA = -151,
  TOKEN_SOS = -152,
  TOKEN_SGCI = -153,
  TOKEN_SCI = -154,
  TOKEN_CSI = -155,
  TOKEN_ST = -156,
  TOKEN_OSC = -157,
  TOKEN_PM = -158,
  TOKEN_APC = -159,
};

class Tokenizer {
 public:
//FIXME
  Tokenizer(bool allow_8bit_C1, CharacterEncoding character_encoding)
      : allow_8bit_C1_(allow_8bit_C1),
        character_encoding_(character_encoding),
        got_ESC_(false),
        num_buffered_bytes_(0u) {}
  ~Tokenizer() {}

  // Processes the input byte; returns the number (0..4) of tokens produced
  // |tokens[...]|. (|tokens| must point to an array of size at least four.)
  size_t OnInput(uint8_t input_byte, Token* tokens);

  bool allow_8bit_C1() const { return allow_8bit_C1_; }
  CharacterEncoding character_encoding() const { return character_encoding_; }

  // Note: Changing these settings may have side effects (i.e., setting a value
  // and immediately setting it back to the previous value is not nilpotent).
  void SetAllow8bitC1(bool allow_8bit_C1);
  void SetCharacterEncoding(CharacterEncoding character_encoding);

 private:
  size_t OnInputContinued(uint8_t input_byte, Token* tokens);

  bool allow_8bit_C1_;  // Are 8-bit C1 control codes enabled?
  CharacterEncoding character_encoding_;

  // Last character was ESC (processing ESC c -> C1 transformation):
  bool got_ESC_;

  // Got the initial |num_buffered_bytes_| of a (potentially) valid multibyte
  // (e.g., UTF-8) sequence:
  size_t num_buffered_bytes_;

//FIXME
};

}  // namespace internal
}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_INTERNAL_INPUT_H_
