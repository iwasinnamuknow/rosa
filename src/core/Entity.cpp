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
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>
#include <core/components/SpriteComponent.hpp>
#include <core/components/TextComponent.hpp>
#include <core/components/TransformComponent.hpp>

namespace rosa {

    auto Entity::setParent(const Uuid& parent_id) -> bool {

        assert(m_scene != nullptr);

        // no empty entities
        if (parent_id == Uuid()) {
            return false;
        }

        Entity& new_parent = m_scene->getEntity(parent_id);

        // Remove ourselves from the previous parents' children
        if (m_parent != Uuid()) {
            Entity& old_parent = m_scene->getEntity(m_parent);

            for (auto ch_it = old_parent.m_children.begin(); ch_it != old_parent.m_children.end(); ++ch_it) {
                if (*ch_it == getUuid()) {
                    old_parent.m_children.erase(ch_it);
                }
            }
        }

        // Set our new parent
        m_parent = parent_id;
        // Set us as a new child
        new_parent.m_children.push_back(getUuid());
        return true;
    }

    auto Entity::removeParent() -> bool {

        assert(m_scene != nullptr);

        if (m_parent != Uuid()) {

            Entity& old_parent = m_scene->getEntity(m_parent);

            for (auto ch_it = old_parent.m_children.begin(); ch_it != old_parent.m_children.end(); ++ch_it) {
                if (*ch_it == getUuid()) {
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
        return m_scene->getRegistry().getComponent<T>(getUuid());
    }

    template auto Entity::getComponent<TransformComponent>() -> TransformComponent&;
    template auto Entity::getComponent<CameraComponent>() -> CameraComponent&;
    template auto Entity::getComponent<NativeScriptComponent>() -> NativeScriptComponent&;
    template auto Entity::getComponent<TextComponent>() -> TextComponent&;
    template auto Entity::getComponent<MusicPlayerComponent>() -> MusicPlayerComponent&;
    template auto Entity::getComponent<SoundPlayerComponent>() -> SoundPlayerComponent&;
    template auto Entity::getComponent<SpriteComponent>() -> SpriteComponent&;

    template<typename T>
    auto Entity::hasComponent() -> bool {
        assert(m_scene != nullptr);
        return m_scene->getRegistry().hasComponent<T>(getUuid());
    }

    template auto Entity::hasComponent<TransformComponent>() -> bool;
    template auto Entity::hasComponent<CameraComponent>() -> bool;
    template auto Entity::hasComponent<NativeScriptComponent>() -> bool;
    template auto Entity::hasComponent<TextComponent>() -> bool;
    template auto Entity::hasComponent<MusicPlayerComponent>() -> bool;
    template auto Entity::hasComponent<SoundPlayerComponent>() -> bool;
    template auto Entity::hasComponent<SpriteComponent>() -> bool;

    template<typename T>
    auto Entity::addComponent() -> T& {
        assert(!hasComponent<T>());
        return m_scene->getRegistry().addComponent<T>(getUuid());
    }

    template auto Entity::addComponent<TransformComponent>() -> TransformComponent&;
    template auto Entity::addComponent<CameraComponent>() -> CameraComponent&;
    template auto Entity::addComponent<NativeScriptComponent>() -> NativeScriptComponent&;
    template auto Entity::addComponent<TextComponent>() -> TextComponent&;
    template auto Entity::addComponent<MusicPlayerComponent>() -> MusicPlayerComponent&;
    template auto Entity::addComponent<SoundPlayerComponent>() -> SoundPlayerComponent&;
    template auto Entity::addComponent<SpriteComponent>() -> SpriteComponent&;

    template<typename T>
    auto Entity::addComponent(T& data) -> T& {
        assert(!hasComponent<T>());
        return m_scene->getRegistry().addComponent<T>(getUuid(), data);
    }

    template auto Entity::addComponent<TransformComponent>(TransformComponent& data) -> TransformComponent&;
    template auto Entity::addComponent<CameraComponent>(CameraComponent& data) -> CameraComponent&;
    template auto Entity::addComponent<NativeScriptComponent>(NativeScriptComponent& data) -> NativeScriptComponent&;
    template auto Entity::addComponent<TextComponent>(TextComponent& data) -> TextComponent&;
    template auto Entity::addComponent<MusicPlayerComponent>(MusicPlayerComponent& data) -> MusicPlayerComponent&;
    template auto Entity::addComponent<SoundPlayerComponent>(SoundPlayerComponent& data) -> SoundPlayerComponent&;
    template auto Entity::addComponent<SpriteComponent>(SpriteComponent& data) -> SpriteComponent&;

    template<typename T>
    auto Entity::removeComponent() -> bool {
        if (hasComponent<T>()) {
            m_scene->getRegistry().removeComponent<T>(getUuid());
            return true;
        }

        return false;
    }

    template auto Entity::removeComponent<TransformComponent>() -> bool;
    template auto Entity::removeComponent<CameraComponent>() -> bool;
    template auto Entity::removeComponent<NativeScriptComponent>() -> bool;
    template auto Entity::removeComponent<TextComponent>() -> bool;
    template auto Entity::removeComponent<MusicPlayerComponent>() -> bool;
    template auto Entity::removeComponent<SoundPlayerComponent>() -> bool;
    template auto Entity::removeComponent<SpriteComponent>() -> bool;

} // namespace rosa