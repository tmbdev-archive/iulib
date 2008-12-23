file = arg[1]
print("file",file)

-- dinit(900,700)

temp_image = bytearray:new()
function time(f) start = user_time(); f() return user_time()-start end
function mx(x,y) if x>y then return x else return y end end
function l(r) if r<1 then return 1 end if r%2==0 then return r+1 else return r end end
function drle(ri,where) rle.convert(temp_image,ri); dshow(temp_image,where or "") end
function dbits(bits,where) bits.convert(temp_image,bits); dshow(temp_image,where or "") end
function dlept(limage,where) lepton.as_array(temp_image,limage); dshow(temp_image,where or "") end

image = bytearray:new()
read_image_gray(image,file)
binarize_by_range(image)
boxes = rectanglearray:new(10000)

h0 = intarray:new(500)
h1 = intarray:new(500)
v0 = intarray:new(500)
v1 = intarray:new(500)

ri = RLEImage:new()
rle.convert(ri,image)
drle(ri)
dwait()
rle.bounding_boxes(boxes,ri)
print("nboxes",boxes:length())
rle.runlength_peaks(h0,h1,v0,v1,ri)
rx = 50; ry = 50
pcall(function() rx = 5*h0:at(0); ry = 5*v0:at(0) end)
pcall(function() rx = 1.5*h0:at(1); ry = 1.5*v0:at(1) end)
print("mask",rx,ry)
t_rle = time(function() 
		rle.erode_rect(ri,rx,ry)
		rle.invert(ri)
		boxes:clear()
		rle.bounding_boxes(boxes,ri)
	     end)
drle(ri,"x")
print("rle",boxes:length()-1)

pix = lepton.of_array(image)
pix = lepton.pixConvertTo1(pix,128)
lepton.pixInvert(pix,pix)
t_lept = time(function()
		 lepton.pixErodeCompBrick(pix,pix,l(rx),l(ry))
		 lepton.pixInvert(pix,pix)
		 boxes:clear()
		 lepton.pixConnComp(boxes,pix,4)
	      end)
dlept(pix,"X")
print("lept",boxes:length())
print("times",t_rle,t_lept)

dwait()
