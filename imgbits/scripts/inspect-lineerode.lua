function time(f) start = user_time(); f() return user_time()-start end
pi = 3.1415926535897931
deg = pi/180
dinit(512,512)
bi_orig = BitImage:new()
bits.read(bi_orig,"test.png")
bi = BitImage:new()

for step=0,400,1 do
    print(step)
    bi:copy(bi_orig)
    bits.erode_line_by_rotation(bi,20,step*deg)
    bits.dshow(bi,"")
    dwait()
    bi:copy(bi_orig)
    bits.erode_line_by_skew(bi,20,step*deg)
    bits.dshow(bi,"")
    dwait()
end
