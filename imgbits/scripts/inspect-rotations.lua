function time(f) start = user_time(); f() return user_time()-start end
dinit(512,512)
bi_orig = BitImage:new()
bits.read(bi_orig,"images/hello.png")
bi = BitImage:new()

bi:copy(bi_orig)
bits.flip_v(bi)
bits.dshow(bi,"")
dwait()

bi:copy(bi_orig)
bits.transpose(bi)
bits.dshow(bi,"")
dwait()

for step=0,720,90 do
    bi:copy(bi_orig)
    print(step)
    bits.rotate_rect(bi,step)
    bits.dshow(bi,"")
    dwait()
end
for step=-400,400 do
    bi:copy(bi_orig)
    bits.rotate(bi,step*1.5/90)
    bits.dshow(bi,"")
    dwait()
end
