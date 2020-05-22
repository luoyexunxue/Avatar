MessageArray = {
	"Welcome to avatar engine demo",
	"This engine is cross platform",
	"Is created by zrf (ye_luo@qq.com)",
	"Avatar is easy and full feature to use",
	"And it is always on the go"
}

--创建滚动标签
createMessage = function()
	local w, h = graphics.windowSize()
	gui.create("scrollLabel", "label", "width=400;height=28;top=10;left="..(w / 2 - 200)..";foreColor=0xF000F0FF")
end

--滚动标签
animateMessage = function(theta)
	local t = theta / 3.1415 * 2
	local c = string.format("0xF000F0%02X", math.floor(255 - math.abs((t - math.floor(t) - 0.5) * 510)))
	gui.modify("scrollLabel", "text="..MessageArray[math.floor(t % 5) + 1]..";foreColor="..c)
end

--创建帧率显示
createFps = function()
	local w, h = graphics.windowSize()
	gui.create("fps", "label", "height=28;top="..(h - 28)..";left=10;foreColor=0xF000F0FF")
end

--帧率显示更新
updateFps = function()
	gui.modify("fps", "text="..string.format("当前帧率 %.3f", engine.fps()))
end

--更新帧率标签位置
updateTipLocation = function()
	local w, h = gui.size()
	gui.modify("fps", "top=" .. (h - 28))
	gui.modify("scrollLabel", "left="..(w / 2 - 200))
end