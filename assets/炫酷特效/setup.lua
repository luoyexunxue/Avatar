if engine.info().platform == 'android' then
   gui.scale(2)
end
updateLight = function()
end
graphics.directionLight(0,1,-1,1,1,1)
camera.type('chase')
camera.fov(60)
camera.clip(0.2,2000)
camera.position(0,0,0)
camera.angle(0,0,0)