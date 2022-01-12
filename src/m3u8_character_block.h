#pragma once

#include <wasm_simd128.h>

#include <cstdint>

struct m3u8_character_block {
  static m3u8_character_block classify(const v128_t &in);
  // ASCII whitespace except newline ('\r', '\t', ' ')
  uint64_t whitespace() const noexcept;
  // non-quote structural characters ('#', '\n')
  uint64_t op() const noexcept;
  // neither a structural character or whitespace
  uint64_t scalar() const noexcept;

  uint64_t _whitespace;
  uint64_t _op;
};

uint64_t m3u8_character_block::whitespace() const noexcept {
  return _whitespace;
}
uint64_t m3u8_character_block::op() const noexcept { return _op; }
uint64_t m3u8_character_block::scalar() const noexcept {
  return ~(op() | whitespace());
}

m3u8_character_block m3u8_character_block::classify(const v128_t &in) {
  // TODO: fix me
  // PSHUFB operation on in to classify characters
  return {0, 0};
}