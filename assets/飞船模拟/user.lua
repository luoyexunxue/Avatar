graphics.background(0, 0, 0, 1.0)
graphics.directionLight(-1, -1, -0.3)
graphics.environmentMap(true, 'skybox_texture')

camera.type("viewer")
camera.target(600, 0, 0)
camera.angle(1.9, 0.5, 0)
camera.position(660, 0, 0)

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

post.enable('bloom', true)