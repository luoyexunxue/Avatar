updateLight = function()
end

dofile = function(file)
   engine.script(file)
end

-- 初始化完成
OnReady = function()
   graphics.background(0.160802,0.152958,0.160802)
   engine.input('map',1,2,3)
   engine.input('scale',1,1)
   engine.input('disable',false,false,true)
   camera.type('geographic')
   camera.position(-1.290999,-9.161304,4.545068)
   camera.angle(-0.1399968,-0.4049998,0)
   if engine.info().PLATFORM == "android" then
      gui.scale(2);
   end
   dofile('load.lua')
   dofile('gui.lua')
   dofile('user.lua')
end

-- 逻辑更新
OnUpdate = function(dt)
   updateLight()
end

-- 输入事件
OnInput = function(name, value, arg1, arg2)
end

-- 画面大小改变
OnSize = function(width, height)
end

-- 引擎退出
OnExit = function()
end