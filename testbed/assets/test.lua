
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

local last_rot = 0
local rotation = 0
local red = 0

function onCreate()

end

function onUpdate(delta_time)
  if last_rot > 0.02 then
    transform.setRotation(rotation)
    rotation = (rotation + 1) % 360
    sprite.setColor(red, 255, 255, 255)
    red = (red + 1) % 255
    last_rot = 0
  else
    last_rot = last_rot + delta_time
  end
  log.debug("hello from lua")
  --print(dump(transform.getPosition()))
end

function onDelete()

end
