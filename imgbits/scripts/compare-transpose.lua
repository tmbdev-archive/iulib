file = arg[1] or "images/cadastral.png"

dofile("bitutil.lua")

ri_orig = RLEImage:new()
rle.read(ri_orig,file)
ri = RLEImage:new()

loops = 10

rle.transpose(ri,ri_orig)
rle.transpose_table(ri,ri_orig)
t_rle = time(function() 
                for i=1,loops do
                    rle.transpose(ri,ri_orig)
                end
             end)
t_table = time(function() 
                for i=1,loops do
                    rle.transpose_table(ri,ri_orig)
                end
             end)
print(string.format("%-30s %10.6g %10.6g",file,t_rle/loops,t_table/loops))
io.flush()
