
function onCreate()
end

function onLoad()
end

function onUpdate(delta_time)
    transform = entity:getTransform()
    pos = transform:getPosition()

    if (isKeyDown(EventKeyboardKey.KeyLeft)) then
        pos.x = pos.x + (80 * delta_time)
    end

    if (isKeyDown(EventKeyboardKey.KeyRight)) then
        pos.x = pos.x - (80 * delta_time)
    end

    if (isKeyDown(EventKeyboardKey.KeyUp)) then
        pos.y = pos.y - (80 * delta_time)
    end

    if (isKeyDown(EventKeyboardKey.KeyDown)) then
        pos.y = pos.y + (80 * delta_time)
    end
    
    transform:setPosition(pos);

    --print(transform:getPosition():to_string())
end

function onInput(event)
end

function onDelete()
end