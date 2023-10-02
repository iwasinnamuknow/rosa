#include <string>
#include <cassert>
#include <memory>
#include <type_traits>
#include <iostream>
#include <utility>
/*
class DataA {
  public:
    explicit DataA(std::string str) : m_str(std::move(str)) {}
    const std::string& getData() {
      return m_str;
    }
  
  private:
    std::string m_str;
};

class DataB {
  public:
    explicit DataB(const std::string& str) : str(str) {}
    const std::string& getData() {
      return str;
    }
  
  private:
    std::string str;
};

class DataC {
  public:
    explicit DataC(const std::string& str) : str(str) {}
    const std::string& getData() {
      return str;
    }
  
  private:
    std::string str;
};

class Reader {
  public:
    Reader(DataA& a, DataB& b) {
      assert(a.getData() == "DataA");
      assert(b.getData() == "DataB");
    }
};

int main() {
  const auto injector = boost::di::make_injector(
      boost::di::bind<DataA>.to(std::make_shared<DataA>("DataA")),
      boost::di::bind<DataB>.to(std::make_shared<DataB>("DataB")),
      boost::di::bind<DataC>.to(std::make_shared<DataC>("DataC"))
      );

  auto rd = injector.create<std::unique_ptr<Reader>>();

  assert((std::is_same<std::unique_ptr<Reader>, decltype(rd)>::value));
}
*/

#include "core/GameManager.hpp"
#include <physfs.h>
#include <core/PhysFSStream.hpp>
#include <span>

auto main(int argc, char** argv) -> int {

  auto args = std::span(argv, static_cast<size_t>(argc));
  PHYSFS_init(args[0]);

  rosa::GameManager mgr{};
  mgr.run();

  return 0;
}