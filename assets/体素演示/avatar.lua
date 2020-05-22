--重载 dofile 函数
dofile = function(file)
	engine.script(file)
end

--------------------------------------------------
-- 引擎初始化完成
--------------------------------------------------
OnReady = function()
	scene.insert(nil, "skybox", "skybox", { texture = "px.jpg,nx.jpg,pz.jpg,nz.jpg,py.jpg,ny.jpg" })
	scene.insert(nil, "geometry", "center", { shape = "box", texture = "logo.png" })
	camera.type("gaze")
	camera.position(0, -6, 0)
	engine.input("scale", 1, -1)
end

--------------------------------------------------
-- 引擎逻辑更新
--------------------------------------------------
OnUpdate = function(dt)
	local x, y, z = camera.position()
	graphics.directionLight(-x, -y, -z)
end

--------------------------------------------------
-- 输入事件
--------------------------------------------------
OnInput = function(name, value, arg1, arg2)
	if name == "fire" then
		local sx, sy, sz, dx, dy, dz = graphics.pickingRay(arg1, arg2)
		local node, x, y, z, mesh, face = scene.pick(sx, sy, sz, dx, dy, dz)
		if node ~= "" then
			local px, py, pz = scene.position(node)
			local vx, vy, vz, s, t, nx, ny, nz = scene.vertex(node, mesh, face, 0)
			px = px + nx
			py = py + ny
			pz = pz + nz
			local name = string.format("cube@%d@%d@%d", math.ceil(px), math.ceil(py), math.ceil(pz))
			scene.insert(nil, "geometry", name, { shape = "box", texture = "brick.jpg" })
			scene.position(name, px, py, pz)
			engine.log(name)
		end
	end
end

--------------------------------------------------
-- 引擎退出
--------------------------------------------------
OnExit = function()
end