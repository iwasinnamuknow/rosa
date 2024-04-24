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

#include <core/NativeScriptEntity.hpp>

#pragma once

class NSCRotate : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCRotate)
    ~NSCRotate() override = default;

    void onUpdate(float /*delta_time*/) override {
        m_rotation += 0.1F;
        if (m_rotation >= 360.0F) {
            m_rotation = 0.F;
        }
        getEntity().getComponent<rosa::TransformComponent>().setRotation(m_rotation);
        //        persist["rotation"] = (persist["rotation"] + 0.1) % 360
        //                              entity:getTransform():setRotation(persist["rotation"])
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCRotate(scene, entity);
    }

protected:
    // Provide serialisation, yea you have to manually deal with yaml
    auto serialise() const -> YAML::Node override {

        YAML::Node res;
        res["rotation"] = m_rotation;
        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        if (node.Type() == YAML::NodeType::Map) {
            if (node["rotation"]) {
                m_rotation = node["rotation"].as<float>();
            }
        }

        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCRotate"};
    }

    float m_rotation{0.F};
};