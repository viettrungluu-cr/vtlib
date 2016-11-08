#include <vtlib/tokenizer.h>

#include <assert.h>

#include <utility>

namespace vtlib {

Tokenizer::Tokenizer(bool accept_8bit_C1,
          std::unique_ptr<CharacterDecoder> character_decoder)
    : accept_8bit_C1_(accept_8bit_C1),
      character_decoder_(std::move(character_decoder)),
      got_ESC_(false) {
  assert(character_decoder_);
}

Tokenizer::~Tokenizer() {}

void Tokenizer::ProcessByte(uint8_t input_byte,
                            size_t* num_output_tokens,
                            Token* output_tokens) {
  if (got_ESC_) {
    got_ESC_ = false;
//FIXME

    return;
  }

  if (character_decoder_->ProcessByte(input_byte, num_output_tokens,
                                      output_tokens))
    return;

  if (CharacterDecoder::is_control_code(input_byte)) {
    // ESC is processed into sequences that may be transformed into C1 control
    // codes.
    if (input_byte == 27u) {
      got_ESC_ = true;
      return;
    }

    if (CharacterDecoder::is_C0_control_code(input_byte) ||
        (accept_8bit_C1_ && CharacterDecoder::is_C1_control_code(input_byte))) {
      assert(*num_output_tokens < kMaxOutputTokensPerInputByte);
      output_tokens[*num_output_tokens] = -static_cast<Token>(input_byte);
      (*num_output_tokens)++;
      return;
    }
  }

  // Otherwise, just eat unknown/invalid characters (and 8-bit C1 control codes
  // when they are not being accepted).
}

std::unique_ptr<CharacterDecoder> Tokenizer::set_character_decoder(
    std::unique_ptr<CharacterDecoder> character_decoder) {
  assert(character_decoder);
  std::swap(character_decoder, character_decoder_);
  return character_decoder;
}

}  // namespace vtlib
