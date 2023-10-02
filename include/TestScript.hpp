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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "core/NativeScriptEntity.hpp"
#include <spdlog/spdlog.h>

namespace rosa {

    class TestScript : public NativeScriptEntity {
        public:
            TestScript(Scene& scene, entt::entity entity) : NativeScriptEntity(scene, entity) {}

            void onCreate() override {
                spdlog::info("Test script initialised");
            }

            void onUpdate(float delta_time) override {
                //std::cout << "DeltaTime: " << delta_time << std::endl;
                spdlog::debug("Deltatime: {}", delta_time);
            }

            void onDestroy() override {

            }
    };

} // namespace rosa