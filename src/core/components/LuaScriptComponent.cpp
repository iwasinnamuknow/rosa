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
 *  You should have received a copy of the GNU General Public License along with bbai. If not,
 *  see <https://www.gnu.org/licenses/>.
 */

#include <core/components/LuaScriptComponent.hpp>

#include <graphics/Vertex.hpp>
#include <core/Event.hpp>
#include <core/LuaScript.hpp>

#include <sstream>

namespace rosa {

    auto init_events(sol::state& state) -> void {
        state["EventType"] = state.create_table_with( 
            "Keyboard", EventType::EventKeyboard, 
            "Mouse", EventType::EventMouse,
            "Resize", EventType::EventResize
        );

        state["EventKeyboardType"] = state.create_table_with( 
            "KeyPressed", KeyboardEventType::KeyPressed, 
            "KeyReleased", KeyboardEventType::KeyReleased,
            "KeyRepeated", KeyboardEventType::KeyRepeated
        );

        state["EventKeyboardMods"] = state.create_table_with( 
            "CapsLock", KeyboardModifier::ModifierCapsLock, 
            "NumLock", KeyboardModifier::ModifierNumLock,
            "Alt", KeyboardModifier::ModifierKeyAlt,
            "Control", KeyboardModifier::ModifierKeyControl,
            "None", KeyboardModifier::ModifierKeyNone,
            "Shift", KeyboardModifier::ModifierKeyShift,
            "Super", KeyboardModifier::ModifierSuper
        );

        state["EventKeyboardKey"] = state.create_table_with( 
            "Space", Key::KeySpace,
            "KeyApostrophe", Key::KeyApostrophe,
            "KeyComma", Key::KeyComma,
            "KeyMinus", Key::KeyMinus,
            "KeyPeriod", Key::KeyPeriod,
            "KeySlash", Key::KeySlash,
            "Key0", Key::Key0,
            "Key1", Key::Key1,
            "Key2", Key::Key2,
            "Key3", Key::Key3,
            "Key4", Key::Key4,
            "Key5", Key::Key5,
            "Key6", Key::Key6,
            "Key7", Key::Key7,
            "Key8", Key::Key8,
            "Key9", Key::Key9,
            "KeySemicolon", Key::KeySemicolon,
            "KeyEquals", Key::KeyEquals,
            "KeyA", Key::KeyA,
            "KeyB", Key::KeyB,
            "KeyC", Key::KeyC,
            "KeyD", Key::KeyD,
            "KeyE", Key::KeyE,
            "KeyF", Key::KeyF,
            "KeyG", Key::KeyG,
            "KeyH", Key::KeyH,
            "KeyI", Key::KeyI,
            "KeyJ", Key::KeyJ,
            "KeyK", Key::KeyK,
            "KeyL", Key::KeyL,
            "KeyM", Key::KeyM,
            "KeyN", Key::KeyN,
            "KeyO", Key::KeyO,
            "KeyP", Key::KeyP,
            "KeyQ", Key::KeyQ,
            "KeyR", Key::KeyR,
            "KeyS", Key::KeyS,
            "KeyT", Key::KeyT,
            "KeyU", Key::KeyU,
            "KeyV", Key::KeyV,
            "KeyW", Key::KeyW,
            "KeyX", Key::KeyX,
            "KeyY", Key::KeyY,
            "KeyZ", Key::KeyZ,
            "KeyLeftBracket", Key::KeyLeftBracket,
            "KeyBackslash", Key::KeyBackslash,
            "KeyRightBracket", Key::KeyRightBracket,
            "KeyGraveAccent", Key::KeyGraveAccent,
            "KeyEscape", Key::KeyEscape,
            "KeyEnter", Key::KeyEnter,
            "KeyTab", Key::KeyTab,
            "KeyBackspace", Key::KeyBackspace,
            "KeyInsert", Key::KeyInsert,
            "KeyDelete", Key::KeyDelete,
            "KeyRight", Key::KeyRight,
            "KeyLeft", Key::KeyLeft,
            "KeyDown", Key::KeyDown,
            "KeyUp", Key::KeyUp,
            "KeyPageUp", Key::KeyPageUp,
            "KeyPageDown", Key::KeyPageDown,
            "KeyHome", Key::KeyHome,
            "KeyEnd", Key::KeyEnd,
            "KeyCapsLock", Key::KeyCapsLock,
            "KeyScrollLock", Key::KeyScrollLock,
            "KeyNumLock", Key::KeyNumLock,
            "KeyPrintScreen", Key::KeyPrintScreen,
            "KeyPause", Key::KeyPause,
            "KeyF1", Key::KeyF1,
            "KeyF2", Key::KeyF2,
            "KeyF3", Key::KeyF3,
            "KeyF4", Key::KeyF4,
            "KeyF5", Key::KeyF5,
            "KeyF6", Key::KeyF6,
            "KeyF7", Key::KeyF7,
            "KeyF8", Key::KeyF8,
            "KeyF9", Key::KeyF9,
            "KeyF10", Key::KeyF10,
            "KeyF11", Key::KeyF11,
            "KeyF12", Key::KeyF12,
            "KeyF13", Key::KeyF13,
            "KeyF14", Key::KeyF14,
            "KeyF15", Key::KeyF15,
            "KeyF16", Key::KeyF16,
            "KeyF17", Key::KeyF17,
            "KeyF18", Key::KeyF18,
            "KeyF19", Key::KeyF19,
            "KeyF20", Key::KeyF20,
            "KeyF21", Key::KeyF21,
            "KeyF22", Key::KeyF22,
            "KeyF23", Key::KeyF23,
            "KeyF24", Key::KeyF24,
            "KeyF25", Key::KeyF25,
            "KeyKp0", Key::KeyKp0,
            "KeyKp1", Key::KeyKp1,
            "KeyKp2", Key::KeyKp2,
            "KeyKp3", Key::KeyKp3,
            "KeyKp4", Key::KeyKp4,
            "KeyKp5", Key::KeyKp5,
            "KeyKp6", Key::KeyKp6,
            "KeyKp7", Key::KeyKp7,
            "KeyKp8", Key::KeyKp8,
            "KeyKp9", Key::KeyKp9,
            "KeyKpDecimal", Key::KeyKpDecimal,
            "KeyKpDivide", Key::KeyKpDivide,
            "KeyKpMultiply", Key::KeyKpMultiply,
            "KeyKpSubtract", Key::KeyKpSubtract,
            "KeyKpAdd", Key::KeyKpAdd,
            "KeyKpEnter", Key::KeyKpEnter,
            "KeyKpEquals", Key::KeyKpEquals,
            "KeyLeftShift", Key::KeyLeftShift,
            "KeyLeftControl", Key::KeyLeftControl,
            "KeyLeftAlt", Key::KeyLeftAlt,
            "KeyLeftSuper", Key::KeyLeftSuper,
            "KeyRightShift", Key::KeyRightShift,
            "KeyRightControl", Key::KeyRightControl,
            "KeyRightAlt", Key::KeyRightAlt,
            "KeyRightSuper", Key::KeyRightSuper,
            "KeyMenu", Key::KeyMenu
        );

        state["EventMouseButton"] = state.create_table_with( 
            "ButtonLeft", MouseButton::MouseButtonLeft,
            "ButtonRight", MouseButton::MouseButtonRight,
            "ButtonMiddle", MouseButton::MouseButtonMiddle
        );

        state["EventMouseType"] = state.create_table_with(
            "ButtonPressed", MouseEventType::MouseButtonPressed,
            "ButtonReleased", MouseEventType::MouseButtonReleased,
            "MouseMoved", MouseEventType::MouseMoved,
            "MouseScrolled", MouseEventType::MouseScrolled
        );

        state.new_usertype<rosa::Event>("event",
            "type", &rosa::Event::type,
            "keyboard", &rosa::Event::keyboard,
            "mouse", &rosa::Event::mouse,
            "resize", &rosa::Event::resize
        );

        state.new_usertype<rosa::KeyboardEvent>("keyboard",
            "type", &rosa::KeyboardEvent::type,
            "key", &rosa::KeyboardEvent::key,
            "modifiers", &rosa::KeyboardEvent::modifiers
        );

        state.new_usertype<rosa::MouseEvent>("mouse",
            "type", &rosa::MouseEvent::type,
            "button", &rosa::MouseEvent::button,
            "position", &rosa::MouseEvent::position
        );

        state.new_usertype<rosa::ResizeEvent>("resize",
            "size", &rosa::ResizeEvent::size
        );
    }

