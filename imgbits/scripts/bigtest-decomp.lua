image = bytearray:new()
read_image_gray(image,"images/twocol.png")

timer = Timer:new()

bits = BitImage:new()
temp = BitImage:new()

bits.convert(bits,image)

count=0
function try_offsets(dx,dy)
      temp:copy(bits)
      bits.erode_rect(temp,dx,dy)
      fc = bits.count_rect(temp,0,0,temp:dim(0),temp:dim(1))
      temp:copy(bits)
      bits.erode_rect_bruteforce(temp,dx,dy)
      sc = bits.count_rect(temp,0,0,temp:dim(0),temp:dim(1))
      if fc~=sc then
	 print("FAIL",dx,dy,fc,sc)
      end
      count = count+1
end

try_offsets(0,0)
try_offsets(1,1)
try_offsets(8,8)
try_offsets(32,1)
for offset=0,16 do try_offsets(0,offset) end
for offset=0,16 do try_offsets(offset,0) end
print("tested "..count.." different shifts")
