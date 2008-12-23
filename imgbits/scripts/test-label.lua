image = bytearray:new()
read_image_gray(image,"images/twocol.png")
bi = BitImage:new()
bits.convert(bi,image)
print(bi:megabytes())
rli = RLEImage:new()
rle.convert(rli,image)
print(rli:number_of_runs())
print(rli:megabytes())
boxes = rectanglearray:new()
n = rle.bounding_boxes(boxes,rli)
print(n)
for i=0,10 do
   b = boxes:at(i)
   print(b.x0,b.y0,b.x1,b.y1)
end
blackhist = floatarray:new(1000)
whitehist = floatarray:new(1000)
rle.runlength_statistics(whitehist,blackhist,rli)
function write_hist(file,data)
    stream = io.open(file,"w")
    for i=0,data:length()-1 do
       stream:write(""..i.." "..data:at(i).."\n")
    end
    stream:close()
end
write_hist("blackhist",blackhist)
write_hist("whitehist",whitehist)
debug_array(whitehist)
debug_array(blackhist)

