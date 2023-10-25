# What is Rosa?

Rosa is my attempt to write a fairly simple, code-only 2d game engine. It is built on OpenGL and is my first attempt to use OpenGL directly. This is also my second attempt at a game engine. It has many bad points but trust me, it's an improvement on the previous one. The feature list currently looks like:

* Linux support only currently.
* 2d OpenGL 3.3 renderer supporting sprite quads
    * Default or user-supplied vertex/fragment shaders
    * DDS (DX1 DX3 DX5) texture support
    * Full movement, scaling and rotation
* ECS based on entt
    * Transform component
    * Sprite component
    * Script components
* NativeScript - Write component scripts in C++
    * Access to the entire language and engine
* LuaScript - Write component scripts in Lua
    * Transform component access
    * Sprite component access
* Packaged asset format using PhysFS - assets are packaged in archives including:
    * ZIP
    * 7z
    * PAK
* UUID represenation of all assets and entities
    * Use assets via UUID
    * Asset UUIDs based on archive manifest (assets.lst)
* Event queue on top of OpenGL supporting
    * Mouse movement
    * Mouse button presses
    * Mouse scrolling
    * Keypress/release
    * Window resize
    * Window close
* Scene serialisation/deserialisation
    * Using UUIDs for all indices for portability
    * Supports all three component types but no support for serialising NativeScript yet.

I hope to keep extending Rosa, particularly LuaScript support and general code quality/architecture. Additional platforms will also be added hopefully, starting with Windows.

# Building

## vcpkg

`./vendor/vcpkg/bootstrap-vcpkg.sh`

## Rosa

### Configure

`cmake --preset={preset} -B build/`

Where **{preset}** is one of:
* linux-debug
* linux-debug-profile (enables built-in profiling)
* linux-release

### Build

`cmake --build build/`

# Using

coming soon