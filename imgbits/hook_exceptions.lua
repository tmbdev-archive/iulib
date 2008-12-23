function try_hook(f)
	output('try {')
end

function catch_hook(f)
	output('} catch(const char *s) {')
	output(' return luaL_error(tolua_S, "%s", s);')
	output('}')
end
