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
        spdlog::info("NativeScript Test script initialised");
    }

    void onLoad() override {
        spdlog::info("NativeScript Test script loaded");
    }

    void onUpdate(float /*delta_time*/) override {
        m_test_int++;
        m_test_string = "test_string" + std::to_string(m_test_int);
    }

    void onDestroy() override {
        spdlog::info("NativeScript Dieing!");
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCTest(scene, entity);
    }

protected:
    // Provide serialisation, yea you have to manually deal with yaml
    auto serialise() const -> YAML::Node override {

        YAML::Node res;

        res["test_int"]    = m_test_int;
        res["test_string"] = m_test_string;

        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        if (node.Type() == YAML::NodeType::Map) {
            if (node["test_int"]) {
                m_test_int = node["test_int"].as<int>();
            }

            if (node["test_string"]) {
                m_test_string = node["test_string"].as<std::string>();
            }
        }

        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCTest"};
    }

    int         m_test_int{1234};
    std::string m_test_string{"testing1234"};
};