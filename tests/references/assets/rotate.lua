persist = {}
persist["rotation"] = 0

function onCreate()
end

function onLoad()
end

function onUpdate(delta_time)
  persist["rotation"] = (persist["rotation"] + 1) % 360
  entity:getTransform():setRotation(persist["rotation"])
  print(persist["rotation"])
end

function onInput(event)
  if (event.type == EventType.Resize) then
    entity:getTransform():setPosition(event.resize.size.x / 2, event.resize.size.y / 2)
    print("Resize: " .. event.resize.size:to_string())
  end
end

function onDelete()
end
