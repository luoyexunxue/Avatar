soundSwap = 0
soundWin = 0
currentLevel = 2
lastSelected = nil
showtime = false
swapTable = {}
levelMap = {"lvl1.jpg", "lvl2.jpg", "lvl3.jpg", "lvl4.jpg", "lvl5.jpg", "lvl6.jpg", "lvl7.jpg"}

function loadLevel(level)
	currentLevel = level
	swapTable = {}
	scene.clear()
	scene.insert(nil, "skybox", "skybox", { texture = "nebula_px.jpg,nebula_nx.jpg, nebula_pz.jpg, nebula_nz.jpg, nebula_py.jpg, nebula_ny.jpg" })
	engine.log("Load level "..level)
	for i = 1, currentLevel do
		local u0 = (i - 1) / currentLevel
		local u1 = i / currentLevel
		local u0_ = (currentLevel - i) / currentLevel
		local u1_ = (currentLevel - i + 1) / currentLevel
		for j = 1, currentLevel do
			local v0 = (j - 1) / currentLevel
			local v1 = j / currentLevel
			local v0_ = (currentLevel - j) / currentLevel
			local v1_ = (currentLevel - j + 1) / currentLevel
			for k = 1, currentLevel do
				local w0 = (k - 1) / currentLevel
				local w1 = k / currentLevel
				local w0_ = (currentLevel - k) / currentLevel
				local w1_ = (currentLevel - k + 1) / currentLevel
				if i == 1 or i == currentLevel or j == 1 or j == currentLevel or k == 1 or k == currentLevel then
					local x = (2 * i - currentLevel - 1) / 2.0
					local y = (2 * j - currentLevel - 1) / 2.0
					local z = (2 * k - currentLevel - 1) / 2.0
					local uv = {}
					uv[1] = { s = u0, t = w1_ }
					uv[2] = { s = u1, t = w1_ }
					uv[3] = { s = u0, t = w0_ }
					uv[4] = { s = u1, t = w0_ }
					uv[5] = { s = v0, t = w1_ }
					uv[6] = { s = v1, t = w1_ }
					uv[7] = { s = v0, t = w0_ }
					uv[8] = { s = v1, t = w0_ }
					uv[9] = { s = u0_, t = w1_ }
					uv[10] = { s = u1_, t = w1_ }
					uv[11] = { s = u0_, t = w0_ }
					uv[12] = { s = u1_, t = w0_ }
					uv[13] = { s = v0_, t = w1_ }
					uv[14] = { s = v1_, t = w1_ }
					uv[15] = { s = v0_, t = w0_ }
					uv[16] = { s = v1_, t = w0_ }
					uv[17] = { s = u0, t = v1_ }
					uv[18] = { s = u1, t = v1_ }
					uv[19] = { s = u0, t = v0_ }
					uv[20] = { s = u1, t = v0_ }
					uv[21] = { s = u0, t = v1 }
					uv[22] = { s = u1, t = v1 }
					uv[23] = { s = u0, t = v0 }
					uv[24] = { s = u1, t = v0 }
					local cube = "cube"..i..j..k
					scene.insert(nil, "geometry", cube, { shape = "box", texture = levelMap[currentLevel - 1] })
					scene.position(cube, x, y, z)
					scene.update(cube, 0, uv)
					swapTable[cube] = (i - 1) * currentLevel * currentLevel + (j - 1) * currentLevel + k
					animation.scale(cube, 0,0,0, 0.0, "accelerate")
					animation.scale(cube, 1,1,1, 1.0, "accelerate")
					animation.start(cube)
				end
			end
		end
	end
end

function swapCube(a, b)
	local x, y, z = scene.position(a)
	local m, n, k = scene.position(b)
	scene.position(a, m, n, k)
	scene.position(b, x, y, z)
	local temp = swapTable[a];
	swapTable[a] = swapTable[b]
	swapTable[b] = temp;
end

function randomSwap()
	for i = 1, 10 * currentLevel * currentLevel do
		local a = math.random(currentLevel)
		local b = math.random(currentLevel)
		local c = math.random(currentLevel)
		if a == 1 or a == currentLevel or b == 1 or b == currentLevel or c == 1 or c == currentLevel then
			local e = math.random(currentLevel)
			local f = math.random(currentLevel)
			local g = math.random(currentLevel)
			if e == 1 or e == currentLevel or f == 1 or f == currentLevel or g == 1 or g == currentLevel then
				swapCube("cube"..a..b..c, "cube"..e..f..g)
			end
		end
	end
