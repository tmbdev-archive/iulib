-- file = "cadastral-sm.png"
file = "images/cadastral.png"

-- dinit(800,800)

temp_image = bytearray:new()
function time(f) start = user_time(); f() return user_time()-start end
function mx(x,y) if x>y then return x else return y end end
function l(r) if r<1 then return 1 end if r%2==0 then return r+1 else return r end end
function drle(ri,where) rle.convert(temp_image,ri); dshow(temp_image,where or "") end
function dbits(bits,where) bits.convert(temp_image,bits); dshow(temp_image,where or "") end
function dlept(limage,where) lepton.as_array(temp_image,limage); dshow(temp_image,where or "") end

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

print("size rle lept combo")
for r=2,100,2 do
    ri:copy(ri_orig)
    t_rle = time(function() 
                    rle.erode_rect(ri,r,r)
                    rle.dilate_rect(ri,r,r)
                 end)
    pix = lepton.pixClone(pix_orig)
    t_lept = time(function()
                     lepton.pixErodeCompBrick(pix,pix,r,r)
                     lepton.pixDilateCompBrick(pix,pix,r,r)
                  end)
    lepton.pixDestroy(pix)
    t_combo = time(function() 
                rle.convert(bi,ri_orig)
                bits.erode_rect(bi,r,r) 
                rle.convert(ri,bi)
               end)
    print(r,t_rle,t_lept,t_combo)
    io.flush()
end
