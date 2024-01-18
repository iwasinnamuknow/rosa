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

#include "core/components/LuaScriptComponent.hpp"
#include "core/components/NativeScriptComponent.hpp"
#include "core/components/SpriteComponent.hpp"
#include "core/components/TransformComponent.hpp"
#include <core/Entity.hpp>
#include <core/GameManager.hpp>

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

} // namespace rosa