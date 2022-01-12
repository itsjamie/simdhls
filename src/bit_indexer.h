#pragma once

#include <emscripten.h>
#include <wasm_simd128.h>

#include <cstdint>

class bit_indexer {
 public:
  uint32_t *tail;

  bit_indexer(uint32_t *index_buf) : tail(index_buf) {}

  void write(uint32_t idx, uint64_t bits) {
    if (bits == 0) {
      return;
    }

    // __builtin_popcount comes from Clang, and is compiled to popcnt
    // instruction in wasm.
    int cnt = static_cast<int>(__builtin_popcount(bits));
  }
}