local menu = {}

local nk = moonnuklear

function menu:init()
end

function menu:draw(ctx)
    if nk.window_begin(ctx, "Lua Menu", {0, 0, 640, 480}, 0) 
    then
        local btnFlags = nk.TEXT_ALIGN_CENTERED | nk.TEXT_ALIGN_MIDDLE

        nk.layout_row_dynamic(ctx, 25, 3)


        button_helper('Start', btnFlags, function()
            printf('Cinematics')
        end)

        button_helper('Back', btnFlags, function()
            setMenu('main')
        end)
    end
    nk.window_end(ctx)
end

function menu:shutdown()
end

return menu
