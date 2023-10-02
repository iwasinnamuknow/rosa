#include "core/GameManager.hpp"
#include <physfs.h>
#include <span>

auto main(int argc, char** argv) -> int {

  auto args = std::span(argv, static_cast<size_t>(argc));
  PHYSFS_init(args[0]);

  rosa::GameManager::instance().run();

  return 0;
}