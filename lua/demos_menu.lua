local menu = {}

local nk = moonnuklear

function menu:init()
end

local EASY = 1;
local HARD = 2;
local op = 'easy'
local value = 0.6

function menu:draw(ctx)
    if nk.window_begin(ctx, "Lua Menu", {0, 0, 1920, 1080}, 0) 
    then
        nk.layout_row_static(ctx, 30, 80, 1)
        if (nk.button(ctx, nil, "button")) then
            -- event handler
        end

        nk.layout_row_dynamic(ctx, 30, 2)
        if (nk.option(ctx, "easy", op == EASY)) then op = EASY end
        if (nk.option(ctx, "hard", op == HARD)) then op = HARD end

        nk.layout_row_begin(ctx, 'static', 30, 2)
            nk.layout_row_push(ctx, 50)
            nk.label(ctx, "Volume:", nk.TEXT_LEFT)
            nk.layout_row_push(ctx, 110)
            value = nk.slider(ctx, 0, value, 1.0, 0.1)
        nk.layout_row_end(ctx)
    end
    nk.window_end(ctx)
end

function menu:shutdown()
end

return menu
