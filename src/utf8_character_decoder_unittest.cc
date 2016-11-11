#include "src/utf8_character_decoder.h"

#include <algorithm>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <vtlib/token.h>

namespace vtlib {
namespace {

// 1 byte:  0xxxxxxx                            - 7 bits
//                                                U+0 to U+7F
TEST(Utf8CharacterDecoderTest, Valid1ByteC0) {
  Utf8CharacterDecoder d;
  TokenVector t;

  for (uint8_t c = 0u; c < 0x20u; c++) {
    t.clear();
    EXPECT_FALSE(d.ProcessByte(c, &t));
    EXPECT_EQ(TokenVector(), t) << c;
  }
}

TEST(Utf8CharacterDecoderTest, Valid1ByteNonC0) {
  Utf8CharacterDecoder d;
  TokenVector t;

  for (uint8_t c = 0x20u; c < 0x80u; c++) {
    t.clear();
    EXPECT_TRUE(d.ProcessByte(c, &t));
    EXPECT_EQ(TokenVector({static_cast<Token>(c)}), t) << c;
  }
}

// 2 bytes: 110xxxxx 10xxxxxx                   - 6+5 = 11 bits
//                                                U+80 to U+7FF
TEST(Utf8CharacterDecoderTest, Valid2Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;

  for (uint32_t c = 0x80u; c < 0x800u; c++) {
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 6u) | 0xc0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({static_cast<Token>(c)}), t) << c;
  }
}

// 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx          - 4+6+6 = 16 bits
//                                                U+800 to U+D7FF,
//                                                U+E000 to U+FFFF
TEST(Utf8CharacterDecoderTest, Valid3Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;

  for (uint32_t c = 0x800u; c < 0x10000u; c++) {
    if (c >= 0xd800u && c < 0xe000u)
      continue;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({static_cast<Token>(c)}), t) << c;
  }
}

// 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx - 3+6+6+6 = 21 bits
//                                                U+10000 to U+10FFFF
TEST(Utf8CharacterDecoderTest, Valid4Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;

  for (uint32_t c = 0x10000u; c < 0x110000u; c++) {
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({static_cast<Token>(c)}), t) << c;
  }
}

// Tests invalid codepoints U+D800 to U+DFFF.
TEST(Utf8CharacterDecoderTest, InvalidCodepoints1) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0xd800u; c < 0xe000u; c++) {
    // After the first byte, we can't tell anything.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    // After the second byte, we can already tell that it's invalid (and we
    // should immediately emit two replacement tokens).
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd, 0xfffd}), t) << c;

    // The third byte will also be detected as invalid (since it's an unexpected
    // continuation byte).
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

// Tests invalid codepoints U+110000 to U+13FFFF
TEST(Utf8CharacterDecoderTest, InvalidCodepoints2) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0x110000u; c < 0x140000u; c++) {
    // After the first byte, we can't tell anything.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    // After the second byte, we can already tell that it's invalid.
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd, 0xfffd}), t) << c;

    // The remaining two bytes will also each be detected as invalid (since they
    // are unexpected continuation bytes).
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