    auto init_types(sol::state& state) -> void {
        state.new_usertype<glm::vec2>("Vec2",
            sol::constructors<glm::vec2(float, float)>(),
            "x", &glm::vec2::x,
            "y", &glm::vec2::y,
            "to_string", [](glm::vec2 vec) {
                std::stringstream pvalue{};
                pvalue << vec.x << ", " << vec.y;
                return pvalue.str();
            },
            "distance_to", [](glm::vec2 a, glm::vec2 b) {
                return  glm::distance(a, b);
            }
        );

        state.new_usertype<glm::vec3>("Vec3",
            sol::constructors<glm::vec3(float, float, float)>(),
            "x", &glm::vec3::x,
            "y", &glm::vec3::y,
            "z", &glm::vec3::z,
            "to_string", [](glm::vec3 vec) {
                std::stringstream pvalue{};
                pvalue << vec.x << ", " << vec.y << ", " << vec.z;
                return pvalue.str();
            },
            "distance_to", [](glm::vec3 a, glm::vec3 b) {
                return  glm::distance(a, b);
            }
        );

        state.new_usertype<glm::vec4>("Vec4",
            sol::constructors<glm::vec4(float, float, float, float)>(),
            "x", &glm::vec4::x,
            "y", &glm::vec4::y,
            "z", &glm::vec4::z,
            "w", &glm::vec4::w,
            "to_string", [](glm::vec4 vec) {
                std::stringstream pvalue{};
                pvalue << vec.x << ", " << vec.y << ", " << vec.z << ", " << vec.w;
                return pvalue.str();
            },
            "distance_to", [](glm::vec4 a, glm::vec4 b) {
                return  glm::distance(a, b);
            }
        );

        state.new_usertype<rosa::Colour>("Colour",
            sol::constructors<rosa::Colour()>(),
            sol::constructors<rosa::Colour(uint8_t, uint8_t, uint8_t)>(),
            sol::constructors<rosa::Colour(uint8_t, uint8_t, uint8_t, uint8_t)>(),
            "r", &rosa::Colour::r,
            "g", &rosa::Colour::g,
            "b", &rosa::Colour::b,
            "a", &rosa::Colour::a,
            "to_string", [](rosa::Colour colour) {
                std::stringstream pvalue{};
                pvalue << colour.r << ", " << colour.g << ", " << colour.b << ", " << colour.a;
                return pvalue.str();
            }
        );

        state.new_usertype<rosa::Vertex>("Vertex",
            sol::constructors<rosa::Vertex(glm::vec2)>(),
            sol::constructors<rosa::Vertex(glm::vec2, Colour)>(),
            sol::constructors<rosa::Vertex(glm::vec2, Colour, glm::vec2)>(),
            "position", &rosa::Vertex::position,
            "colour",   &rosa::Vertex::colour,
            "uv",       &rosa::Vertex::texture_coords
        );

        state.new_usertype<rosa::Rect>("Rect",
            sol::constructors<rosa::Rect(glm::vec2, glm::vec2)>(),
            "position", &rosa::Rect::position,
            "size",     &rosa::Rect::size
        );

        state.new_usertype<rosa::Entity>("Entity",
            "getTransform", &rosa::Entity::getComponent<TransformComponent>,
            "getSprite",    &rosa::Entity::getComponent<SpriteComponent>
        );

        state.new_usertype<rosa::TransformComponent>("Transform",
            "setPosition", &rosa::TransformComponent::setPosition,
            "getPosition", &rosa::TransformComponent::getPosition,
            "setRotation", &rosa::TransformComponent::setRotation,
            "getRotation", &rosa::TransformComponent::getRotation,
            "getScale",    &rosa::TransformComponent::getScale,
            "setScale",    &rosa::TransformComponent::setScale
        );

        state.new_usertype<rosa::SpriteComponent>("Sprite",
            "getTextureSize",   &rosa::SpriteComponent::getSize,
            "setTexture",       &rosa::SpriteComponent::setTexture,
            "getColour",        &rosa::SpriteComponent::getColour,
            "setColour",        &rosa::SpriteComponent::setColour,
            "getSize",          &rosa::SpriteComponent::getSize,
            "setTextureRect",   sol::overload(
                static_cast<void (rosa::SpriteComponent::*) (glm::vec2, glm::vec2 )>(&rosa::SpriteComponent::setTextureRect),
                static_cast<void (rosa::SpriteComponent::*) (rosa::Rect           )>(&rosa::SpriteComponent::setTextureRect)
            ),
            "getTextureRect",   &rosa::SpriteComponent::getTextureRect
        );
    }

