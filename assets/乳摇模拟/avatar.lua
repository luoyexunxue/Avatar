dofile = function(file)
   engine.script(file)
end

-- 初始化完成
OnReady = function()
   graphics.background(0.16, 0.153, 0.16, 1.0)
   camera.position(0.0, -3.333972, 1.604796)
   camera.angle(0.0, -0.08499949, 0)
   engine.input("disable", true, true, false)
   dofile("load.lua")
   dofile("gui.lua")
end

-- 逻辑更新
OnUpdate = function(dt)
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