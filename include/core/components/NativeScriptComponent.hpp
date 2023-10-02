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
#include <cstddef>
#include <functional>

namespace rosa {

    struct NativeScriptComponent {
        NativeScriptEntity* instance = nullptr;

        std::function<void(Scene*, Entity*)> instantiate_function;
        std::function<void()> destroy_instance_function;

        std::function<void(NativeScriptEntity*)> on_create_function;
        std::function<void(NativeScriptEntity*)> on_destroy_function;
        std::function<void(NativeScriptEntity*, float)> on_update_function;

        template<typename T>
        void bind() {
            instantiate_function = [this](Scene* scene, Entity* entity) {
                instance = new T(scene, entity);
            };

            destroy_instance_function = [this]() {
                delete instance;
                instance = nullptr;
            };

            on_create_function = [](NativeScriptEntity* instance) -> void {
                instance->onCreate();
            };

            on_destroy_function = [](NativeScriptEntity* instance) -> void {
                instance->onDestroy();
            };

            on_update_function = [](NativeScriptEntity* instance, float delta_time) -> void {
                instance->onUpdate(delta_time);
            };
        }
    };

} // namespace rosa