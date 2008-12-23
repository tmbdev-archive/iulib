file = arg[1] or "images/cadastral.png"

bits.change_blit(1)

dofile("bitutil.lua")

image = bytearray:new()
read_image_gray(image,file)

ri_orig = RLEImage:new()
rle.convert(ri_orig,image)
rle.invert(ri_orig)
ri = RLEImage:new()

pix_orig = lepton.of_array(image)
pix_orig = lepton.pixConvertTo1(pix_orig,128)
pix_orig = lepton.pixInvert(pix_orig,pix_orig)

bi = BitImage:new()

-- conditions = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 14, 15, 20, 25, 30, 35, 40, 45, 50, 75, 100}
conditions = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 20, 21, 30, 31, 40, 41, 50, 51, 75, 100}
has_dwa = { 
    [1]=1, [2]=1, [3]=1, [4]=1, [5]=1, [6]=1, [7]=1, [8]=1, [9]=1, 
    [10]=1, [11]=1, [15]=1, [20]=1, [21]=1,
    [30]=1, [31]=1, [40]=1, [41]=1,
    [50]=1, [51]=1,
}

-- print("file size rle conv combo lept")
for i,r in ipairs(conditions) do
    ri:copy(ri_orig)
    t_rle = time(function() 
                    rle.erode_rect(ri,r,r)
                    rle.dilate_rect(ri,r,r)
                 end)
    t_conv = time(function() 
                rle.convert(bi,ri_orig)
                bits.erode_rect(bi,r,r) 
                bits.dilate_rect(bi,r,r) 
                rle.convert(ri,bi)
               end)
    pix = lepton.pixClone(pix_orig)
    t_lept = time(function()
                     lepton.pixErodeCompBrick(pix,pix,r,r)
                     lepton.pixDilateCompBrick(pix,pix,r,r)
                  end)
    lepton.pixDestroy(pix)
    pix = lepton.pixClone(pix_orig)
    t_lept2 = time(function()
                     if has_dwa[r] then
                         lepton.pixErodeBrickDwa(pix,pix,r,r)
                         lepton.pixDilateBrickDwa(pix,pix,r,r)
                     else
                         lepton.pixErodeCompBrick(pix,pix,r,r)
                         lepton.pixDilateCompBrick(pix,pix,r,r)
                     end
                  end)
    lepton.pixDestroy(pix)
    ni = ri_orig:number_of_runs()
    no = ri:number_of_runs()
    print(string.format("%-30s %3d %10.6g %10.6g %10.6g %10.6g %6d %6d",file,r,t_rle,t_conv,t_lept,t_lept2,ni,no))
    io.flush()
end
