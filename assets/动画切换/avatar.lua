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
   camera.type('smooth')
   camera.position(0.5204799,-8.363161,2.74688)
   camera.angle(0.06215466,-0.3167765,-0.02491974)
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