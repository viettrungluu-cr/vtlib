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

Tokenizer::Tokenizer(bool accept_8bit_C1,
                     CharacterEncoding character_encoding)
    : Tokenizer(accept_8bit_C1, CharacterDecoder::Create(character_encoding)) {}

Tokenizer::~Tokenizer() = default;

void Tokenizer::ProcessByte(uint8_t input_byte, TokenVector* output_tokens) {
  if (got_ESC_) {
    got_ESC_ = false;
    // If it's a C1 escape sequence, we just transform it into a C1 token.
    if (input_byte >= 64u && input_byte <= 95u) {
      output_tokens->push_back(-static_cast<Token>(input_byte) - 64);
      return;
    }
    // Otherwise, we just emit an ESC token, and process the character as usual.
    output_tokens->push_back(TOKEN_ESC);
  }

  if (character_decoder_->ProcessByte(input_byte, output_tokens))
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
      output_tokens->push_back(-static_cast<Token>(input_byte));
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

void Tokenizer::set_character_encoding(CharacterEncoding character_encoding) {
  set_character_decoder(CharacterDecoder::Create(character_encoding));
}

}  // namespace vtlib
