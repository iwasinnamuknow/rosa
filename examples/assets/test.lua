
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
persist["last_rot"] = 0
persist["rotation"] = 0
persist["red"] = 0

general = 2

function onCreate()
  print("Created!")
  sound.play()
  music.play()
end

function onLoad()
  print("loaded!")
end

function onUpdate(delta_time)
  if persist["last_rot"] > 0.1 then
    transform.setRotation(persist["rotation"])
    persist["rotation"] = (persist["rotation"] + 0.2) % 360
    sprite.setColour(persist["red"], 255, 255, 255)
    persist["red"] = (persist["red"] + 5) % 255
    persist["last_rot"] = 0
    log.debug("Red: " .. tostring(persist["red"]))
  else
    persist["last_rot"] = persist["last_rot"] + delta_time
  end
  --log.debug("hello from lua")
--   print(dump(transform.getPosition()))
end

function onInput(event)
  if (event.type == EventType.Mouse) then
    if (event.mouse.type == EventMouseType.MouseMoved) then
      --print(event.mouse.position.x .. ", " .. event.mouse.position.y)
      --print(event.mouse.position:to_string())

      --print(event.mouse.position == vec2.new(100.0, 100.0))
    end
  elseif (event.type == EventType.Resize) then
    print(event.resize.size.x)
    print(event.resize.size.y)
    transform.setPosition(event.resize.size.x / 2, event.resize.size.y / 2)
  end
end

function onDelete()

end
