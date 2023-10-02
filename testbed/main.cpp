#include "core/GameManager.hpp"
#include <memory>
#include <physfs.h>
#include <span>
#include "MyScene.hpp"

auto main(int argc, char** argv) -> int {

  auto args = std::span(argv, static_cast<size_t>(argc));
  PHYSFS_init(args[0]);

  auto& game_mgr = rosa::GameManager::instance();

  game_mgr.addScene("testbed", std::make_unique<MyScene>(game_mgr.getRenderWindow()));
  game_mgr.changeScene("testbed");
  game_mgr.run();

  return 0;
}