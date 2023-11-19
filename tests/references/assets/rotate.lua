persist = {}
persist["last_rot"] = 0
persist["rotation"] = 0

function onCreate()
end

function onLoad()
end

function onUpdate(delta_time)
  if persist["last_rot"] > 0.1 then
    entity:getTransform():setRotation(persist["rotation"])
    persist["rotation"] = (persist["rotation"] + 5) % 360
    persist["last_rot"] = 0
    print(persist["rotation"])
  else
    persist["last_rot"] = persist["last_rot"] + delta_time
  end
end

function onInput(event)
  if (event.type == EventType.Resize) then
    local size = entity:getSprite():getSize()
    entity:getTransform():setPosition((event.resize.size.x / 2) - (size.x / 2), (event.resize.size.y / 2) - (size.y / 2))
    --print("Resize: " .. event.resize.size:to_string())
  end
end

function onDelete()
end
