#include <vtlib/character_decoder.h>

namespace vtlib {

// A trivial character decoder for (7-bit) ASCII.
class AsciiCharacterDecoder : public CharacterDecoder {
 public:
  AsciiCharacterDecoder() {}
  ~AsciiCharacterDecoder() override {}

  AsciiCharacterDecoder(const AsciiCharacterDecoder&) = delete;
  AsciiCharacterDecoder& operator=(const AsciiCharacterDecoder&) = delete;

  bool ProcessByte(uint8_t input_byte,
                   size_t* num_output_tokens,
                   Token* output_tokens) override {
    if (is_C0_control_code(input_byte) || input_byte >= 128u) {
      *num_output_tokens = 0u;
      return false;
    }
    output_tokens[0] = static_cast<Token>(input_byte);
    *num_output_tokens = 1u;
    return true;
  }

  // Static helper functions to determine whether a given byte is a control
  // code.
  static bool is_C0_control_code(uint8_t b) { return b <= 31u; }
  static bool is_C1_control_code(uint8_t b) { return b >= 128u && b <= 159u; }
  static bool is_control_code(uint8_t b) {
    return is_C0_control_code(b) || is_C1_control_code(b);
  }
};

}  // namespace vtlib
