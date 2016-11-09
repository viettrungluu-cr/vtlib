#include "src/utf8_character_decoder.h"

#include <gtest/gtest.h>
#include <vtlib/token.h>

namespace vtlib {
namespace {

// 1 byte:  0xxxxxxx                            - 7 bits
//                                                U+0 to U+7F
TEST(Utf8CharacterDecoderTest, Valid1ByteC0) {
  Utf8CharacterDecoder d;

  for (uint8_t c = 0u; c < 0x20u; c++) {
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_FALSE(d.ProcessByte(c, &n, t));
    EXPECT_EQ(0u, n);
  }
}

TEST(Utf8CharacterDecoderTest, Valid1ByteNonC0) {
  Utf8CharacterDecoder d;

  for (uint8_t c = 0x20u; c < 0x80u; c++) {
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(c, &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(static_cast<Token>(c), t[0]);
  }
}

// 2 bytes: 110xxxxx 10xxxxxx                   - 6+5 = 11 bits
//                                                U+80 to U+7FF
TEST(Utf8CharacterDecoderTest, Valid2Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0x80u; c < 0x800u; c++) {
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 6u) | 0xc0u), &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(static_cast<Token>(c), t[0]);
  }
}

// 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx          - 4+6+6 = 16 bits
//                                                U+800 to U+D7FF,
//                                                U+E000 to U+FFFF
TEST(Utf8CharacterDecoderTest, Valid3Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0x800u; c < 0x10000u; c++) {
    if (c >= 0xd800u && c < 0xe000u)
      continue;

    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                              &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(static_cast<Token>(c), t[0]);
  }
}

// 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx - 3+6+6+6 = 21 bits
//                                                U+10000 to U+10FFFF
TEST(Utf8CharacterDecoderTest, Valid4Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0x10000u; c < 0x110000u; c++) {
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(
        static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                &n, t));
    EXPECT_EQ(0u, n);

    n = static_cast<size_t>(-1);
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(static_cast<Token>(c), t[0]);
  }
}

// Tests invalid codepoints U+D800 to U+DFFF.
TEST(Utf8CharacterDecoderTest, InvalidCodepoints1) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0xd800u; c < 0xe000u; c++) {
    // After the first byte, we can't tell anything.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &n, t));
    EXPECT_EQ(0u, n);

    // After the second byte, we can already tell that it's invalid (and we
    // should immediately emit two replacement tokens).
    n = static_cast<size_t>(-1);
    t[0] = 0;
    t[1] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                              &n, t));
    EXPECT_EQ(2u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
    EXPECT_EQ(d.replacement_token(), t[1]);

    // The third byte will also be detected as invalid (since it's an unexpected
    // continuation byte).
    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

// Tests invalid codepoints U+110000 to U+13FFFF
TEST(Utf8CharacterDecoderTest, InvalidCodepoints2) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0x110000u; c < 0x140000u; c++) {
    // After the first byte, we can't tell anything.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &n, t));
    EXPECT_EQ(0u, n);

    // After the second byte, we can already tell that it's invalid.
    n = static_cast<size_t>(-1);
    t[0] = 0;
    t[1] = 0;
    EXPECT_TRUE(d.ProcessByte(
        static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &n, t));
    EXPECT_EQ(2u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
    EXPECT_EQ(d.replacement_token(), t[1]);

    // The remaining two bytes will also each be detected as invalid (since they
    // are unexpected continuation bytes).
    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

// Tests invalid codepoints U+140000 to U+1FFFFF.
TEST(Utf8CharacterDecoderTest, InvalidCodepoints3) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0x140000u; c < 0x200000u; c++) {
    // After the first byte, we can already tell that it's invalid.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    // The remaining three bytes will also each be detected as invalid (since
    // they are unexpected continuation bytes).
    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(
        static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

TEST(Utf8CharacterDecoderTest, Overlong2Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0u; c < 0x80u; c++) {
    // After the first byte, we can already tell that it's invalid.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 6u) | 0xc0u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

TEST(Utf8CharacterDecoderTest, Overlong3Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0u; c < 0x800u; c++) {
    // After the first byte, we can't tell anything.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &n, t));
    EXPECT_EQ(0u, n);

    // After the second byte, we can already tell that it's invalid.
    n = static_cast<size_t>(-1);
    t[0] = 0;
    t[1] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                              &n, t));
    EXPECT_EQ(2u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
    EXPECT_EQ(d.replacement_token(), t[1]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

TEST(Utf8CharacterDecoderTest, Overlong4Byte) {
  Utf8CharacterDecoder d;

  for (uint32_t c = 0u; c < 0x10000u; c++) {
    // After the first byte, we can't tell anything.
    size_t n = static_cast<size_t>(-1);
    Token t[kMaxOutputTokensPerInputByte];
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &n, t));
    EXPECT_EQ(0u, n);

    // After the second byte, we can already tell that it's invalid.
    n = static_cast<size_t>(-1);
    t[0] = 0;
    t[1] = 0;
    EXPECT_TRUE(d.ProcessByte(
        static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &n, t));
    EXPECT_EQ(2u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
    EXPECT_EQ(d.replacement_token(), t[1]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u),
                &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);

    n = static_cast<size_t>(-1);
    t[0] = 0;
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &n, t));
    EXPECT_EQ(1u, n);
    EXPECT_EQ(d.replacement_token(), t[0]);
  }
}

// TODO(vtl): overlong encodings, moar invalid codepoints, "interrupted"
// encodings

}  // namespace
}  // namespace vtlib
