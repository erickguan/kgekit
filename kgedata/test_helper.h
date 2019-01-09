#pragma once

#include <memory>
#include <fstream>
#include <pybind11/pytypes.h>
#include <pybind11/embed.h>
/*
 * catch2 config
 */
#define CATCH_CONFIG_CPP17_UNCAUGHT_EXCEPTIONS
#define CATCH_CONFIG_CPP17_STRING_VIEW
#include <catch.hpp>

#include "kgedata.h"

namespace kgedata {

namespace py = pybind11;

using std::make_shared;
using std::shared_ptr;

class IndexerTestsFixture {
protected:
  template <typename T>
  shared_ptr<T> getIndexer()
  {
    auto kgedata = py::module::import("kgedata_testing");
    py::list content;
    std::ifstream f(path);

    std::string h, r, t;
    while (f >> h >> r >> t) {
      content.append(Triple(h, r, t));
    }
    assert(content.size() == 3);

    return make_shared<T>(content, "hrt");
  }
private:
  std::string path = "kgedata/tests/fixtures/triple.txt";
  py::scoped_interpreter guard{};
};

} // namespace kgedata
