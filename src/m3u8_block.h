#pragma once

#include <cstdint>
#include <utility>

#include "m3u8_character_block.h"
#include "m3u8_string_block.h"

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
  m3u8_block(m3u8_string_block &&string, m3u8_character_block characters,
             uint64_t follows_potential_nonquote_scalar)
      : _string(std::move(string)),
        _characters(characters),
        _follows_potential_nonquote_scalar(follows_potential_nonquote_scalar) {}

  m3u8_block(m3u8_string_block string, m3u8_character_block characters,
             uint64_t follows_potential_nonquote_scalar)
      : _string(string),
        _characters(characters),
        _follows_potential_nonquote_scalar(follows_potential_nonquote_scalar) {}

  uint64_t structural_start() const noexcept {
    return potential_structural_start() & ~_string.string_tail();
  }

  // string and escape characters
  m3u8_string_block _string;
  // whitespace, structural characters, scalars
  m3u8_character_block _characters;
  // whether the previous character was a scalar
  uint64_t _follows_potential_nonquote_scalar;

 private:
  /**
   * Potential structural elements, may reside inside a string.
   * Includes #, comma, =, :, @
   */
  uint64_t potential_structural_start() const noexcept {
    return _characters.op() | potential_scalar_start();
  }

  uint64_t potential_scalar_start() const noexcept {
    return _characters.scalar() & ~follows_potential_scalar();
  }

  uint64_t follows_potential_scalar() const noexcept {
    return _follows_potential_nonquote_scalar;
  }
};