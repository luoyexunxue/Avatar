if engine.info().platform == 'android' then
   gui.scale(2)
end
updateLight = function()
local lightdirx,lightdiry,lightdirz = camera.direction()
graphics.directionLight(lightdirx,lightdiry,lightdirz,1,1,1)
end
camera.type('gaze')
camera.fov(60)
camera.clip(0.2,2000)
camera.position(-16.15913,20.96049,18.5136)
camera.angle(-2.484828,-0.6104029,0)