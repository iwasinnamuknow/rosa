/*
 * This file is part of rosa.
 *
 *  rosa is free software: you can redistribute it and/or modify it under the terms of the
 *  GNU General Public License as published by the Free Software Foundation, either version
 *  3 of the License, or (at your option) any later version.
 *
 *  rosa is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 *  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with rosa. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include <GLFW/glfw3.h>
#include <entt/fwd.hpp>
#include <fmt/format.h>
#include <glm/glm.hpp>
#include <graphics/gl.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <physfs.h>
#include <sol/sol.hpp>
#include <spdlog/spdlog.h>
#include <tracy/Tracy.hpp>
#include <yaml-cpp/yaml.h>