    LuaScriptComponent::LuaScriptComponent(Scene* scene, entt::entity entity) : m_entity(entity), m_scene(scene) {
        ROSA_PROFILE_SCOPE("LuaScriptComponent:Initialise");

        m_state.open_libraries(sol::lib::base);
    }

    auto LuaScriptComponent::setScript(uuids::uuid uuid, bool deserialised) -> bool {
        ROSA_PROFILE_SCOPE("LuaScriptComponent:setScript");

        try {
            const auto& script = ResourceManager::instance().getAsset<LuaScript>(uuid);
            auto& entity = m_scene->getEntity(m_entity);

            auto result = m_state.script(script.getContent(), &sol::script_default_on_error);

            if(result.valid()) {
                m_on_create_function = m_state["onCreate"];
                m_on_delete_function = m_state["onDelete"];
                m_on_update_function = m_state["onUpdate"];
                m_on_input_function =  m_state["onInput"];
                m_on_load_function =  m_state["onLoad"];

                m_uuid = uuid;

                init_events(m_state);
                init_types(m_state);
                
                // logger
                auto log_table = m_state["log"].get_or_create<sol::table>();
                log_table.set_function("error", [](const std::string& message) {
                    spdlog::error(message);
                });
                log_table.set_function("warning", [](const std::string& message) {
                    spdlog::warn(message);
                });
                log_table.set_function("info", [](const std::string& message) {
                    spdlog::info(message);
                });
                log_table.set_function("debug", [](const std::string& message) {
                    spdlog::debug(message);
                });

                // Transform component
                m_state["transform"] = &entity.getComponent<TransformComponent>();

                // Sprite component
                if (entity.hasComponent<SpriteComponent>()) {
                    m_state["sprite"] = &entity.getComponent<SpriteComponent>();
                }

                // Sound player component
                if (entity.hasComponent<SoundPlayerComponent>()) {
                    m_state["sound"] = &entity.getComponent<SoundPlayerComponent>();
                }

                // Music player component
                if (entity.hasComponent<MusicPlayerComponent>()) {
                    m_state["music"] = &entity.getComponent<MusicPlayerComponent>();
                }

                // Call the lua initialiser
                if (deserialised) { 
                    m_on_load_function();
                } else {
                    m_on_create_function();
                }

                return true;
            }

        } catch (sol::error& e) {
            spdlog::error("Couldn't load lua script: {}", e.what());
        }
        
        return false;
    }

    auto LuaScriptComponent::set_data(const std::string& key, sol::table& table) -> void {
        m_state.set(key, table);
    }

} // namespace rosa