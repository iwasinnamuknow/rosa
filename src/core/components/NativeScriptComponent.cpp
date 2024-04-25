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

#include <core/components/NativeScriptComponent.hpp>

namespace rosa {

    auto NativeScriptComponent::bind(NativeScriptEntity* nse) -> void {
        instantiate_function = [this, nse](Scene* /*scene*/, Entity* /*entity*/) {
            instance = nse;
        };

        destroy_instance_function = [this]() {
            delete instance;
            instance = nullptr;
        };

        on_create_function = [](NativeScriptEntity* p_instance) -> void {
            p_instance->onCreate();
        };

        on_load_function = [](NativeScriptEntity* p_instance) -> void {
            p_instance->onLoad();
        };

        on_destroy_function = [](NativeScriptEntity* p_instance) -> void {
            p_instance->onDestroy();
        };

        on_update_function = [](NativeScriptEntity* p_instance, float delta_time) -> void {
            p_instance->onUpdate(delta_time);
        };

        on_input_function = [](NativeScriptEntity* p_instance, const Event& event) -> void {
            p_instance->onInput(event);
        };
    }

    auto operator<<(YAML::Emitter& out, const NativeScriptEntity& component) -> YAML::Emitter& {
        out << YAML::BeginMap;// nsc
        out << YAML::Key << "type" << YAML::Value << "native_script";
        out << YAML::Key << "data" << YAML::Value << component.serialise();
        out << YAML::Key << "script" << YAML::Value << component.getName();
        out << YAML::EndMap;// nsc
        return out;
    }

}// namespace rosa
