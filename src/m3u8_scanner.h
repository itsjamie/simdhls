#pragma once

#include <wasm_simd128.h>

#include "error.h"
#include "m3u8_string_scanner.h"
#include "m3u8_block.h"
#include "m3u8_string_block.h"
#include "m3u8_character_block.h"

class m3u8_scanner {
 public:
  m3u8_scanner() {}
  m3u8_block next(const v128_t &in);
  // Will either return UNCLOSED_STRING, or SUCCESS.
  error_code finish();

 private:
  // Whether the last character is part of a scalar token.
  // Anything that isn't whitespace or a structural character.
  uint64_t prev_scalar = 0ULL;
  m3u8_string_scanner string_scanner{};
};

// Check if the current character immediately follows a matching character.
uint64_t follows(const uint64_t match, uint64_t &overflow) {
  const uint64_t result = match << 1 | overflow;
  overflow = match >> 63;
  return result;
}

m3u8_block m3u8_scanner::next(const v128_t &in) {
  m3u8_string_block strings = string_scanner.next(in);
  m3u8_character_block characters = m3u8_character_block::classify(in);

  const uint64_t nonquote_scalar = characters.scalar() & ~strings.quote();
  uint64_t follows_nonquote_scalar = follows(nonquote_scalar, prev_scalar);

  return m3u8_block(strings, characters, follows_nonquote_scalar);
}

error_code m3u8_scanner::finish() { return string_scanner.finish(); }