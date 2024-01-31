
function dump(o)
  if type(o) == 'table' then
     local s = '{ '
     for k,v in pairs(o) do
        if type(k) ~= 'number' then k = '"'..k..'"' end
        s = s .. '['..k..'] = ' .. dump(v) .. ','
     end
     return s .. '} '
  else
     return tostring(o)
  end
end

persist = {}
persist["rotation"] = 0
persist["red"] = 0.01

general = 2

function onCreate()
  print("Created!")
  entity:getSoundPlayer():play()
  entity:getMusicPlayer():play()
end

function onLoad()
  print("loaded!")
end

function onUpdate(delta_time)
    persist["rotation"] = (persist["rotation"] + 0.2) % 360
    entity:getTransform():setRotation(persist["rotation"])
    local col = Colour.new(persist["red"], 1, 1, 1)
    entity:getSprite():setColour(col)
    persist["red"] = (persist["red"] + 0.05) % 1
end

function onInput(event)
  if (event.type == EventType.Resize) then
    entity:getTransform():setPosition(event.resize.size.x / 2, event.resize.size.y / 2)
  end
end

function onDelete()

end
