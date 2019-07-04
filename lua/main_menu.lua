local menu = {}

local nk = moonnuklear
local img = nil
local img_handle = nil

function menu:init()
    img_handle = trap_R_RegisterShader("crosshairs/crosshair1")
    img = nk.new_image(img_handle)
end

function menu:draw(ctx)
    if nk.window_begin(ctx, "Lua Menu", {0, 0, 640, 480}, 0) 
    then
        local btnFlags = nk.TEXT_ALIGN_CENTERED | nk.TEXT_ALIGN_MIDDLE

        nk.layout_row_dynamic(ctx, 50, 3)
        nk.spacing(ctx, 1)
        nk.image(ctx, img)
        
        nk.layout_row_dynamic(ctx, 25, 3)

        button_helper('Single Player', btnFlags, function()
            setMenu('sp')
        end)

        button_helper('Multiplayer', btnFlags, function()
            setMenu('mp')
        end)

        button_helper('Setup', btnFlags, function()
            setMenu('setup')
        end)

        button_helper('Demos', btnFlags, function()
            setMenu('demos')
        end)

        button_helper('Cinematics', btnFlags, function()
            setMenu('cin')
        end)

        button_helper('Exit', btnFlags, function()
            exit()
        end)
    end
    nk.window_end(ctx)
end

function menu:shutdown()
end

return menu
