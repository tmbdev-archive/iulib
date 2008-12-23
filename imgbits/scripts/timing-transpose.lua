function time(f) start = user_time(); f() return user_time()-start end
bi = BitImage:new()
for state=0,1 do
    bits.bits_transpose_slow = state
    bits.read(bi,"images/cadastral.png")
    print(state,time(function() for i=1,20 do bits.transpose(bi) end end))
end
