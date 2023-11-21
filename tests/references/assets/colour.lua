persist = {}
persist["red"] = 1.0
persist["green"] = 1.0
persist["blue"] = 1.0
persist["alpha"] = 1.0
persist["channel"] = 0

function onCreate()
end

function onLoad()
end

function onUpdate(delta_time)

  if (persist["channel"] == 0) then
    if (persist["red"] > 0.0) then
      persist["red"] = persist["red"] - 0.25
    else
      persist["red"] = 1.0
      persist["channel"] = persist["channel"] + 1
    end
  elseif (persist["channel"] == 1) then
    if (persist["green"] > 0.0) then
      persist["green"] = persist["green"] - 0.25
    else
      persist["green"] = 1.0
      persist["channel"] = persist["channel"] + 1
    end
  elseif (persist["channel"] == 2) then
    if (persist["blue"] > 0.0) then
      persist["blue"] = persist["blue"] - 0.25
    else
      persist["blue"] = 1.0
      persist["channel"] = persist["channel"] + 1
    end
  elseif (persist["channel"] == 3) then
    if (persist["alpha"] > 0.0) then
      persist["alpha"] = persist["alpha"] - 0.25
    else
      persist["alpha"] = 1.0
      persist["channel"] = persist["channel"] + 1
    end
  end

  local colour = Colour.new(persist["red"], persist["green"], persist["blue"], persist["alpha"])
  entity:getSprite():setColour(colour)
  --print(colour:to_string())
end

function onInput(event)
  if (event.type == EventType.Resize) then
    local size = entity:getSprite():getSize()
    entity:getTransform():setPosition((event.resize.size.x / 2), (event.resize.size.y / 2))
    --print("Resize: " .. event.resize.size:to_string())
  end
end

function onDelete()
end
