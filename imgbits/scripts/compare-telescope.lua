file = arg[1] or "images/cadastral.png"

bits.change_blit(1)

function time(f) 
    start = user_time()
    counts = bits.get_blit_count()
    rops = lepton.count_pixRasterops;
    f() 
    return user_time()-start,bits.get_blit_count()-counts,lepton.count_pixRasterops-rops
end

bi_orig = BitImage:new()
bits.read(bi_orig,file)
bi = BitImage:new()

image = bytearray:new()
read_image_binary(image,file)
pix_orig = lepton.of_array(image)
pix_orig = lepton.pixConvertTo1(pix_orig,128)
pix_orig = lepton.pixInvert(pix_orig,pix_orig)

-- print("file size rle conv combo lept")
for r=2,200 do
    bi:copy(bi_orig)
    bits.use_telescope = 1
    t_bits,a = time(function() 
                    bits.erode_rect(bi,r,1)
                 end)
    bi:copy(bi_orig)
    bits.use_telescope = 3
    t_tele,b = time(function() 
                    bits.erode_rect(bi,r,1)
               end)
    bits.use_telescope = 1
    pix = lepton.pixClone(pix_orig)
    t_lept,_,c = time(function()
                     lepton.pixErodeCompBrick(pix,pix,r,1)
                  end)
    lepton.pixDestroy(pix)
    pix = lepton.pixClone(pix_orig)
    t_lept2 = time(function()
                     if r<=10 then
                         lepton.pixErodeBrickDwa(pix,pix,r,1)
                     else
                         lepton.pixErodeCompBrick(pix,pix,r,1)
                     end
                  end)
    lepton.pixDestroy(pix)
    print(string.format("%-30s %3d %10.6g %10.6g %10.6g %10.6g %d %d %d",
        file,r,t_bits,t_tele,t_lept,t_lept2,a,b,c))
    io.flush()
end
