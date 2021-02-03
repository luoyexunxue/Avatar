shader.create('silk', 'silk.vert', 'silk.frag')
scene.material('untitled', 2, {color='purple', shader='silk'})
current_animation = 0
OnInput = function(name, param)
	if name == "fire" then
		local x, y, z, dx, dy, dz = graphics.pickingRay(param.x, param.y)
		local node = scene.pick(x, y, z, dx, dy, dz)
		if node == 'untitled' then
			current_animation = current_animation + 1
			local name = scene.handle(node, 'GetAnimationName', {index=current_animation})
			if name == '' then current_animation = 0 end
			scene.handle(node, 'StartAnimation', {name=name, transition=1})
		end
	end
end