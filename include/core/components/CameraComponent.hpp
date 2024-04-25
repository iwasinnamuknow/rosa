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

#include <glm/vec2.hpp>

namespace rosa {

    /**
     * \brief Component to represent a camera viewpoint
     *
     * This component doesn't do anything clever, it merely indicates to the scene
     * that the parent entity transform will be used as the viewpoint if the camera
     * is enabled.
     */
    class CameraComponent {
    public:
        CameraComponent() = default;

        /**
         * \brief Set enable state of the camera
         * \param enable true for enable, false for disable
         *
         * When using multiple cameras, it is up to the user to disable a camera
         * when enabling another. If multiple cameras are enabled, the last processed
         * will be used.
         */
        auto setEnabled(bool enable) -> void;

        /**
         * \brief Get the enable state of the camera
         * \return true for enabled, false for disabled
         */
        auto getEnabled() const -> bool;

    private:
        bool m_enabled{false};
    };

    auto operator<<(YAML::Emitter& out, const CameraComponent& component) -> YAML::Emitter&;

}// namespace rosa

namespace YAML {
    template<>
    struct convert<rosa::CameraComponent> {
        static auto decode(const Node& node, rosa::CameraComponent& rhs) -> bool {
            if (!node.IsMap()) {
                return false;
            }

            rhs.setEnabled(node["enabled"].as<bool>());
            return true;
        }

        static auto encode(const rosa::CameraComponent& rhs) -> Node {
            Node node;
            node["enabled"] = rhs.getEnabled();
            return node;
        }
    };
}// namespace YAML