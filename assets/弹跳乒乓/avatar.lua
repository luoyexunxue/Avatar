﻿--重载 dofile 函数
dofile = function(file)
	engine.script(file)
end

--------------------------------------------------
-- 引擎初始化完成
--------------------------------------------------
OnReady = function()
	graphics.background(0.16, 0.153, 0.16, 1.0)
	dofile('user.lua')
end

--------------------------------------------------
-- 引擎逻辑更新
--------------------------------------------------
OnUpdate = function(dt)
	local x, y, z = camera.target()
	graphics.directionLight(x, y, z)
end

--------------------------------------------------
-- 输入事件
--------------------------------------------------
OnInput = function(name, value, arg1, arg2)
end

--------------------------------------------------
-- 引擎退出
--------------------------------------------------
OnExit = function()
end