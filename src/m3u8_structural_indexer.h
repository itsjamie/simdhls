#pragma once

#include <wasm_simd128.h>

#include <cstdint>

#include "error.h"
#include "m3u8_scanner.h"

class m3u8_structural_indexer {
 public:
  template <size_t STEP_SIZE>
  static error_code index(const uint8_t *buf, size_t len) noexcept;

 private:
  template <size_t STEP_SIZE>
  void step(const uint8_t *block, buf_block_reader<STEP_SIZE> &reader) noexcept;
  void next(const v128_t in, const m3u8_block block, size_t idx);
  error_code finish(size_t idx, size_t len);

  m3u8_scanner scanner{};
  uint64_t prev_structurals = 0;
};

template <size_t STEP_SIZE>
error_code m3u8_structural_indexer::index(const uint8_t *buf,
                                          size_t len) noexcept {
  if (len == 0) {
    return EMPTY;
  }

  buf_block_reader<STEP_SIZE> reader(buf, len);
  m3u8_structural_indexer indexer{};

  while (reader.has_full_block()) {
    indexer.step<STEP_SIZE>(reader.full_block(), reader);
  }

  // Handle the last block (will always be there unless file is empty)
  uint8_t block[STEP_SIZE];
  if (reader.get_remainder(block)) {
    return UNEXPECTED_ERROR;
  }
  indexer.step<STEP_SIZE>(block, reader);

  return indexer.finish(reader.block_index(), len);
}

template <>
void m3u8_structural_indexer::step<64>(const uint8_t *block,
                                       buf_block_reader<64> &reader) noexcept {
  v128_t in1 = wasm_v128_load(block);
  m3u8_block block_1 = scanner.next(in1);
  this->next(in1, block_1, reader.block_index());
  reader.advance();
}

void m3u8_structural_indexer::next(const v128_t &in, const m3u8_block &block,
                                   size_t idx) {
  // Output *last* iteration's structurals to the parser.
  indexer.write(uint32_t(idx - 64), prev_structurals);
}

error_code m3u8_structural_indexer::finish(size_t idx, size_t len) {
  error_code error = scanner.finish();
  if (error != SUCCESS) {
    return error;
  }

  return SUCCESS;
}
