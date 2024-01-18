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

//#include <core/Scene.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TransformComponent.hpp>
#include <entt/entt.hpp>
#include <functional>
#include <spdlog/spdlog.h>
#include <core/Entity.hpp>
#include <core/Scene.hpp>
#include <yaml-cpp/yaml.h>

#define ROSA_CONSTRUCTOR(class) explicit class(std::reference_wrapper<rosa::Scene> scene, std::reference_wrapper<rosa::Entity> entity) : NativeScriptEntity(scene, entity) {}

namespace rosa {

    class SceneSerialiser;

    class NativeScriptEntity {
        public:
            explicit NativeScriptEntity(std::reference_wrapper<Scene> scene, std::reference_wrapper<Entity> entity) : m_entity(entity), m_scene(scene) {}
            NativeScriptEntity(NativeScriptEntity const &) = delete;
            auto operator=(NativeScriptEntity const &) -> NativeScriptEntity & = delete;
            NativeScriptEntity(NativeScriptEntity const &&) = delete;
            auto operator=(NativeScriptEntity const &&) -> NativeScriptEntity & = delete;
            virtual ~NativeScriptEntity() = default;

            virtual auto onCreate() -> void{}
            virtual auto onLoad() -> void{}
            virtual auto onUpdate([[maybe_unused]] float delta_time) -> void{}
            virtual auto onInput([[maybe_unused]] const Event& event) -> void{}
            virtual auto onDestroy() -> void{}

            static NativeScriptEntity* factoryCreator(Scene* scene, Entity* entity) {
                return new NativeScriptEntity(*scene, *entity);
            }

            auto getScene() -> Scene& {
                return m_scene.get();
            }

            auto getEntity() -> Entity& {
                return m_entity.get();
            }

            auto die() -> void {
                m_entity.get().die();
            }

        protected:
            virtual auto serialise() -> YAML::Node{ return {}; }
            virtual auto deserialise(YAML::Node node) -> void{}
            virtual auto getName() -> std::string{ return{}; }
            
        private:
            std::reference_wrapper<Entity> m_entity;
            std::reference_wrapper<Scene> m_scene;
            friend class Scene;
            friend class SceneSerialiser;
    };

} // namespace rosa