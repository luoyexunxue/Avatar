if engine.info().platform == 'android' then
   gui.scale(2)
end
updateLight = function()
local lightdirx,lightdiry,lightdirz = camera.direction()
graphics.directionLight(lightdirx,lightdiry,lightdirz,1,1,1)
end
camera.type('smooth')
camera.fov(60)
camera.clip(0.2,2000)
camera.position(0,-5.999999,0)
camera.angle(0,0,0)