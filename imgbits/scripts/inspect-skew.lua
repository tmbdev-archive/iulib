function time(f) start = user_time(); f() return user_time()-start end
pi = 3.1415926535897931
deg = pi/180
dinit(512,512)
bi_orig = BitImage:new()
bits.read(bi_orig,"test.png")
bi = BitImage:new()

for round=0,1000 do
    for step=-500,500 do
        print(step)
        bits.dshow(bi_orig,"")
        bi:copy(bi_orig)
        bits.skew(bi,step*0.003,bi:dim(0)/2)
        bits.skew(bi,-step*0.003,bi:dim(0)/2,true)
        bits.dshow(bi,"")
    end
end
