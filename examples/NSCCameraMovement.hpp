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

#include <core/GameManager.hpp>
#include <core/NativeScriptEntity.hpp>
#include <core/components/NativeScriptComponent.hpp>
#include <core/input/Keyboard.hpp>

// This is our NativeScript test class
class NSCCameraMovement : public rosa::NativeScriptEntity {
public:
    ROSA_CONSTRUCTOR(NSCCameraMovement)
    ~NSCCameraMovement() override = default;

    void onUpdate(float delta_time) override {

        auto& transform = getEntity().getComponent<rosa::TransformComponent>();
        auto  position  = transform.getPosition();

        if (getScene().getRenderWindow().isKeyDown(rosa::Key::KeyLeft)) {
            position.x = (position.x - (1 * delta_time));
        }

        if (getScene().getRenderWindow().isKeyDown(rosa::Key::KeyRight)) {
            position.x = (position.x + (1 * delta_time));
        }

        if (getScene().getRenderWindow().isKeyDown(rosa::Key::KeyUp)) {
            position.y = (position.y + (1 * delta_time));
        }

        if (getScene().getRenderWindow().isKeyDown(rosa::Key::KeyDown)) {
            position.y = (position.y - (1 * delta_time));
        }

        transform.setPosition(position);
    }

    // We need this for deserialisation so we can re-create the class
    static rosa::NativeScriptEntity* factoryCreator(rosa::Scene* scene, rosa::Entity* entity) {
        return new NSCCameraMovement(scene, entity);
    }

protected:
    // Provide serialisation, yea you have to manually deal with yaml
    auto serialise() const -> YAML::Node override {
        YAML::Node res;
        return res;
    }

    // Deserialisation, you also have to deal with yaml here
    auto deserialise(YAML::Node node) -> void override {
        onLoad();
    }

    auto getName() const -> std::string override {
        return {"NSCCameraMovement"};
    }
};