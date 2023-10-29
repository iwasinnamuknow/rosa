# events

- [x] Create containers to organise opengl event data
- [x] Pass event containers to NativeScript
- [x] Pass event containers to LuaScript

```cpp
enum class Direction {
		Up,
		Right,
		Down,
		Left
}

sol::state lua;

lua["Direction"] = lua.create_table_with( 
		"Up", Direction::Up, 
		"Down", Direction::Down, 
		"Left", Direction::Left, 
		"Right", Direction::Right 
);
```

- [ ] Extend event system for more types (enter/leave etc)

# Ser/Deser

- [ ] Validate existing deser
- [ ] Clean up LuaScript variables

	Search global table but for what criteria? prefix (rosa_*)?

```cpp
for (auto it = m_state.globals().begin(); it != m_state.globals().end(); ++it) {
	auto [key, val] = *it;
	
	auto type = val.get_type();
	if (type == sol::type::number) {
		spdlog::debug("{} {}", key.as<std::string>(), val.as<double>());
	
	} else if (type == sol::type::boolean) {
		spdlog::debug("{} {}", key.as<std::string>(), val.as<bool>());
	
	} else if (type == sol::type::string) {
		spdlog::debug("{} {}", key.as<std::string>(), val.as<std::string>());
	}
	//assert(key);
}
```

- [ ] Find a way to ser/deser NativeScript variables

# LuaScript

- [x] Add (set)texture by uuid
- [ ] Add Native/Lua interop?
- [ ] Add component support?
- [x] Represent glm::vec2/3/4

# Profiling

- [ ] Add better ImGui profiler output. Maybe some kind of histogram
- [ ] Improve profiler output file format. Perhaps the one cherno used?

# Core

- [ ] Generic timer
- [ ] Slots
- [x] Hierarchical transforms

# Quality

- [x] Warnings pass
- [ ] Figure out how to write some unit tests
- [ ] Add doxygen comments for public interfaces