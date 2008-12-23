file = arg[1] or "images/cadastral.png"
bits.change_blit(1)

function time(f) start = user_time(); f() return user_time()-start end
function max(x,y) if x>y then return x else return y end end
function min(x,y) if x<y then return x else return y end end
function l(r) if r<1 then return 1 end if r%2==0 then return r+1 else return r end end
function drle(ri,where) rle.convert(temp_image,ri); dshow(temp_image,where or "") end
function dbits(bi,where) bits.convert(temp_image,bi); dshow(temp_image,where or "") end
function dlept(li,where) lepton.as_array(temp_image,li); dshow(temp_image,where or "") end
function rlencomps(li) local bs = rectanglearray:new(); rle.bounding_boxes(bs,li); return bs:length() end

image = bytearray:new()
read_image_gray(image,file)

ri_orig = RLEImage:new()
rle.convert(ri_orig,image)
rle.invert(ri_orig)
ri = RLEImage:new()

bi_orig = BitImage:new()
bits.convert(bi_orig,image)
bits.invert(bi_orig)
bi = BitImage:new()

pix_orig = lepton.of_array(image)
pix_orig = lepton.pixConvertTo1(pix_orig,128)
pix_orig = lepton.pixInvert(pix_orig,pix_orig)

-- those weird steps below 75 are the ones for which Leptonica has partially evaluated implementations
conditions = { 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 15, 20, 21, 30, 31, 40, 41, 50, 51, 75, 100, 150, 200}

for mode = 1,2 do
    for i,r in ipairs(conditions) do
        ri:copy(ri_orig)
        t_rle = time(function() 
                        if mode==1 then
                            rle.dilate_rect(ri,r,r)
                            rle.erode_rect(ri,r,r)
                        else
                            rle.erode_rect(ri,r,r)
                            rle.dilate_rect(ri,r,r)
                        end
                     end)
        ri:copy(ri_orig)
        t_rlet = time(function() 
                        if mode==1 then
                            rle.dilate_rect_runlength(ri,r,r)
                            rle.erode_rect_runlength(ri,r,r)
                        else
                            rle.erode_rect_runlength(ri,r,r)
                            rle.dilate_rect_runlength(ri,r,r)
                        end
                     end)
        bi:copy(bi_orig)
        t_bits = time(function()
                        if mode==1 then
                            bits.dilate_rect(bi,r,r)
                            bits.erode_rect(bi,r,r)
                        else
                            bits.erode_rect(bi,r,r)
                            bits.dilate_rect(bi,r,r)
                        end
                     end)
        pix = lepton.pixClone(pix_orig)
        t_lept = time(function()
                         if mode==1 then
                             lepton.pixDilateCompBrick(pix,pix,r,r)
                             lepton.pixErodeCompBrick(pix,pix,r,r)
                         else
                             lepton.pixErodeCompBrick(pix,pix,r,r)
                             lepton.pixDilateCompBrick(pix,pix,r,r)
                         end
                      end)
        lepton.pixDestroy(pix)
        ni = ri_orig:number_of_runs()
        no = ri:number_of_runs()
        print(string.format("%-30s %d %3d %10.6g %10.6g %10.6g %10.6g %6d %6d",
            file,r,mode,t_rle,t_rlet,t_bits,t_lept,ni,no))
        io.flush()
    end
end
