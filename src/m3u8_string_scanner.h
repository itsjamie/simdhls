#include <bitmask.h>
#include <error.h>
#include <wasm_simd128.h>

struct m3u8_string_block {
  m3u8_string_block(uint64_t quote, uint64_t in_string)
      : _quote(quote), _in_string(in_string) {}

  // real quotes that aren't escaped
  uint64_t _quote;
  uint64_t _in_string;
};

// Scan blocks for string characters
class m3u8_string_scanner {
 public:
  m3u8_string_block next(const v128_t &in);
  error_code finish();

 private:
  // TODO: handle escaped string characters?

  // Whether the last iteration was still inside a string
  // (all 1's = true, all 0's = false)
  uint64_t prev_in_string = 0ULL;
};

// Return a mask of all string characters and their end quotes
// prev_in_string is overflow for the next block indicating in a string
m3u8_string_block m3u8_string_scanner::next(const v128_t &in) {
  const v128_t mask = wasm_i8x16_splat(uint8_t('"'));
  const v128_t eq = wasm_i8x16_eq(in, mask);
  const int32_t quote_mask = wasm_i8x16_bitmask(eq);

  // prefix_xor flips the bits inside the string, and flips off the end quote.
  // Then it is xor'd with prev_in_string:
  //   if we were in a string already, it's effect is flipped characters inside
  //   the string are outside, and characters outside a string are inside.
  const uint64_t in_string = prefix_xor(quote_mask) ^ prev_in_string;

  // Checks if we're still in a string at the end so the next block will know.
  prev_in_string = uint64_t(static_cast<int64_t>(in_string) >> 63);

  return m3u8_string_block(quote_mask, in_string);
}