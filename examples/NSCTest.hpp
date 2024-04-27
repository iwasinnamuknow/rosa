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
#include <core/components/MusicPlayerComponent.hpp>
#include <core/components/SoundPlayerComponent.hpp>

#pragma once

class NSCTest : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCTest)
    ~NSCTest() override = default;

    void onCreate() override {
        getEntity().getComponent<rosa::SoundPlayerComponent>().play();
        getEntity().getComponent<rosa::MusicPlayerComponent>().play();
    }

    void onUpdate(float /*delta_time*/) override {
        m_rotation += 0.1F;
        if (m_rotation >= 360.0F) {
            m_rotation = 0.F;
        }
        auto& transform = getEntity().getComponent<rosa::TransformComponent>();
        transform.setRotation(m_rotation);
        //        persist["rotation"] = (persist["rotation"] + 0.1) % 360
        //                              entity:getTransform():setRotation(persist["rotation"])
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCTest(scene, entity);
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
    }

    auto getName() const -> std::string override {
        return {"NSCTest"};
    }

    float m_rotation{0.F};
};