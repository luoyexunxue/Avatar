--------------------------------------------------
-- 引擎初始化完成
--------------------------------------------------
OnReady = function()
	graphics.background(0, 0, 0, 1.0)
	graphics.directionLight(-1, -1, -0.3)

	scene.insert(nil, "skybox", "skybox", { texture = "nebula_px.jpg, nebula_nx.jpg, nebula_pz.jpg, nebula_nz.jpg, nebula_py.jpg, nebula_ny.jpg" })
	scene.insert(nil, "planet", "earth", { texture = "earth.jpg", textureNight = "earth_night.jpg", radius = 500, slices = 128 })
	scene.insert(nil,'geometry','moon',{shape='sphere',texture='moon.jpg',r=50,slices=128})
	scene.insert(nil, "static", "friendship", { meshFile="frienship7.3ds"})
	scene.insert("friendship", "particles", "flame1", { texture = "smoke.tga", size = 1.1, count = 100, spread = 2.5, dz = -23, fade = 8, color = 0xEFB59DFF, dark = false })
	scene.insert("friendship", "particles", "flame2", { texture = "smoke.tga", size = 1.1, count = 100, spread = 2.5, dz = -23, fade = 8, color = 0xEFB59DFF, dark = false })
	scene.insert("friendship", "particles", "flame3", { texture = "smoke.tga", size = 1.1, count = 100, spread = 2.5, dz = -23, fade = 8, color = 0xEFB59DFF, dark = false })

	scene.position("earth", 0, 0, 0)
	scene.position("moon", 0, 800, 0)
	scene.position("flame1", -0.48, 0.18, 1.2)
	scene.position("flame2", -0.66, -1.38, 1.2)
	scene.position("flame3", 0.69, -0.66, 1.2)
	scene.angle("earth", 0, -0.52, 1.57)

	scene.material('friendship', -1, {roughness=0.0,metalness=0.5,color='white'})
	graphics.environmentMap(true, 'skybox_texture')

	camera.type("viewer")
	camera.target(600, 0, 0)
	camera.angle(1.9, 0.5, 0)
	camera.position(660, 0, 0)
end

--------------------------------------------------
-- 引擎逻辑更新
--------------------------------------------------
gTime = 0
OnUpdate = function(dt)
	gTime = gTime + dt
	local theta = gTime * 0.02
	local x = math.cos(theta) * 600.0
	local y = math.sin(theta) * 600.0
	scene.position("friendship", x, y, 0)
	scene.angle("friendship", 0, 1.57, theta + 1.57)
	camera.target(x, y, 0)
end