
digits_uuid = Uuid.new("409b5f84-4060-0a2e-9227-04056ac17ffb")
alarm_uuid  = Uuid.new("47185140-8a1b-9d46-3606-402ed23bcf83")

char_width = 64
char_height = 128

total_width = 0
character_order = {}
finished = false

characters = {
    ["0"] = Rect.new( Vec2.new(char_width * 0, char_height * 0),         Vec2.new(char_width, char_height)),
    ["1"] = Rect.new( Vec2.new(char_width * 1, char_height * 0),         Vec2.new(char_width, char_height)),
    ["2"] = Rect.new( Vec2.new(char_width * 2, char_height * 0),         Vec2.new(char_width, char_height)),
    ["3"] = Rect.new( Vec2.new(char_width * 3, char_height * 0),         Vec2.new(char_width, char_height)),
    ["4"] = Rect.new( Vec2.new(char_width * 4, char_height * 0),         Vec2.new(char_width, char_height)),
    ["5"] = Rect.new( Vec2.new(char_width * 5, char_height * 0),         Vec2.new(char_width, char_height)),
    ["6"] = Rect.new( Vec2.new(char_width * 0, char_height * 1),         Vec2.new(char_width, char_height)),
    ["7"] = Rect.new( Vec2.new(char_width * 1, char_height * 1),         Vec2.new(char_width, char_height)),
    ["8"] = Rect.new( Vec2.new(char_width * 2, char_height * 1),         Vec2.new(char_width, char_height)),
    ["9"] = Rect.new( Vec2.new(char_width * 3, char_height * 1),         Vec2.new(char_width, char_height)),
    [":"] = Rect.new( Vec2.new(char_width * 4 + 22, char_height * 1),    Vec2.new(20, char_height)),
    ["."] = Rect.new( Vec2.new(char_width * 5 + 22, char_height * 1),    Vec2.new(20, char_height)),
}

function onCreate()

    local counter = 1
    local children = entity:getChildren()

    
    for i=1, #children do        
        local child_entity = scene:getEntity(children[i])
        local child_transform = child_entity:getTransform()
        local child_sprite = child_entity:getSprite()
        
        local child_uuid = child_entity:getUuid()
        character_order[child_uuid:to_string()] = counter


        child_sprite:setTexture(digits_uuid);

        if (counter == 3 or counter == 6) then -- colons
            child_sprite:setTextureRect(characters[':'])
            child_transform:setPosition(total_width - 20, 0);
            total_width = total_width + 20
        elseif (counter == 9) then -- period
            child_sprite:setTextureRect(characters['.'])
            child_transform:setPosition(total_width - 20, 0);
            total_width = total_width + 20
        else -- digits
            child_sprite:setTextureRect(characters['0'])
            child_transform:setPosition(total_width, 0);
            total_width = total_width + 64;
        end

        counter = counter + 1
    end

    local start_pos = Vec2.new(0, 0)
    local end_pos = Vec2.new(total_width, 128)
    local window_size = scene:getRenderWindow():getWindowSize()

    local position = Vec2.new(
        ((window_size.x / 2.0) - ((end_pos.x - start_pos.x) / 2.0)),
        ((window_size.y / 2.0) - ((end_pos.y - start_pos.y) / 2.0))
    )

    transform = entity:getTransform()
    transform:setPosition(position); 

    entity:getSoundPlayer():setAudio(alarm_uuid);
    entity:getSoundPlayer():setLooping(true);

    timer = Countdown.new(start_time)
end

function onLoad()
end

function onUpdate(delta_time)
    if (not finished) then

        local time = timer:getFormatted()
        
        if (timer:getFinished()) then
            finished = true
            time = "00:00:00.000"
            entity:getSoundPlayer():play()
        end
        local children = entity:getChildren()

        for i=1, #children do
            local child_entity = scene:getEntity(children[i])
            local child_sprite = child_entity:getSprite()
            local index = character_order[child_entity:getUuid():to_string()]

            if (index ~= 3 and index ~= 6 and index ~= 9) then
                local character = string.sub(time, index, index)
                child_sprite:setTextureRect(characters[character])
            end
        end
    end
end

function onInput(event)
    if (event.type == EventType.Resize) then
        local start_pos = Vec2.new(0, 0)
        local end_pos = Vec2.new(total_width, 128)
        local window_size = event.resize.size

        local position = Vec2.new(
            ((window_size.x / 2.0) - ((end_pos.x - start_pos.x) / 2.0)),
            ((window_size.y / 2.0) - ((end_pos.y - start_pos.y) / 2.0))
        )

        transform = entity:getTransform()
        transform:setPosition(position); 
    end
end

function onDelete()
end