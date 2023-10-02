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

#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace rosa {

    class NativeScriptEntity {
        public:
            explicit NativeScriptEntity(Scene& scene, entt::entity entity) : m_scene(scene), m_entity(entity) {}

            virtual ~NativeScriptEntity() = default;

            virtual auto onCreate() -> void{};
            virtual auto onUpdate(float delta_time) -> void{};
            virtual auto onDestroy() -> void{};

            template<typename T>
            auto getComponent() -> T& {
                return m_scene.getRegistry().get<T>(m_entity);
            }

            
        private:
            entt::entity m_entity;
            Scene& m_scene;
            friend class Scene;
    };

} // namespace rosa