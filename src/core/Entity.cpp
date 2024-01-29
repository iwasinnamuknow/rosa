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

#include <core/Entity.hpp>
#include <core/GameManager.hpp>
#include <core/Scene.hpp>
#include <core/components/CameraComponent.hpp>
#include <core/components/LuaScriptComponent.hpp>
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TextComponent.hpp>
#include <core/components/TransformComponent.hpp>

namespace rosa {

    auto Entity::setParent(Uuid parent_id) -> bool {

        // no empty entities
        if (parent_id == Uuid()) {
            return false;
        }

        Entity& new_parent = m_scene.getEntity(parent_id);

        // Remove ourself from the previous parents' children
        if (m_parent != Uuid()) {
            Entity& old_parent = m_scene.getEntity(m_parent);

            for (auto ch_it = old_parent.m_children.begin(); ch_it != old_parent.m_children.end(); ++ch_it) {
                if (*ch_it == m_uuid) {
                    old_parent.m_children.erase(ch_it);
                }
            }
        }

        // Set our new parent
        m_parent = parent_id;
        // Set us as a new child
        new_parent.m_children.push_back(m_uuid);
        return true;
    }

    auto Entity::removeParent() -> bool {
        if (m_parent != Uuid()) {

            Entity& old_parent = m_scene.getEntity(m_parent);

            for (auto ch_it = old_parent.m_children.begin(); ch_it != old_parent.m_children.end(); ++ch_it) {
                if (*ch_it == m_uuid) {
                    old_parent.m_children.erase(ch_it);
                }
            }

            m_parent = Uuid();
            return true;
        }

        return false;
    }

    template<typename T>
    auto Entity::getComponent() -> T& {
        assert(hasComponent<T>());
        return m_scene.getRegistry().get<T>(m_id);
    }

    template auto Entity::getComponent<TransformComponent>() -> TransformComponent&;
    template auto Entity::getComponent<CameraComponent>() -> CameraComponent&;
    template auto Entity::getComponent<LuaScriptComponent>() -> LuaScriptComponent&;
    template auto Entity::getComponent<NativeScriptComponent>() -> NativeScriptComponent&;
    template auto Entity::getComponent<TextComponent>() -> TextComponent&;
    template auto Entity::getComponent<MusicPlayerComponent>() -> MusicPlayerComponent&;
    template auto Entity::getComponent<SoundPlayerComponent>() -> SoundPlayerComponent&;
    template auto Entity::getComponent<SpriteComponent>() -> SpriteComponent&;

    template<typename T>
    auto Entity::hasComponent() -> bool {
        return m_scene.getRegistry().any_of<T>(m_id);
    }

    template auto Entity::hasComponent<TransformComponent>() -> bool;
    template auto Entity::hasComponent<CameraComponent>() -> bool;
    template auto Entity::hasComponent<LuaScriptComponent>() -> bool;
    template auto Entity::hasComponent<NativeScriptComponent>() -> bool;
    template auto Entity::hasComponent<TextComponent>() -> bool;
    template auto Entity::hasComponent<MusicPlayerComponent>() -> bool;
    template auto Entity::hasComponent<SoundPlayerComponent>() -> bool;
    template auto Entity::hasComponent<SpriteComponent>() -> bool;

    template<typename T>
    auto Entity::addComponent() -> T& {
        assert(!hasComponent<T>());
        return m_scene.getRegistry().emplace<T>(m_id);
    }

    template auto Entity::addComponent<TransformComponent>() -> TransformComponent&;
    template auto Entity::addComponent<CameraComponent>() -> CameraComponent&;
    template auto Entity::addComponent<NativeScriptComponent>() -> NativeScriptComponent&;
    template auto Entity::addComponent<TextComponent>() -> TextComponent&;
    template auto Entity::addComponent<MusicPlayerComponent>() -> MusicPlayerComponent&;
    template auto Entity::addComponent<SoundPlayerComponent>() -> SoundPlayerComponent&;
    template auto Entity::addComponent<SpriteComponent>() -> SpriteComponent&;

    template<typename T>
    auto Entity::addComponent(Scene* scene, entt::entity entity) -> T& {
        assert(!hasComponent<T>());
        return m_scene.getRegistry().emplace<T>(m_id, std::forward<Scene*>(scene), std::forward<entt::entity>(entity));
    }

    template auto Entity::addComponent<LuaScriptComponent>(Scene* scene, entt::entity entity) -> LuaScriptComponent&;

    /**
         * \brief Remove a specific component type from the Entity
         * \tparam T Component type
         * \return true if the component was removed, false otherwise
         */
    template<typename T>
    auto Entity::removeComponent() -> bool {
        assert(hasComponent<T>());
        return m_scene.getRegistry().remove<T>(m_id);
    }

    template auto Entity::removeComponent<TransformComponent>() -> bool;
    template auto Entity::removeComponent<CameraComponent>() -> bool;
    template auto Entity::removeComponent<LuaScriptComponent>() -> bool;
    template auto Entity::removeComponent<NativeScriptComponent>() -> bool;
    template auto Entity::removeComponent<TextComponent>() -> bool;
    template auto Entity::removeComponent<MusicPlayerComponent>() -> bool;
    template auto Entity::removeComponent<SoundPlayerComponent>() -> bool;
    template auto Entity::removeComponent<SpriteComponent>() -> bool;

} // namespace rosa