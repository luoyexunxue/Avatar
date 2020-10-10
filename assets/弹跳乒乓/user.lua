engine.input('scale', 0, 0)
camera.type('viewer')
camera.position(0, -20, 0)
camera.angle(0, -0.57, 0)
scene.insert(nil, 'geometry', 'plane', {shape='circle', r=10, texture='green'})
scene.insert(nil, 'geometry', 'sphere', {shape='sphere', r=1})
scene.position('sphere', 0,0,10)
physics.bind("plane", { shape='plane', mass = 0, restitution=0.8 })
physics.bind("sphere", { mass = 1, angularDamping = -0.05, restitution=0.6 })
physics.gravity(0, 0, -9.8)
local snd = sound.create('sound.wav', true)
physics.collide("sphere", function(enter)
	if enter then sound.play(snd) end
end)
OnInput = function(name, value, arg1, arg2)
	if name == 'fire' then
		scene.position('sphere', 0,0,10)
		physics.reset('sphere')
	end
end