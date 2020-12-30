gElementTop = 0
gElementVisible = false;

--��Ӳ��������
gui.create('panel', 'panel', 'width=100;height=500;top=10;left=10;backColor=#0000;visible=false')

--��Ӳ��������
gui.create("ctrl", "button", "width=32;height=32;top=10;left=10;image=arrow_right.png;backColor=0xFFFFFFFF", function(evt, arg1, arg2)
	if evt == 0 then
		gElementVisible = not gElementVisible
		gui.modify('panel', "visible="..tostring(gElementVisible))
		if gElementVisible then gui.modify("ctrl", "image=arrow_left.png;left=120")
		else gui.modify("ctrl", "image=arrow_right.png;left=10") end
	elseif evt == 1 then
		gui.modify("ctrl", "backColor=0xFF00FFFF")
	elseif evt == 2 then
		gui.modify("ctrl", "backColor=0xFFFFFFFF")
	end
end)

--��Ӱ�ť
addButton = function(name, text, onClick)
	gui.create(name, "button", "parent=panel;width=100;height=42;borderRadius=21;top="..gElementTop..";text="..text, function(evt, arg1, arg2)
		if evt == 0 then onClick()
		elseif evt == 1 then gui.modify(name, "backColor=0xFF00FFFF")
		elseif evt == 2 then gui.modify(name, "backColor=0x0D74D5FF")
		end
	end)
	gElementTop = gElementTop + 50
end

--��Ӹ�����
addTrackBar = function(name, value, onChange)
	gui.create(name, "trackbar", "parent=panel;width=100;height=42;top="..gElementTop..";percent=true;value="..value, function(evt, arg1, arg2)
		local percent = arg1;
		if percent < 0 then percent = 0
		elseif percent > 100 then percent = 100
		end
		if evt == 1 then
			gui.modify(name, "value="..percent)
			onChange(percent)
		elseif evt == 3 then
			gui.modify(name, "value="..percent)
			onChange(percent)
		end
	end)
	gElementTop = gElementTop + 50
end

--��ӱ༭��
addEditbox = function(name, text)
	gui.create(name, "editbox", "parent=panel;width=100;height=42;top="..gElementTop..";text="..text..";align=center")
	gElementTop = gElementTop + 50
end

--����panel�߶�
updatePanelSize = function()
	local w, h = gui.size()
	gui.modify("panel", "height="..(h - 10 - 28))
end