--重载 print 函数
print = function(text)
	engine.log(text)
end

--重载 dofile 函数
dofile = function(file)
	engine.script(file)
end

gTime = 0
gInterval = 0
gChasing = false
gPauseSound = false
gLightPos = false
gGravity = false
gShootCount = 0
gPickingMode = 0

dofile("gui.lua")
dofile("tips.lua")

--------------------------------------------------
-- 引擎初始化完成
--------------------------------------------------
OnReady = function()
	local info = engine.info()
	print("引擎版本 "..info.VERSION)
	print("脚本版本 ".._VERSION)
	print("运行平台 "..info.PLATFORM)
	gPlatform = info.PLATFORM
	engine.input("disable", false, false, true)
	if gPlatform == "android" then
		gui.scale(2);
	end
	--创建按钮
	addButton("stereoMode", "立体显示", function()
		local stereoPostprocess = "anaglyph"
		if gPlatform == "android" then stereoPostprocess = "oculus" end
		if post.enable(stereoPostprocess) then
			graphics.stereo(false)
			post.enable(stereoPostprocess, false)
		else
			graphics.stereo(true)
			post.enable(stereoPostprocess, true)
		end
	end)
	addButton("shiftCamera", "相机切换", function()
		local cameraType = camera.type()
		if cameraType == "chase" then
			camera.type("gaze")
		elseif cameraType == "gaze" then
			camera.type("geographic")
		elseif cameraType == "geographic" then
			camera.type("free")
		elseif cameraType == "free" then
			camera.type("viewer")
		elseif cameraType == "viewer" then
			camera.type("default")
		elseif cameraType == "default" then
			camera.type("chase")
		end
		print("切换相机类型为 "..camera.type())
	end)
	addButton("targetChasing", "人物跟踪", function()
		gChasing = not gChasing
	end)
	addButton("pauseSound", "暂停声音", function()
		sound.pause(not gPauseSound)
		gPauseSound = not gPauseSound
	end)
	addButton("shadow", "阴影显示", function()
		graphics.shadow(not graphics.shadow())
	end)
	addButton("lightChasing", "光源跟踪", function()
		gLightPos = not gLightPos
	end)
	addButton("fog", "雾效显示", function()
		graphics.fog(not graphics.fog(), 0.7, 0.7, 0.7, 0.1, 500.0)
	end)
	addButton("environmentMap", "环境光照", function()
		graphics.environmentMap(not graphics.environmentMap(), 'skybox_texture')
	end)
	addButton("gravity", "引力开关", function()
		gGravity = not gGravity
		physics.gravity(0, 0, gGravity and -9.8 or 0)
	end)
	addButton("water", "水面显示", function()
		scene.visible("water", not scene.visible("water"))
	end)
	addEditbox("postName", "snow")
	addButton("post", "POST", function()
		local postname = gui.attrib("postName", "text")
		post.enable(postname, not post.enable(postname))
	end)
	--光照方向
	addTrackBar("trackbar", 50, function(percent)
		local alpha = percent * 0.062832
		local lx = math.sin(alpha)
		local ly = math.cos(alpha)
		graphics.directionLight(lx, ly, -1.0)
		scene.position("lensflare", lx * 100, ly * 100, 100.0)
		sound.volume(percent)
	end)
	createMessage()
	createFps()
	--播放背景音乐
	sound.play(sound.create("background.mp3", true, true, 0.2))
	--加载场景
	dofile("load.lua")
	--加载后处理
	dofile("postprocess.lua")
	--加载用户脚本
	dofile("user.lua")
end

--------------------------------------------------
-- 引擎逻辑更新
--------------------------------------------------
OnUpdate = function(dt)
	gTime = gTime + dt
	--获取帧率
	gInterval = gInterval + dt
	if gInterval > 3 then
		updateFps()
		gInterval = 0
	end
	--实现酒娘环绕行走
	local x, y, z = scene.position("jiuniang")
	local theta = gTime * 0.25
	x = math.cos(theta) * 6.5
	y = math.sin(theta) * 6.5
	scene.angle("jiuniang", 1.57, 0, theta + 3.14)
	scene.position("jiuniang", x, y, z)
	if gChasing and not gLightPos then
		camera.target(x, y, z + 2.0)
	end
	--跟踪光源
	if gLightPos then
		local lx, ly, lz, lw = graphics.lightPosition()
		camera.position(lx, ly, lz)
		camera.target(0, 0, 0)
	end
	--标签演示
	animateMessage(theta)
end

--------------------------------------------------
-- 引擎退出
--------------------------------------------------
OnExit = function()
	print("再见")
end

--------------------------------------------------
-- 窗体大小改变
--------------------------------------------------
OnSize = function(width, height)
	print("窗体大小 "..width.." x "..height)
	updateTipLocation()
	updatePanelSize()
end

--------------------------------------------------
-- 输入事件
--------------------------------------------------
OnInput = function(name, value, arg1, arg2)
	-- key
	if name == "key" then
		local iname = ""
		local ishape = 1
		local icolor = 1
		local shape_set = {"box", "sphere", "capsule", "cylinder", "torus", "cone"}
		local color_set = {'red', 'green', 'blue', 'yellow', 'purple', 'white', ''}
		if value == 32 then
			for i = 0,gShootCount,1 do
				scene.delete("geometry_"..i)
			end
			gShootCount = 0
		elseif value > 32 then
			iname = "geometry_"..gShootCount
			ishape = value % 6 + 1
			icolor = math.random(7)
		end
		print(value)
		if iname ~= "" then
			local tx, ty, tz = camera.target()
			local px, py, pz = camera.position()
			scene.insert(nil, "geometry", iname, { shape = shape_set[ishape], r = 0.5, h = 0.5, c = 0.1, texture = color_set[icolor] })
			scene.position(iname, px, py, pz - 0.5)
			physics.bind(iname, { mass = 0.5, linearDamping = -0.5, angularDamping = -0.1 })
			physics.velocity(iname, tx * 50, ty * 50, tz * 50, 0, 0, 0)
			gShootCount = gShootCount + 1
			print("add "..shape_set[ishape].." "..iname)
		end
	end
	-- fire
	if name == "fire" then
		local x, y, z, dx, dy, dz = graphics.pickingRay(arg1, arg2)
		local node, x, y, z = scene.pick(x, y, z, dx, dy, dz)
		if gPickingMode == 0 then
			physics.applyImpulse(node, dx, dy, dz, x, y, z)
			if node == 'jiuniang' and type(tts) == 'table' then
				tts.play('fuck you')
			end
		else
			scene.handle("line", "AddPoint", { x = x, y = y, z = z })
		end
	end
	-- function
	if name == "function" then
		if value == 2 then
			graphics.stereo(true)
		elseif value == 3 then
			graphics.stereo(false)
		elseif value == 4 then
			post.enable("oculus", not post.enable("oculus"))
		elseif value == 5 then
			post.enable("anaglyph", not post.enable("anaglyph"))
		elseif value == 8 then
			if gPickingMode == 0 then
				gPickingMode = 1
			elseif gPickingMode == 1 then
				gPickingMode = 2
				scene.handle('line', 'SmoothLine', { ds = 0.1 })
			elseif gPickingMode == 2 then
				gPickingMode = 3
				camera.type('free', { track_line = 'line', track_offset_z = 0.5, track_speed = 10, track_follow=false })
			elseif gPickingMode == 3 then
				gPickingMode = 0
				scene.handle("line", "ClearPoint")
			end
		elseif value == 12 then
			graphics.screenshot("screen_shots.png")
		end
	end
end