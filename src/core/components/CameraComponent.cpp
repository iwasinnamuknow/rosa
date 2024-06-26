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

#include <core/components/CameraComponent.hpp>

namespace rosa {

    auto CameraComponent::setEnabled(bool enable) -> void {
        m_enabled = enable;
    }

    auto CameraComponent::getEnabled() const -> bool {
        return m_enabled;
    }

    auto operator<<(YAML::Emitter& out, const CameraComponent& component) -> YAML::Emitter& {
        out << YAML::BeginMap;
        out << YAML::Key << "type" << YAML::Value << "camera";
        out << YAML::Key << "enabled" << YAML::Value << component.getEnabled();
        out << YAML::EndMap;
        return out;
    }

} // namespace rosa