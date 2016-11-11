#include <vtlib/tokenizer.h>

#include <gtest/gtest.h>
#include <vtlib/character_encoding.h>

namespace vtlib {
namespace {

TEST(TokenizerTest, AsciiNonControl) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 32u; c <= 127u; c++) {
    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(c, o[0]) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(c, o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiC0ControlNonEsc) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 0u; c <= 31u; c++) {
    if (c == 27u)
      continue;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiInvalidNonC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 160u; c <= 255u; c++) {
    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    EXPECT_EQ(0u, o.size()) << c;
  }
}

TEST(TokenizerTest, Ascii8bitC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 128u; c <= 159u; c++) {
    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[0]) << c;
  }
}

TEST(TokenizerTest, EscapeSequence) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 64u; c <= 95u; c++) {
    o.clear();
    t1.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiEscapeAsciiNonC0Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 96u; c <= 127u; c++) {
    o.clear();
    t1.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(2u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;
    EXPECT_EQ(static_cast<Token>(c), o[1]) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(2u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;
    EXPECT_EQ(static_cast<Token>(c), o[1]) << c;
  }
}

TEST(TokenizerTest, AsciiEscapeNonAsciiNonC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 160u; c <= 255u; c++) {
    o.clear();
    t1.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;
  }
}

TEST(TokenizerTest, AsciiEscapeC1Control) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 128u; c <= 159u; c++) {
    o.clear();
    t1.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(2u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;
    EXPECT_EQ(-static_cast<Token>(c), o[1]) << c;
  }
}

TEST(TokenizerTest, AsciiEscapeC1EscapeSequence) {
  Tokenizer t1(false, CharacterEncoding::ASCII);
  Tokenizer t2(true, CharacterEncoding::ASCII);
  TokenVector o;

  for (unsigned c = 64u; c <= 95u; c++) {
    o.clear();
    t1.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t1.ProcessByte(27u, &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;

    o.clear();
    t1.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    EXPECT_EQ(0u, o.size()) << c;

    o.clear();
    t2.ProcessByte(27u, &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(TOKEN_ESC, o[0]) << c;

    o.clear();
    t2.ProcessByte(static_cast<uint8_t>(c), &o);
    ASSERT_EQ(1u, o.size()) << c;
    EXPECT_EQ(-static_cast<Token>(c) - 64, o[0]) << c;
  }
}

// TODO(vtl): Test setting accept_8bit_C1, character_decoder.

}  // namespace
}  // namespace vtlib
