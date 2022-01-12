#pragma once

#include <cstdint>

struct m3u8_string_block {
  m3u8_string_block(uint64_t quote, uint64_t in_string)
      : _quote(quote), _in_string(in_string) {}

  // Real unescaped quotes
  uint64_t quote() const { return _quote; }
  // Tail of string, all but starting quote
  uint64_t string_tail() const { return _in_string ^ _quote; }

  // real quotes that aren't escaped
  uint64_t _quote;
  uint64_t _in_string;
};