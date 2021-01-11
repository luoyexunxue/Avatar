is_fail = true
s_start = sound.create('start.wav')
s_win = sound.create('win.wav')
s_fail = sound.create('fail.wav')
animation.translation('ban', -5,0,7.2, 0, 'acceleratedecelerate')
animation.translation('ban', 5,0,7.2, 3, 'acceleratedecelerate')
animation.start('ban', -1, true)
physics.gravity(0,0,-9.8)
physics.collide('detect', function(enter)
	if enter then
		sound.play(s_win)
		is_fail = false
	end
end)
physics.collide('detect_ground', function(enter)
	if enter then
		if is_fail then sound.play(s_fail) end
	end
end)
OnSize = function(w, h)
	local width, height = gui.size()
	width = width / 2 - 25
	height = height - 50 - 10
	gui.modify('start', 'top='..height..';left='..width);
end
OnInput = function(name, key, arg1, arg2, arg3)
	if name == 'fire' then
		local x, y, z = scene.position('ball')
		if z < -10 then throwBall() end
	end
end
throwBall = function()
	sound.play(s_start)
	scene.position('ball',0,-20,-3.4)
	physics.reset('ball')
	physics.applyImpulse('ball', 0,10,14)
	is_fail = true
end