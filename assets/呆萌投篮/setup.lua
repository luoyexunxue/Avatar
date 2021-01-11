if engine.info().platform == 'android' then
   gui.scale(2)
end
updateLight = function()
local lightx,lighty,lightz = camera.position()
local lightdirx,lightdiry,lightdirz = camera.direction()
graphics.spotLight(lightx,lighty,lightz,lightdirx,lightdiry,lightdirz,1.130972,500,1,1,1)
end
camera.type('chase')
camera.fov(60)
camera.clip(0.2,2000)
camera.position(0,-45,0)
camera.angle(0,0,0)