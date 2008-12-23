dinit(700,700)

-- stop and wait if there are any errors
show_differences = 1

-- erase boundary--don't pay attention to pixels on the boundary
eb = 1

function abs(x) if x<0 then return -x else return x end end

function byte_op(image,rx,ry,op)
   local direct = bytearray:new()
   narray.copy(direct,image)
   if op=="erode" then erode_rect(direct,rx,ry) end
   if op=="dilate" then dilate_rect(direct,rx,ry) end
   if op=="close" then close_rect(direct,rx,ry) end
   if op=="open" then open_rect(direct,rx,ry) end
   return direct
end

function bits_op(image,rx,ry,op)
    local bi = BitImage:new()
    bits.convert(bi,image)
    if op=="erode" then bits.erode_rect(bi,rx,ry) end
    if op=="dilate" then bits.dilate_rect(bi,rx,ry) end
    if op=="close" then bits.close_rect(bi,rx,ry) end
    if op=="open" then bits.open_rect(bi,rx,ry) end
    local result = bytearray:new()
    bits.convert(result,bi)
    return result
end

function lept_op(image,rx,ry,op,comp)
    local pix = lepton.of_array(image)
    lepton.pixInvert(pix,pix)
    pixn = lepton.pixConvertTo1(pix,128)
    lepton.pixDestroy(pix); pix = pixn
    local eroded = lepton.pixCreate(image:dim(0),image:dim(1),1)
    if comp then
       if op=="erode" then lepton.pixErodeCompBrick(eroded,pix,rx,ry) end
       if op=="dilate" then lepton.pixDilateCompBrick(eroded,pix,rx,ry) end
       if op=="close" then lepton.pixCloseCompBrick(eroded,pix,rx,ry) end
       if op=="open" then lepton.pixOpenCompBrick(eroded,pix,rx,ry) end
    else
       if op=="erode" then lepton.pixErodeBrick(eroded,pix,rx,ry) end
       if op=="dilate" then lepton.pixDilateBrick(eroded,pix,rx,ry) end
       if op=="close" then lepton.pixCloseBrick(eroded,pix,rx,ry) end
       if op=="open" then lepton.pixOpenBrick(eroded,pix,rx,ry) end
    end
    lepton.pixDestroy(pix)
    local result = bytearray:new()
    lepton.as_array(result,eroded)
    lepton.pixDestroy(eroded)
    return result
 end

function rle_op(image,rx,ry,op)
   local ri = RLEImage:new()
   rle.convert(ri,image)
   if op=="erode" then rle.erode_rect(ri,rx,ry) end
   if op=="dilate" then rle.dilate_rect(ri,rx,ry) end
   if op=="close" then rle.close_rect(ri,rx,ry) end
   if op=="open" then rle.open_rect(ri,rx,ry) end
   local result = bytearray:new()
   rle.convert(result,ri)
   return result
end

image = bytearray:new()

ops = {"erode", "dilate", "close", "open"}
files = {
    "images/hello.png", 
    "images/twocol.png",
    -- "images/empty.png", 
    -- "images/boundary.png",
    -- "images/test.png", 
}

function compare(truth,output,rx,ry,file,op,message)
   local delta = maxdifference(truth,output,abs(rx),abs(ry))
   if delta~=0 then 
      print("FAIL "..message,file,op,rx,ry)
      if show_differences then 
         dshow(truth,"a")
         dshow(output,"b")
         diff = bytearray:new()
         narray.copy(diff,truth)
         difference(diff,output,0,0)
         dshow(diff,"c")
         dwait()
      end
    end
   if not eb then
      local delta_b = maxdifference(truth,output,0,0)
      if delta_b~=0 then
         print("BOUNDARY "..message,file,op,rx,ry)
      end
   end
end

for i,file in ipairs(files) do
   for iv=0,1 do
       read_image_gray(image,file)
       make_boolean(image)
       if iv==1 then complement(image) end
       for j,op in ipairs(ops) do
          for rx=1,17 do
             for ry = 1,17 do
                truth = byte_op(image,rx,ry,op)
                if eb then erase_boundary(truth,abs(rx),abs(ry),0) end
                dshow(truth,"a")

                bi = bits_op(image,rx,ry,op)
                compare(truth,bi,rx,ry,file,op,"bits")
                if eb then erase_boundary(bi,abs(rx),abs(ry),0) end
                dshow(bi,"b")

                ri = rle_op(image,rx,ry,op)
                compare(truth,ri,rx,ry,file,op,"rle")
                if eb then erase_boundary(ri,abs(rx),abs(ry),0) end
                dshow(ri,"c")

                if nil then
                    -- The Leptonica ops have trouble with odd size
                    -- masks and use different boundary conditions,
                    -- so we just skip them.
                    if nil then
                       leptcomp = lept_op(image,rx,ry,op,1)
                       compare(truth,leptcomp,rx,ry,file,op,"leptcomp")
                       erase_boundary(leptcomp,abs(rx),abs(ry),0)
                       dshow(leptcomp,"d")
                    else
                       lept = lept_op(image,rx,ry,op,nil)
                       compare(truth,lept,rx,ry,file,op,"lept")
                       erase_boundary(lept,abs(rx),abs(ry),0)
                       dshow(lept,"d")
                    end
                end

                collectgarbage()
             end
          end
       end
    end
end
