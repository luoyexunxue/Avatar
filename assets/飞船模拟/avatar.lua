OnReady = function()
   graphics.background(0.160802,0.152958,0.160802,1.0)
   engine.input('map',1,2,3)
   engine.input('scale',1,1)
   engine.input('disable',false,false,true)
   dofile('setup.lua')
   dofile('load.lua')
   dofile('gui.lua')
   dofile('user.lua')
end

OnUpdate = function(dt)
   updateLight()
end

OnInput = function(name, param)
end

OnSize = function(width, height)
end

OnExit = function()
end

updateLight = function()
end

dofile = function(file)
   engine.script(file)
end
