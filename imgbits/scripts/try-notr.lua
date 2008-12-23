function time(f) start = user_time(); f() return user_time()-start end

dinit(700,700)

ri_orig = RLEImage:new()
rle.read(ri_orig,"images/twocol600.png")
ri = RLEImage:new()
temp = RLEImage:new()

for r=1,200 do
    ri:copy(ri_orig)
    t_notr = time(function() rle.erode_rect_notr(ri,r,r) end)
    rle.dshow(ri,"X")

    ri:copy(ri_orig)
    t_rle = time(function() rle.erode_rect(ri,r,r) end)
    rle.dshow(ri,"x")

    print(r,t_rle,t_notr)
    dwait()
end
