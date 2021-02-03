local magnitude = 0.4
local currentLevel = 1

local saveLevel = engine.read("balance.conf")
if saveLevel ~= nil then currentLevel = tonumber(saveLevel) end
OnSuccess = function()
	currentLevel = currentLevel + 1
	engine.write("balance.conf", currentLevel)
	physics.unbind('player')
	scene.position('player', 0, 0, 0.6)
	sound.play(soundSuccess)
	camera.position(0,0,10)
	local w, h = gui.size()
	local desc = "width=400;height=100;top="..(h / 2)..";left="..(w / 2 - 200)..";backColor=#FFFFFF00;foreColor=#FF0000;text=NEXT LEVEL;font=72;stretch=true"
	gui.create("nextLevel", "button", desc, function(evt, arg1, arg2)
		if evt == 1 then
			gui.modify("nextLevel", "foreColor=#FF00FFFF")
		elseif evt == 2 then
			gui.delete("nextLevel")
			scene.clear()
			dofile('load_level'..currentLevel..'.lua')
		end
	end)
end
OnFail = function()
	sound.play(soundFailed)
	physics.reset('player')
	scene.position('player', 0, 0, 0.6)
	camera.position(0,0,10)
end
OnInput = function(name, param)
	if name == 'key' then
		if param.value == 32 and currentLevel == 1 then scene.position('player',-0.04,31.54,10) end
		if param.value == 32 and currentLevel == 2 then scene.position('player',11,6.92,1.312) end
		local fx = 0
		local fy = 0
		local fz = 0
		local lx, ly, lz, rx, ry, rz, ux, uy, uz = camera.direction()
		if param.value == 97 then
			fx = -magnitude * rx
			fy = -magnitude * ry
			fz = -magnitude * rz
		end
		if param.value == 115 then
			fx = -magnitude * ux
			fy = -magnitude * uy
			fz = -magnitude * uz
		end
		if param.value == 100 then
			fx = magnitude * rx
			fy = magnitude * ry
			fz = magnitude * rz
		end
		if param.value == 119 then
			fx = magnitude * ux
			fy = magnitude * uy
			fz = magnitude * uz
		end
		physics.applyImpulse('player', fx, fy, fz)
	end
end
OnUpdate = function(dt)
	local x, y, z = scene.position('player')
	if z < -50 or z > 50 then
		OnFail()
	end
	if z > -3 then
		camera.target(x, y, z)
	end
end

soundSuccess = sound.create("sound_success.mp3")
soundFailed = sound.create("sound_fail.mp3")
graphics.directionLight(0,0.3,-1)
camera.type('gaze', {pitchMin=-1.57, pitchMax=-1.5, maxDistance=20})
camera.position(0,0,10)
engine.input('disable', false, false, false)
physics.gravity(0,0,-9.8)
dofile('load_level'..currentLevel..'.lua')