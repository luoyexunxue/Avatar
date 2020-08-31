current = 1
postcount = 0
postlist = {
	'colorcircle',
	'glowcircle',
	'flowsmoke',
	'oilpainting',
}

function hideGUI()
	for key, value in ipairs(postlist) do
		gui.delete(value)
	end
end

if engine.info().platform == "android" then
	gui.scale(2);
end

graphics.background(0.16, 0.153, 0.16, 1.0)
local top = 0
for key, value in ipairs(postlist) do
	postcount = postcount + 1;
	engine.script(value..'.lua')
	gui.create(value, 'button', 'text='..value..';width=120;top='..top, function(evt, arg1, arg2)
		if evt == 1 then
			gui.modify(value, "backColor=0xFF00FFFF")
		elseif evt == 2 then
			gui.modify(value, "backColor=0x0D74D5FF")
			post.enable(value, post.enable(value) == false)
		end
	end)
	top = top + 40
end

OnInput = function(name, value, arg1, arg2)
	if name == 'key' then
		if value == 32 then
			post.enable(postlist[current], false)
			current = current + 1
			if current > postcount then current = 1 end
			post.enable(postlist[current], true)
		end
	end
end