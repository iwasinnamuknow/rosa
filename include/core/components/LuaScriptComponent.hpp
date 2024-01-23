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

#define SOL_PRINT_ERRORS 1
#define SOL_ALL_SAFETIES_ON 1

#include <memory>
#include <functional>
#include <sol/sol.hpp>
#include <core/Uuid.hpp>
#include <spdlog/spdlog.h>
#include <core/Scene.hpp>
#include <core/components/TransformComponent.hpp>
#include <core/components/SpriteComponent.hpp>

namespace rosa {

    class Scene;

    class SceneSerialiser;

    /**
     * \brief A component to interface with a Lua script
     *
     * This class handles all Lua communications. It is responsible for creating/initialising
     * any usertypes, variables etc that will be available in Lua.
     */
    struct LuaScriptComponent {
        /**
         * \brief Construct a component
         * \param scene parent scene
         * \param entity parent entity
         */
        LuaScriptComponent(Scene* scene, entt::entity entity);

        /**
         * \brief Load a script
         *
         * This function loads a script as text and passes it to the Lua engine. It also must
         * set up function callbacks, references etc. It is not a cheap function.
         *
         * \param uuid the script asset to load
         * \param deserialised true if this entity is being deserialised
         * \return true if loading was successful
         */
        auto setScript(Uuid uuid, bool deserialised = false) -> bool;

        /**
         * \brief Get the loaded script
         * \return asset Uuid
         */
        auto getScript() const -> Uuid;

        /**
         * \brief Assign an entire Lua table
         * \param key Lua object key
         * \param table source table
         */
        auto setData(const std::string& key, sol::table& table) -> void;

        /**
         * \brief Assign an integer value
         * \param key Lua object key
         * \param value source integer
         */
        auto setValue(const std::string& key, int value) -> void;

        /**
         * \brief Assign a float value
         * \param key Lua object key
         * \param value source float
         */
        auto setValue(const std::string& key, float value) -> void;

        /**
         * \brief Assign a string value
         * \param key Lua object key
         * \param value source string
         */
        auto setValue(const std::string& key, std::string value) -> void;

        /**
         * \brief Retrieve a Lua table
         * \param table Lua object key
         * \return A Lua table
         */
        auto getTable(const std::string& table) const -> sol::table;

    private:
        sol::state              m_state;
        sol::protected_function m_on_create_function;
        sol::protected_function m_on_load_function;
        sol::protected_function m_on_delete_function;
        sol::protected_function m_on_update_function;
        sol::protected_function m_on_input_function;
        Uuid                    m_uuid;

        entt::entity m_entity;
        Scene* m_scene;

        friend class Scene;

        friend class SceneSerialiser;
    };

} // namespace rosa