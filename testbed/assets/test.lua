
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

end

function onUpdate(delta_time)
  if persist["last_rot"] > 0.1 then
    transform.setRotation(persist["rotation"])
    persist["rotation"] = (persist["rotation"] + 1) % 360
    sprite.setColor(persist["red"], 255, 255, 255)
    persist["red"] = (persist["red"] + 1) % 255
    persist["last_rot"] = 0
  else
    persist["last_rot"] = persist["last_rot"] + delta_time
  end
  log.debug("hello from lua")
--   print(dump(transform.getPosition()))
end

function onDelete()

end
