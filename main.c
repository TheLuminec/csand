#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>
#include <time.h>

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 640
#define SANDSIZE 3
#define WIDTH (SCREEN_WIDTH / SANDSIZE)
#define HEIGHT (SCREEN_HEIGHT / SANDSIZE)
#define GAP 0.0f
#define BRUSHSIZE 10

SDL_Window *window;
SDL_Renderer* renderer;
int running;
bool clicked = false;

static bool sand[WIDTH][HEIGHT];
static int flash[WIDTH][HEIGHT];

void setup();
void draw();
void loop();
void randomize();
void ruleStep();
void flclear();
void flashsand();
void click(int x, int y, int s);
void rule(int x, int y);
void conRule(int x, int y);
bool boundCheck(int x, int y);

int main() {
    setup();

    randomize();

    loop();
    

    // Clean up
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}

void loop() {
    SDL_Event event;
    unsigned int a = SDL_GetTicks();
    unsigned int b = SDL_GetTicks();
    double delta = 0;

    // Main loop
    while (running) {

        a = SDL_GetTicks();
        delta = a - b;

        if (delta > 1000 / 24.0) {
            while (SDL_PollEvent(&event)) {
                if (event.type == SDL_QUIT) {
                    running = 0;
                }
                if (event.type == SDL_MOUSEBUTTONDOWN) {
                    clicked = true;	
                }
                else if (event.type == SDL_MOUSEBUTTONUP) {
                    clicked = false;
                }
                if (clicked) {
                    click(event.button.x, event.button.y, BRUSHSIZE);
                }
            }

            b = a;

            draw();
            ruleStep();
        }
    }
}

void click(const int x, const int y, int s) {
	int i = x / SANDSIZE;
	int j = y / SANDSIZE;
	if (boundCheck(i, j)) {
		sand[i][j] = !sand[i][j];
	}

    for (int k = 0; k < s - 1; k++) {
        click(x + rand()%s - s/2, y + rand()%s - s/2, 0);
    }
}

void ruleStep() {
    flclear();
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            rule(i, j);
        }
    }
    flashsand();
}

void rule(const int x, const int y) {
    if (sand[x][y]) {
        if (y == HEIGHT - 1) {
            flash[x][y] = 1;
        }
        else if (sand[x][y + 1]) {
            bool l = (x - 1 == -1 || flash[x-1][y] == 1) ? true : sand[x - 1][y + 1];
            bool r = (x + 1 == WIDTH || flash[x+1][y] == 1) ? true : sand[x + 1][y + 1];
            if (!l && !r) {
                int r = rand() % 2;
                if (r)
                    flash[x - 1][y + 1] = 1;
                else
                    flash[x + 1][y + 1] = 1;
            }
            else if (!l) {
                flash[x - 1][y + 1] = 1;
            }
            else if (!r) {
                flash[x + 1][y + 1] = 1;
            }
            else {
                flash[x][y] = 1;
            }
        }
        else {
            flash[x][y + 1] = 1;
        }
    }
}

void conRule(const int x, const int y) {
    int n = 0;
    n += (boundCheck(x, y - 1) && sand[x][y - 1]);
	n += (boundCheck(x - 1, y - 1) && sand[x - 1][y - 1]);
	n += (boundCheck(x + 1, y - 1) && sand[x + 1][y - 1]);
	n += (boundCheck(x - 1, y) && sand[x - 1][y]);
	n += (boundCheck(x + 1, y) && sand[x + 1][y]);
	n += (boundCheck(x - 1, y + 1) && sand[x - 1][y + 1]);
	n += (boundCheck(x, y + 1) && sand[x][y + 1]);
	n += (boundCheck(x + 1, y + 1) && sand[x + 1][y + 1]);

    if (n > 3) {
        flash[x][y] = false;
    }
    else if (n < 2) {
        flash[x][y] = false;
    }
    else if (n == 3) {
        flash[x][y] = true;
    }
    else if(n == 2){
        flash[x][y] = sand[x][y];
    }
}

bool boundCheck(const int x, const int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT)
        return false;
	return true;
}

void flclear() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
			flash[i][j] = 0;
        }
    }
}

void flashsand() {
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
			sand[i][j] = (flash[i][j] != 0) ? 1 : 0;
        }
    }
}

void draw() {
    // Clear the screen
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black
    SDL_RenderClear(renderer);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    for (int i = 0; i < WIDTH; i++) {
        for (int j = 0; j < HEIGHT; j++) {
            if (sand[i][j]) {
                float x = i * SANDSIZE + GAP;
                float y = j * SANDSIZE + GAP;
                SDL_FRect rect = { x, y, SANDSIZE - 2 * GAP, SANDSIZE - 2 * GAP };
                SDL_RenderFillRectF(renderer, &rect);
            }
        }
    }

    // Present the renderer
    SDL_RenderPresent(renderer);
}

void setup() {
    running = 1;
    srand(time(NULL));

    ///Render setup
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL_Init Error: %s\n", SDL_GetError());
        return 1;
    }

    // Create an SDL window
    window = SDL_CreateWindow(
        "Csand",                   // Title of the window
        SDL_WINDOWPOS_CENTERED,          // Initial x position
        SDL_WINDOWPOS_CENTERED,          // Initial y position
        SCREEN_WIDTH,                             // Width of the window
        SCREEN_HEIGHT,                             // Height of the window
        SDL_WINDOW_SHOWN                 // Flags for window creation
    );

    if (!window) {
        printf("SDL_CreateWindow Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create an SDL renderer
    renderer = SDL_CreateRenderer(
        window,                          // Associated window
        -1,                              // Rendering driver (-1 for default)
        SDL_RENDERER_ACCELERATED         // Use hardware acceleration
    );

    if (!renderer) {
        printf("SDL_CreateRenderer Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
}

void randomize() {
    for (int i = 0; i < WIDTH; i++) {
		for (int j = 0; j < HEIGHT; j++) {
			sand[i][j] = rand() % 2;
			flash[i][j] = false;
		}
    }
}