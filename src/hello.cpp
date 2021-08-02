#include <emscripten/bind.h>
#include <emscripten/emscripten.h>
#include <emscripten/val.h>
#include <stdio.h>
#include <wasm_simd128.h>

#include <iostream>
#include <string>

#include "buf_block_reader.h"
#include "m3u8_structural_indexer.h"

void stage1(const uint8_t* data, size_t len) {
  m3u8_structural_indexer::index<64>(data, len);
}

void parse(std::string m3u8) {
  stage1(reinterpret_cast<const uint8_t*>(m3u8.data()), m3u8.length());
}

EMSCRIPTEN_BINDINGS(my_module) { emscripten::function("parse", &parse); }