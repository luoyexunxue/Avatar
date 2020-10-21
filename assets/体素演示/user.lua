camera.type("gaze")
camera.position(0, -6, 0)
engine.input("scale", 1, -1)

OnInput = function(name, value, arg1, arg2)
	if name == "fire" then
		local sx, sy, sz, dx, dy, dz = graphics.pickingRay(arg1, arg2)
		local node, x, y, z, mesh, face = scene.pick(sx, sy, sz, dx, dy, dz)
		if node ~= nil then
			local px, py, pz = scene.position(node)
			local vx, vy, vz, s, t, nx, ny, nz = scene.vertex(node, mesh, face, 0)
			px = px + nx
			py = py + ny
			pz = pz + nz
			local name = string.format("cube@%d@%d@%d", math.ceil(px), math.ceil(py), math.ceil(pz))
			scene.insert(nil, "geometry", name, { shape = "box", texture = "brick.jpg" })
			scene.position(name, px, py, pz)
			camera.target(px, py, pz)
			engine.log(name)
		else
			camera.target(0, 0, 0)
		end
	end
end