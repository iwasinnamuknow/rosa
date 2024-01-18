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

#include <core/Entity.hpp>
#include <core/Scene.hpp>

#include <string>
#include <memory>
#include <algorithm>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>
#include <physfs.h>
#include <spdlog/spdlog.h>
#include <sol/sol.hpp>
#include <fmt/format.h>
#include <entt/entt.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>
#include <graphics/gl.hpp>
#include <GLFW/glfw3.h>
#include <tracy/Tracy.hpp>
#include <yaml-cpp/yaml.h>