if engine.info().platform == 'android' then
   gui.scale(2)
end
updateLight = function()
end
graphics.directionLight(0,1,-1,1,1,1)
camera.type('smooth')
camera.fov(60)
camera.clip(0.2,2000)
camera.position(573.1732,239.9392,-14.69009)
camera.angle(1.88002,0.60043,-0.1110791)