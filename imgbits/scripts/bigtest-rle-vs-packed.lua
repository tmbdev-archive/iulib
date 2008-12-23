-- uncomment this if you want to see the output
-- dinit(900,700)

dofile("bitutil.lua")

file = arg[1] or "images/twocol600.png"
stderr = io.open("/dev/stderr","w")
stderr:write("# file "..file.."\n")

image = bytearray:new()
read_image_gray(image,file)
temp_image = bytearray:new()

timer = Timer:new()

bi_orig = BitImage:new()
bi = BitImage:new()

ri_orig = RLEImage:new()
ri = RLEImage:new()

bits.convert(bi_orig,image)
rle.convert(ri_orig,image)

pix_orig = lepton.of_array(image)
pix_orig = lepton.pixConvertTo1(pix_orig,128)
lepton.pixInvert(pix_orig,pix_orig)
pix_temp = lepton.pixCreate(image:dim(0),image:dim(1),1)

function try_scale(rx,ry)
    ri:copy(ri_orig)
    drle(ri,"a")
    tr = time(function() rle.erode_rect(ri,rx,ry) end)
    drle(ri,"b")
    ri:copy(ri_orig)
    trc = time(function() 
                rle.convert(bi,ri)
                bits.erode_rect(bi,rx,ry) 
                rle.convert(ri,bi)
               end)
    bi:copy(bi_orig)
    tb = time(function() bits.erode_rect(bi,rx,ry) end)
    dbits(bi,"c")
    tl = time(function() lepton.pixErodeCompBrick(pix_temp,pix_orig,l(rx),l(ry)) end)
    dlept(pix_temp,"d")
    nc = rlencomps(ri)
    nr = ri:number_of_runs()
    print(string.format("%3d %3d %10.5g %10.5g %10.5g %10.5g %10d %10d",rx,ry,tr,trc,tb,tl,nc,nr))
    io.flush()
    dwait()
end

for r=2,250,2 do
   try_scale(r,r)
end
