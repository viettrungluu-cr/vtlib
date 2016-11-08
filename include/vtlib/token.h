#ifndef VTLIB_INCLUDE_VTLIB_TOKEN_H_
#define VTLIB_INCLUDE_VTLIB_TOKEN_H_

#include <stdint.h>

namespace vtlib {

// Tokens represent either Unicode codepoints (as non-negative values) or
// control codes (as non-positive values). (Note that there is overlap for NUL,
// but we can tolerate this.) Control codes are encoded as minus their C0 or
// (8-bit) C1 byte value.
using Token = int32_t;

// Names for the control code tokens.
enum TokenNames : Token {
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

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_TOKEN_H_
