#ifndef SDL_lines__
#define SDL_lines__
namespace SDL_extensions {
    void SDL_draw_line(SDL_Surface *s, 
		       int x1, int y1, 
		       int x2, int y2, 
		       Uint32 color);
}
#endif
