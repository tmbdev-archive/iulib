dinit(700,700)

bi = BitImage:new()
mask = BitImage:new()

bits.read(bi,"image.png")
bits.dshow(bi,"")
dwait()

for i=1,200 do
    bits.circ_mask(mask,i)
    start = bits.get_blit_count()
    bits.read(bi,"image.png")
    bits.erode_runs(bi,mask,i,i)
    bits.dshow(bi,"")
    nruns = bits.get_blit_count()-start

    start = bits.get_blit_count()
    bits.read(bi,"image.png")
    bits.erode_mask(bi,mask,i,i)
    bits.dshow(bi,"")
    nmask = bits.get_blit_count()-start
    print(i,nruns,nmask)
end
