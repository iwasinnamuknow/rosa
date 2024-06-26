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
#include <core/components/NativeScriptComponent.hpp>

// This is our NativeScript test class
class NSCTest : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCTest)
    ~NSCTest() override = default;

    void onCreate() override {
        getEntity().getComponent<rosa::SoundPlayerComponent>().play();
        getEntity().getComponent<rosa::MusicPlayerComponent>().play();
    }

    void onUpdate(float /*delta_time*/) override {
        switch (m_channel) {
            case 0:
                if (m_colour.r > 0.F) {
                    m_colour.r -= 0.05F;
                } else {
                    m_colour.r = 1.F;
                    m_channel++;
                }
                break;
            case 1:
                if (m_colour.g > 0.F) {
                    m_colour.g -= 0.05F;
                } else {
                    m_colour.g = 1.F;
                    m_channel++;
                }
                break;
            case 2:
                if (m_colour.b > 0.F) {
                    m_colour.b -= 0.05F;
                } else {
                    m_colour.b = 1.F;
                    m_channel++;
                }
                break;
            case 3:
                if (m_colour.a > 0.F) {
                    m_colour.a -= 0.05F;
                } else {
                    m_colour.a = 1.F;
                    m_channel  = 0;
                }
                break;
        }
        getEntity().getComponent<rosa::SpriteComponent>().setColour(m_colour);

        m_rotation += 0.1F;
        if (m_rotation >= 360.0F) {
            m_rotation = 0.F;
        }
        getEntity().getComponent<rosa::TransformComponent>().setRotation(m_rotation);
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
        res["colour"]   = m_colour;

        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        if (node.Type() == YAML::NodeType::Map) {
            if (node["colour"]) {
                m_colour = node["colour"].as<rosa::Colour>();
            }

            if (node["rotation"]) {
                m_rotation = node["rotation"].as<float>();
            }
        }

        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCTest"};
    }

    rosa::Colour m_colour{1.F, 1.F, 1.F, 1.F};
    float        m_rotation{0.F};
    int          m_channel{0};
};