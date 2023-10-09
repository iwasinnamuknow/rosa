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

#include "core/Entity.hpp"
#include "core/NativeScriptEntity.hpp"
#include <cstddef>
#include <functional>

namespace rosa {

    struct NativeScriptComponent {
        NativeScriptEntity* instance = nullptr;

        std::function<void(std::reference_wrapper<Scene>, std::reference_wrapper<Entity>)> instantiate_function;
        std::function<void()> destroy_instance_function;

        std::function<void(NativeScriptEntity*)> on_create_function;
        std::function<void(NativeScriptEntity*)> on_destroy_function;
        std::function<void(NativeScriptEntity*, float)> on_update_function;
        std::function<void(NativeScriptEntity*, sf::Event)> on_input_function;

        template<typename T>
        void bind() {
            instantiate_function = [this](std::reference_wrapper<Scene> scene, std::reference_wrapper<Entity> entity) {
                instance = new T(scene, entity);
            };

            destroy_instance_function = [this]() {
                delete instance;
                instance = nullptr;
            };

            on_create_function = [](NativeScriptEntity* p_instance) -> void {
                p_instance->onCreate();
            };

            on_destroy_function = [](NativeScriptEntity* p_instance) -> void {
                p_instance->onDestroy();
            };

            on_update_function = [](NativeScriptEntity* p_instance, float delta_time) -> void {
                p_instance->onUpdate(delta_time);
            };

            on_input_function = [](NativeScriptEntity* p_instance, sf::Event event) -> void {
                p_instance->onInput(event);
            };
        }
    };

} // namespace rosa