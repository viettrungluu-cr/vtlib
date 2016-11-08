#include <vtlib/tokenizer.h>

#include <gtest/gtest.h>
#include <vtlib/character_encoding.h>

namespace vtlib {
namespace {

TEST(TokenizerTest, AsciiNonControl) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 32u; c <= 127u; c++) {
    n = 0u;
    o[0] = -1234;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(c, o[0]) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(c, o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiC0ControlNonEsc) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 0u; c <= 31u; c++) {
    if (c == 27u)
      continue;

    n = 0u;
    o[0] = -1234;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiInvalidNonC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 160u; c <= 255u; c++) {
    n = 1u;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;
  }
}

TEST(TokenizerTest, Ascii8bitC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 128u; c <= 159u; c++) {
    n = 1u;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, EscapeSequence) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 64u; c <= 95u; c++) {
    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;
  }
}

TEST(TokenizerTest, BadEscapeSequenceAsciiNonControl) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 96u; c <= 127u; c++) {
    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(static_cast<Token>(c), o[0]) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, BadEscapeSequenceAsciiInvalidNonC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 160u; c <= 255u; c++) {
    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;
  }
}

TEST(TokenizerTest, BadEscapeSequenceAscii8bitC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 128u; c <= 159u; c++) {
    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, BadEscapeSequenceEscapeSequence) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  size_t n;
  Token o[kMaxOutputTokensPerInputByte];

  for (unsigned c = 64u; c <= 95u; c++) {
    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t1.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t1.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 1u;
    t2.ProcessByte(27u, &n, o);
    EXPECT_EQ(0u, n) << c;

    n = 0u;
    o[0] = -1234;
    t2.ProcessByte(static_cast<uint8_t>(c), &n, o);
    EXPECT_EQ(1u, n) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;
  }
}

// TODO(vtl): Test setting accept_8bit_C1, character_decoder.

}  // namespace
}  // namespace vtlib
