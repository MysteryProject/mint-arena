local menu = {}

local nk = moonnuklear

function menu:init()
end

function menu:draw(ctx)
    local ret = 'main'

    if nk.window_begin(ctx, "Lua Menu", {0, 0, 640, 480}, 0) 
    then
        local btnFlags = nk.TEXT_ALIGN_CENTERED | nk.TEXT_ALIGN_MIDDLE

        nk.layout_row_dynamic(ctx, 96, 1)

        nk.layout_row_dynamic(ctx, 25, 3)

        button_helper('Single Player', btnFlags, function()
            ret = 'sp'
        end)

        button_helper('Multiplayer', btnFlags, function()
            printf('Multiplayer')
        end)

        button_helper('Setup', btnFlags, function()
            printf('Setup')
        end)

        button_helper('Demos', btnFlags, function()
            printf('Demos')
        end)

        button_helper('Cinematics', btnFlags, function()
            printf('Cinematics')
        end)

        button_helper('Exit', btnFlags, function()
            exit()
        end)
    end
    nk.window_end(ctx)

    return ret
end

function menu:shutdown()
end

return menu
