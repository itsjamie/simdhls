#include <wasm_simd128.h>

#include <cstdint>
#include <utility>

#include "error.h"
#include "m3u8_string_scanner.h"

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

// m3u8_character_block m3u8_character_block::classify(const v128_t &in) {
//   wasm_i8x16_shuffle()
// }

/**
 * A block of scanned m3u8, with information on attributes
 *
 * Tags begin with '#EXT' and are case-sensitive.
 *
 * Lines that begin with '#' but are not proceeded by 'EXT' can be dropped.
 * They are comments.
 *
 * We look for '#EXT' case-sensitive these lines should be scanned further.
 *
 * A line that does not start with whitespace or # should be parsed as a URI
 * line.
 *
 */
struct m3u8_block {
 public:
  m3u8_block(m3u8_string_block &&string, m3u8_character_block characters)
      : _string(std::move(string)), _characters(characters) {}
  m3u8_block(m3u8_string_block string, m3u8_character_block characters)
      : _string(string), _characters(characters) {}

  // string and escape characters
  m3u8_string_block _string;
  // whitespace, structural characters, scalars
  m3u8_character_block _characters;
};

class m3u8_scanner {
 public:
  m3u8_scanner() {}
  m3u8_block next(const v128_t &in);
  // 0 on success, 1 otherwise.
  error_code finish();

 private:
  m3u8_string_scanner string_scanner{};
};

m3u8_block m3u8_scanner::next(const v128_t &in) {
  m3u8_string_block strings = string_scanner.next(in);
  m3u8_character_block characters = m3u8_character_block::classify(in);
  return m3u8_block(strings, characters);
}

error_code m3u8_scanner::finish() { return string_scanner.finish(); }