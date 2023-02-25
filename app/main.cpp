#include <libsdl/sdl.hh>

int main() {
    libsdl::Context context;

    auto window =
            libsdl::create_window("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                  640, 480, SDL_WINDOW_SHOWN);

    auto screenSurface = libsdl::get_window_surface(window);

    libsdl::fill_rect(screenSurface, nullptr, libsdl::map_rgb(screenSurface->format, 0xFF, 0xFF, 0xFF));

    libsdl::update_window_surface(window);

    SDL_Event e;
    bool quit = false;
    while (!quit) {
        while (libsdl::poll_event(&e)) {
            if (e.type == SDL_QUIT) quit = true;
        }
    }

    return 0;
}
