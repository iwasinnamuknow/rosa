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

#include <core/NativeScriptEntity.hpp>
#include <core/SerialiserTypes.hpp>

class NSCColour : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCColour)
    ~NSCColour() override = default;

    void onUpdate(float /*delta_time*/) override {
        switch (m_channel) {
            case 0:
                if (m_colour.r > 0.F) {
                    m_colour.r -= 0.25F;
                } else {
                    m_colour.r = 1.F;
                    m_channel++;
                }
                break;
            case 1:
                if (m_colour.g > 0.F) {
                    m_colour.g -= 0.25F;
                } else {
                    m_colour.g = 1.F;
                    m_channel++;
                }
                break;
            case 2:
                if (m_colour.b > 0.F) {
                    m_colour.b -= 0.25F;
                } else {
                    m_colour.b = 1.F;
                    m_channel++;
                }
                break;
            case 3:
                if (m_colour.a > 0.F) {
                    m_colour.a -= 0.25F;
                } else {
                    m_colour.a = 1.F;
                    m_channel++;
                }
                break;
        }

        getEntity().getComponent<rosa::SpriteComponent>().setColour(m_colour);
        //  if (persist["channel"] == 0) then
        //    if (persist["red"] > 0.0) then
        //      persist["red"] = persist["red"] - 0.25
        //    else
        //      persist["red"] = 1.0
        //      persist["channel"] = persist["channel"] + 1
        //    end
        //  elseif (persist["channel"] == 1) then
        //    if (persist["green"] > 0.0) then
        //      persist["green"] = persist["green"] - 0.25
        //    else
        //      persist["green"] = 1.0
        //      persist["channel"] = persist["channel"] + 1
        //    end
        //  elseif (persist["channel"] == 2) then
        //    if (persist["blue"] > 0.0) then
        //      persist["blue"] = persist["blue"] - 0.25
        //    else
        //      persist["blue"] = 1.0
        //      persist["channel"] = persist["channel"] + 1
        //    end
        //  elseif (persist["channel"] == 3) then
        //    if (persist["alpha"] > 0.0) then
        //      persist["alpha"] = persist["alpha"] - 0.25
        //    else
        //      persist["alpha"] = 1.0
        //      persist["channel"] = persist["channel"] + 1
        //    end
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCColour(scene, entity);
    }

protected:
    // Provide serialisation, yea you have to manually deal with yaml
    auto serialise() const -> YAML::Node override {

        YAML::Node res;
        res["colour"]  = m_colour;
        res["channel"] = m_channel;
        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        if (node.Type() == YAML::NodeType::Map) {
            if (node["colour"]) {
                m_colour = node["colour"].as<rosa::Colour>();
            }
            if (node["channel"]) {
                m_channel = node["channel"].as<int>();
            }
        }

        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCRotate"};
    }

    rosa::Colour m_colour{1.F, 1.F, 1.F, 1.F};
    int          m_channel{0};
};