dinit(700,700)

temp_image = bytearray:new()
function abs(x) if x<0 then return -x else return x end end
function drle(ri,where) rle.convert(temp_image,ri); dshow(temp_image,where or "") end

image = bytearray:new()
read_image_gray(image,"images/hello.png")

local ri = RLEImage:new()
rle.convert(ri,image)

drle(ri); dwait()
rle.And(ri,ri,23,23)
drle(ri); dwait()

