package.path = BASE_DIR .. '?.lua;' .. package.path

main = { }

local nk = moonnuklear
local ctx = nil
local crosshairImage = nil
local font = nil
local started = false

local menus = {
    ['main'] = require('main_menu'),
    ['sp'] = require('singleplayer_menu'),
    ['mp'] = require('multiplayer_menu'),
    ['setup'] = require('setup_menu'),
    ['demos'] = require('demos_menu'),
    ['cin'] = require('cinematics_menu')
}
local active_menu = nil

function setMenu(name)
    active_menu = menus[name]
end

function color255(color)
    return {
        color[1] * 255,
        color[2] * 255,
        color[3] * 255,
        color[4] * 255,
    }
end

function RenderCommandList(ctx)
    local cmds = ctx:commands()
    for i, cmd in ipairs(cmds)
    do
        if cmd.type == nk.COMMAND_SCISSOR then
            CG_SetClipRegion(cmd.x, cmd.y, cmd.w, cmd.h)
        elseif cmd.type == nk.COMMAND_LINE then
            NK_DrawLine(cmd.start, cmd.stop, cmd.line_thickness, color255(cmd.color))
        elseif cmd.type == nk.COMMAND_CURVE then
        elseif cmd.type == nk.COMMAND_RECT then
            CG_DrawRect(cmd.x, cmd.y, cmd.w, cmd.h, 2, cmd.color)
        elseif cmd.type == nk.COMMAND_RECT_FILLED then
            CG_FillRect(cmd.x, cmd.y, cmd.w, cmd.h, cmd.color)
        elseif cmd.type == nk.COMMAND_RECT_MULTI_COLOR then
            NK_DrawRectMultiColor(cmd.x, cmd.y, cmd.w, cmd.h, color255(cmd.left), color255(cmd.top), color255(cmd.right), color255(cmd.bottom))
        elseif cmd.type == nk.COMMAND_TRIANGLE then
        elseif cmd.type == nk.COMMAND_TRIANGLE_FILLED then
            NK_DrawTriangleFilled(cmd.a, cmd.b, cmd.c, color255(cmd.color))
        elseif cmd.type == nk.COMMAND_CIRCLE then
        elseif cmd.type == nk.COMMAND_CIRCLE_FILLED then
            NK_DrawCircleFilled(cmd.x, cmd.y, cmd.w, cmd.h, color255(cmd.color))
        elseif cmd.type == nk.COMMAND_ARC then
        elseif cmd.type == nk.COMMAND_ARC_FILLED then
        elseif cmd.type == nk.COMMAND_POLYGON then
        elseif cmd.type == nk.COMMAND_POLYGON_FILLED then
            --NK_DrawPolygon(cmd.points, cmd.color)
        elseif cmd.type == nk.COMMAND_POLYLINE then
        elseif cmd.type == nk.COMMAND_TEXT then
            CG_DrawString(cmd.x, cmd.y, cmd.text, UI_BIGFONT, cmd.foreground)
        elseif cmd.type == nk.COMMAND_IMAGE then
            CG_DrawImage(cmd.x, cmd.y, cmd.w, cmd.h, cmd.handle)
        end
    end
    ctx:clear()
end

function _TextWidth(height, text)
    return CG_DrawStrlen(text)
end

function EmptyFunc(x, y, z)
end

function main:init()
    printf('Initializing Lua UI')

    ctx = nk.init()

    font = nk.new_user_font(12, _TextWidth, 0, EmptyFunc)

    nk.style_set_font(ctx, font)

    active_menu = menus['main']
    active_menu:init()

    started = true
end

local r = 0
local g = 0
local b = 0
local a = 0
local color2 = { 0.5, 0.5, 0.5, 0.5 }

function button_helper(text, flags, func)
    nk.spacing(ctx, 1)
    nk.style_push_flags(ctx, 'button.text_alignment', flags)
    if nk.button(ctx, nil, text) then
        func()
    end
    nk.style_pop_flags(ctx)
    nk.spacing(ctx, 1)
end

function main:draw()
    if not started then return end

    nk.input_motion(ctx, GetMousePosition());
    nk.input_end(ctx)
    
    active_menu:draw(ctx)

    RenderCommandList(ctx)
    nk.input_begin(ctx)
end

function nk2q3key(key)

end

function main:key_event(key, down)
    if key == K_DOWNARROW then
    elseif key == K_MOUSE1 then
        local x, y = GetMousePosition();
        nk.input_button(ctx, nk.BUTTON_LEFT, x, y, down);
    elseif (key & K_CHAR_FLAG) then
        local keycode = key & ~K_CHAR_FLAG
        nk.input_char(ctx, keycode)
    end
end

function exit()
    printf('Shutting down Lua UI')
    --crosshairImage:free()
    started = false
    _exit()
end

function main:shutdown()
    exit()
end
