dinit(700,700)

function abs(x) if x<0 then return -x else return x end end

function byte_op(image,rx,ry,op)
   local direct = bytearray:new()
   copy(direct,image)
   if op=="erode" then erode_rect(direct,rx,ry) end
   if op=="dilate" then dilate_rect(direct,rx,ry) end
   if op=="close" then close_rect(direct,rx,ry) end
   if op=="open" then open_rect(direct,rx,ry) end
   return direct
end

function bits.op(image,rx,ry,op)
    local bits = BitImage:new()
    bits.convert(bits,image)
    if op=="erode" then bits.erode_rect(bits,rx,ry) end
    if op=="dilate" then bits.dilate_rect(bits,rx,ry) end
    if op=="close" then bits.close_rect(bits,rx,ry) end
    if op=="open" then bits.open_rect(bits,rx,ry) end
    local result = bytearray:new()
    bits.convert(result,bits)
    return result
end

function lept_op(image,rx,ry,op,comp)
    local pix = lepton.of_array(image)
    lepton.pixInvert(pix,pix)
    pix = lepton.pixConvertTo1(pix,128)
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
    local result = bytearray:new()
    lepton.as_array(result,eroded)
    return result
 end

function rle.op(image,rx,ry,op)
   local rle = RLEImage:new()
   rle.convert(rle,image)
   if op=="erode" then rle.erode_rect(rle,rx,ry) end
   if op=="dilate" then rle.dilate_rect(rle,rx,ry) end
   if op=="close" then rle.close_rect(rle,rx,ry) end
   if op=="open" then rle.open_rect(rle,rx,ry) end
   local result = bytearray:new()
   rle.convert(result,rle)
   return result
end

image = bytearray:new()
diff = bytearray:new()

ops = {"erode", "dilate", "close", "open"}
files = {"images/hello.png", "images/test.png", "images/empty.png"}

for i,file in ipairs(files) do
   read_image_gray(image,file)
   for j,op in ipairs(ops) do
      for rx=1,19,2 do
	 for ry = 1,19,2 do
	    truth = byte_op(image,rx,ry,op)
	    dshow(truth,"a")

	    bits = bits.op(image,rx,ry,op)
	    dshow(bits,"b")

	    copy(diff,truth)
	    difference(diff,bits,0,0)

	    dshow(diff); dwait()

	    collectgarbage()
	 end
      end
   end
end
