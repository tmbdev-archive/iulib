function time(f) start = user_time(); f() return user_time()-start end
function max(x,y) if x>y then return x else return y end end
function min(x,y) if x<y then return x else return y end end
function l(r) if r<1 then return 1 end if r%2==0 then return r+1 else return r end end
function drle(ri,where) rle.convert(temp_image,ri); dshow(temp_image,where or "") end
function dbits(bi,where) bits.convert(temp_image,bi); dshow(temp_image,where or "") end
function dlept(li,where) lepton.as_array(temp_image,li); dshow(temp_image,where or "") end
function rlencomps(li) local bs = rectanglearray:new(); rle.bounding_boxes(bs,li); return bs:length() end

