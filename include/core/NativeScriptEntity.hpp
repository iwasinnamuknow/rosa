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

//#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <entt/entt.hpp>
#include <spdlog/spdlog.h>
#include <core/Entity.hpp>
#include <core/Scene.hpp>

namespace rosa {

    class NativeScriptEntity {
        public:
            explicit NativeScriptEntity(Scene* scene, Entity* entity) : m_entity(entity), m_scene(scene) {}
            NativeScriptEntity(NativeScriptEntity const &) = delete;
            auto operator=(NativeScriptEntity const &) -> NativeScriptEntity & = delete;
            NativeScriptEntity(NativeScriptEntity const &&) = delete;
            auto operator=(NativeScriptEntity const &&) -> NativeScriptEntity & = delete;
            virtual ~NativeScriptEntity() = default;

            virtual auto onCreate() -> void{}
            virtual auto onUpdate([[maybe_unused]] float delta_time) -> void{} 
            virtual auto onDestroy() -> void{}

            auto getScene() -> Scene& {
                return *m_scene;
            }

            auto getEntity() -> Entity& {
                return *m_entity;
            }

            auto die() -> void {
                m_entity->die();
            }
            
        private:
            Entity* m_entity;
            Scene* m_scene{nullptr};
            friend class Scene;
    };

} // namespace rosa