end

function checkSuccess()
	for i = 1, currentLevel do
		for j = 1, currentLevel do
			for k = 1, currentLevel do
				-- 仅判断外部方块
				if i == 1 or i == currentLevel or j == 1 or j == currentLevel or k == 1 or k == currentLevel then
					local cube = swapTable["cube"..i..j..k]
					if i ~= 1 and i ~= currentLevel and j ~= 1 and j ~= currentLevel then
						local index1 = (i - 1) * currentLevel * currentLevel + (j - 1) * currentLevel + 1
						local index2 = (i - 1) * currentLevel * currentLevel + j * currentLevel
						if cube ~= index1 and cube ~= index2 then return false end
					elseif i ~= 1 and i ~= currentLevel and k ~= 1 and k ~= currentLevel then
						local index1 = (i - 1) * currentLevel * currentLevel + k
						local index2 = i * currentLevel * currentLevel - currentLevel + k
						if cube ~= index1 and cube ~= index2 then return false end
					elseif j ~= 1 and j ~= currentLevel and k ~= 1 and k ~= currentLevel then
						local index1 = (j - 1) * currentLevel + k
						local index2 = (currentLevel - 1) * currentLevel * currentLevel + (j - 1) * currentLevel + k
						if cube ~= index1 and cube ~= index2 then return false end
					else
						local index = (i - 1) * currentLevel * currentLevel + (j - 1) * currentLevel + k
						if cube ~= index then return false end
					end
				end
			end
		end
	end
	return true
end

camera.type("gaze")
camera.position(0, -6, 0)
engine.input("scale", 1, -1)
soundSwap = sound.create("swap.mp3")
soundWin = sound.create("mission_done.wav")
if engine.info().platform == "android" then
	gui.scale(2);
end
local saveLevel = engine.read("puzzle.conf")
if saveLevel ~= nil then
	local index = string.find(saveLevel, ";")
	currentLevel = string.sub(saveLevel, 1, index - 1) * 1
	loadLevel(currentLevel)
	local indexComma = string.find(saveLevel, ",", index + 1)
	while indexComma ~= nil do
		local pair = string.sub(saveLevel, index + 1, indexComma - 1)
		local split = string.find(pair, "=")
		local key = string.sub(pair, 1, split - 1)
		local val = string.sub(pair, split + 1)
		if string.format("%d", swapTable[key]) ~= val then
			for ik, iv in pairs(swapTable) do
				if string.format("%d", iv) == val then
					engine.log(key.." <=> "..ik)
					swapCube(key, ik)
					break
				end
			end
		end
		index = indexComma
		indexComma = string.find(saveLevel, ",", index + 1)
	end
else
	loadLevel(currentLevel)
	randomSwap()
end

OnInput = function(name, value, arg1, arg2)
	if name == "fire" then
		if showtime then return end
		local x, y, z, dx, dy, dz = graphics.pickingRay(arg1, arg2)
		local node, x, y, z = scene.pick(x, y, z, dx, dy, dz)
		if node == nil then return end
		if lastSelected == node then
			scene.material(node, 0, {color="white"})
			lastSelected = nil
		elseif lastSelected == nil then
			scene.material(node, 0, {color="red"})
			lastSelected = node
		else
			scene.material(lastSelected, 0, {color="white"})
			swapCube(lastSelected, node)
			lastSelected = nil
			if checkSuccess() == true then
				sound.play(soundWin)
				showtime = true
				local w, h = gui.size()
				local desc = "width=200;height=64;top="..(h - 64)..";left="..(w / 2 - 100)..";backColor=0x00000000;foreColor=0xFFFF00FF;text=下一关;font=36"
				gui.create("nextLevel", "button", desc, function(evt, arg1, arg2)
					if evt == 1 then
						gui.modify("nextLevel", "foreColor=0xFF00FFFF")
					elseif evt == 2 then
						showtime = false
						post.enable("a", false)
						gui.modify("nextLevel", "foreColor=0xFFFF00FF")
						loadLevel(currentLevel + 1)
						randomSwap()
						gui.delete("nextLevel")
					end
				end)
			else
				sound.play(soundSwap)
			end
		end
	end
end

OnExit = function()
	local saveLevel = string.format("%d;", currentLevel)
	for key, value in pairs(swapTable) do
		saveLevel = saveLevel..string.format("%s=%d,", key, value)
	end
	engine.write("puzzle.conf", saveLevel)
end