// Tests invalid codepoints U+140000 to U+1FFFFF.
TEST(Utf8CharacterDecoderTest, InvalidCodepoints3) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0x140000u; c < 0x200000u; c++) {
    // After the first byte, we can already tell that it's invalid.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    // The remaining three bytes will also each be detected as invalid (since
    // they are unexpected continuation bytes).
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

TEST(Utf8CharacterDecoderTest, Overlong2Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0u; c < 0x80u; c++) {
    // After the first byte, we can already tell that it's invalid.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 6u) | 0xc0u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

TEST(Utf8CharacterDecoderTest, Overlong3Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0u; c < 0x800u; c++) {
    // After the first byte, we can't tell anything.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 12u) | 0xe0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    // After the second byte, we can already tell that it's invalid.
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd, 0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

TEST(Utf8CharacterDecoderTest, Overlong4Byte) {
  Utf8CharacterDecoder d;
  TokenVector t;
  EXPECT_EQ(static_cast<Token>(0xfffd), d.replacement_token());

  for (uint32_t c = 0u; c < 0x10000u; c++) {
    // After the first byte, we can't tell anything.
    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c >> 18u) | 0xf0u), &t));
    EXPECT_EQ(TokenVector(), t) << c;

    // After the second byte, we can already tell that it's invalid.
    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 12u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd, 0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(
        d.ProcessByte(static_cast<uint8_t>(((c >> 6u) & 0x3fu) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;

    t.clear();
    EXPECT_TRUE(d.ProcessByte(static_cast<uint8_t>((c & 0x3f) | 0x80u), &t));
    EXPECT_EQ(TokenVector({0xfffd}), t) << c;
  }
}

// Used in test sequences.
struct TestStep {
  TestStep(uint8_t i, bool a, TokenVector o)
      : input(i), accept(a), output(std::move(o)) {}

  const uint8_t input;
  const bool accept;
  const TokenVector output;
};
using TestSequence = std::vector<TestStep>;

void RunTestSequence(Utf8CharacterDecoder* d, TestSequence seq) {
  TokenVector t;
  size_t i = 0u;
  for (const auto& step : seq) {
    t.clear();
    EXPECT_EQ(step.accept, d->ProcessByte(step.input, &t)) << i;
    EXPECT_EQ(step.output, t) << i;
    i++;
  }
}

// We'll use the following encodings in the following tests:
//   - U+12 = FF = ^L: 0x12
//   - U+41 = 'A': 0x41
//   - U+123: 0xc4, 0xa3
//   - U+1234: 0xe1, 0x88, 0xb4
//   - U+102345: 0xf4, 0x82, 0x8d, 0x85
// Also, for example, 0xf8 is never a valid byte.
// First, sanity check these.
TEST(Utf8CharacterDecoderTest, TestEncodings) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  RunTestSequence(&d, {{0x12u, false, {}}});
  RunTestSequence(&d, {{0x41u, true, {0x41}}});
  RunTestSequence(&d, {{0xc4u, true, {}}, {0xa3u, true, {0x123}}});
  RunTestSequence(
      &d, {{0xe1u, true, {}}, {0x88u, true, {}}, {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xf8u, true, {0xfffd}}});
}

TEST(Utf8CharacterDecoderTest, Interrupted2Byte) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  // Interrupted after the 1st byte:
  RunTestSequence(&d, {{0xc4u, true, {}}, {0x12u, false, {0xfffd}}});
  RunTestSequence(&d, {{0xc4u, true, {}}, {0x41u, true, {0xfffd, 0x41}}});
  RunTestSequence(
      &d, {{0xc4u, true, {}}, {0xc4u, true, {0xfffd}}, {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xc4u, true, {}},
                       {0xe1u, true, {0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xc4u, true, {}},
                       {0xf4u, true, {0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xc4u, true, {}}, {0xf8u, true, {0xfffd, 0xfffd}}});
}

TEST(Utf8CharacterDecoderTest, Interrupted3Byte) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  // Interrupted after the 1st byte:
  RunTestSequence(&d, {{0xe1u, true, {}}, {0x12u, false, {0xfffd}}});
  RunTestSequence(&d, {{0xe1u, true, {}}, {0x41u, true, {0xfffd, 0x41}}});
  RunTestSequence(
      &d, {{0xe1u, true, {}}, {0xc4u, true, {0xfffd}}, {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0xe1u, true, {0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0xf4u, true, {0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xe1u, true, {}}, {0xf8u, true, {0xfffd, 0xfffd}}});

  // Interrupted after the 2nd byte:
  RunTestSequence(
      &d,
      {{0xe1u, true, {}}, {0x88u, true, {}}, {0x12u, false, {0xfffd, 0xfffd}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0x88u, true, {}},
                       {0x41u, true, {0xfffd, 0xfffd, 0x41}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0x88u, true, {}},
                       {0xc4u, true, {0xfffd, 0xfffd}},
                       {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0x88u, true, {}},
                       {0xe1u, true, {0xfffd, 0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0x88u, true, {}},
                       {0xf4u, true, {0xfffd, 0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xe1u, true, {}},
                       {0x88u, true, {}},
                       {0xf8u, true, {0xfffd, 0xfffd, 0xfffd}}});
}

TEST(Utf8CharacterDecoderTest, Interrupted4Byte) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  // Interrupted after the 1st byte:
  RunTestSequence(&d, {{0xf4u, true, {}}, {0x12u, false, {0xfffd}}});
  RunTestSequence(&d, {{0xf4u, true, {}}, {0x41u, true, {0xfffd, 0x41}}});
  RunTestSequence(
      &d, {{0xf4u, true, {}}, {0xc4u, true, {0xfffd}}, {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0xe1u, true, {0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0xf4u, true, {0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xf4u, true, {}}, {0xf8u, true, {0xfffd, 0xfffd}}});

  // Interrupted after the 2nd byte:
  RunTestSequence(
      &d,
      {{0xf4u, true, {}}, {0x82u, true, {}}, {0x12u, false, {0xfffd, 0xfffd}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x41u, true, {0xfffd, 0xfffd, 0x41}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0xc4u, true, {0xfffd, 0xfffd}},
                       {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0xe1u, true, {0xfffd, 0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0xf4u, true, {0xfffd, 0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0xf8u, true, {0xfffd, 0xfffd, 0xfffd}}});

  // Interrupted after the 3rd byte:
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x12u, false, {0xfffd, 0xfffd, 0xfffd}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x41u, true, {0xfffd, 0xfffd, 0xfffd, 0x41}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0xc4u, true, {0xfffd, 0xfffd, 0xfffd}},
                       {0xa3u, true, {0x123}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0xe1u, true, {0xfffd, 0xfffd, 0xfffd}},
                       {0x88u, true, {}},
                       {0xb4u, true, {0x1234}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0xf4u, true, {0xfffd, 0xfffd, 0xfffd}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0x85u, true, {0x102345}}});
  RunTestSequence(&d, {{0xf4u, true, {}},
                       {0x82u, true, {}},
                       {0x8du, true, {}},
                       {0xf8u, true, {0xfffd, 0xfffd, 0xfffd, 0xfffd}}});
}

TEST(Utf8CharacterDecoderTest, UnexpectedContinuationByte) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  for (uint8_t i = 0u; i < 64u; i++) {
    uint8_t c = i | 0x80u;
    RunTestSequence(
        &d,
        {{c, true, {0xfffd}},
         {static_cast<uint8_t>(((i + 32u) % 64u) | 0x80u), true, {0xfffd}}});
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0x12u, false, {}}});
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0x41u, true, {0x41}}});
    RunTestSequence(
        &d, {{c, true, {0xfffd}}, {0xc4u, true, {}}, {0xa3u, true, {0x123}}});
    RunTestSequence(&d, {{c, true, {0xfffd}},
                         {0xe1u, true, {}},
                         {0x88u, true, {}},
                         {0xb4u, true, {0x1234}}});
    RunTestSequence(&d, {{c, true, {0xfffd}},
                         {0xf4u, true, {}},
                         {0x82u, true, {}},
                         {0x8du, true, {}},
                         {0x85u, true, {0x102345}}});
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0xf8u, true, {0xfffd}}});
  }
}

// 0b11111xxx (0xf8 to 0xff) are never valid bytes.
TEST(Utf8CharacterDecoderTest, InvalidByte) {
  Utf8CharacterDecoder d;
  EXPECT_EQ(0xfffd, d.replacement_token());

  for (uint32_t i = 0xf8u; i < 0x100u; i++) {
    uint8_t c = static_cast<uint8_t>(i);
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0x12u, false, {}}});
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0x41u, true, {0x41}}});
    RunTestSequence(
        &d, {{c, true, {0xfffd}}, {0xc4u, true, {}}, {0xa3u, true, {0x123}}});
    RunTestSequence(&d, {{c, true, {0xfffd}},
                         {0xe1u, true, {}},
                         {0x88u, true, {}},
                         {0xb4u, true, {0x1234}}});
    RunTestSequence(&d, {{c, true, {0xfffd}},
                         {0xf4u, true, {}},
                         {0x82u, true, {}},
                         {0x8du, true, {}},
                         {0x85u, true, {0x102345}}});
    RunTestSequence(&d, {{c, true, {0xfffd}}, {0xf8u, true, {0xfffd}}});
  }
}

}  // namespace
}  // namespace vtlib
