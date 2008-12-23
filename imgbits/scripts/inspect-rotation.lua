function time(f) start = user_time(); f() return user_time()-start end
pi = 3.1415926535897931
deg = pi/180
dinit(512,512)
bi_orig = BitImage:new()
bits.read(bi_orig,"test.png")
bi = BitImage:new()

for test=0,1 do
    for angle=-pi/4,pi/4,0.001 do
        print(angle)
        if test==1 then bits.dshow(bi_orig,"") end
        bi:copy(bi_orig)
        bits.rotate(bi,angle)
        if test==1 then bits.rotate(bi,-angle) end
        bits.dshow(bi,"")
    end
    dwait()
end
