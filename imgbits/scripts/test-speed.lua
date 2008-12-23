function time(f) start = user_time(); f() return user_time()-start end

bi = BitImage:new()
bits.read(bi,"images/cadastral.png")
bi2 = BitImage:new()

for blit=0,3 do
    bits.change_blit(blit)
    bi2:copy(bi)
    print(blit,time(function () bits.Or(bi2,bi,17,19) end))
end
