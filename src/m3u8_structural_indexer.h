#include <wasm_simd128.h>

#include <cstdint>

#include "m3u8_scanner.h"

class m3u8_structural_indexer {
 public:
  template <size_t STEP_SIZE>
  static int index(const uint8_t *buf, size_t len) noexcept;

 private:
  template <size_t STEP_SIZE>
  void step(const uint8_t *block, buf_block_reader<STEP_SIZE> &reader) noexcept;
  void next(const v128_t in, const m3u8_block block, size_t idx);
};

template <size_t STEP_SIZE>
int m3u8_structural_indexer::index(const uint8_t *buf, size_t len) noexcept {
  buf_block_reader<STEP_SIZE> reader(buf, len);
  m3u8_structural_indexer indexer{};

  while (reader.has_full_block()) {
    indexer.step<STEP_SIZE>(reader.full_block(), reader);
  }

  // Handle the last block (will always be there unless file is empty)
  uint8_t block[STEP_SIZE];
  if (reader.get_remainder(block)) {
    return -1;
  }
  indexer.step<STEP_SIZE>(block, reader);

  return indexer.finish(parser, reader.block_index(), len);
}

template <>
void m3u8_structural_indexer::step<64>(const uint8_t *block,
                                       buf_block_reader<64> &reader) noexcept {
  v128_t in1 = wasm_v128_load(block);
  m3u8_block block_1 = scanner.next(in1);
  this->next(in1, block_1, reader.block_index());
  reader.advance();
}
