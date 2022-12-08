#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define WIDTH 900
#define HEIGHT 600

#define SEED_COUNT 10
#define SEED_SIZE 2

typedef struct Point{
    int x;
    int y;
} Point;

typedef struct Color{
    int r;
    int g;
    int b;
} Color;

typedef Point Seeds[SEED_COUNT];
typedef Color Canvas[HEIGHT][WIDTH];


static Canvas canvas;

static Color palette[] = {{000, 255, 255}, {255, 000, 255},
                         {255, 000, 255}, {255, 255, 000},
                         {255, 000, 000}, {000, 255, 000},
                         {000, 000, 255}, {69, 42, 10},
                         {43, 23, 236}, {146, 42, 10},
                         {156, 169, 255}, {169, 42, 10},
                         {176, 8, 43}, {69, 122, 169},
                         {43, 0, 255}, {169, 142, 12},
                         {236, 12, 255}, {45, 152, 34},
                         };
static Seeds seeds;

void saveToPPM(const char *filepath)
{
    FILE *file = fopen(filepath, "wb");
    if(!file) exit(1);

    fprintf(file, "P6\n%i %i 255\n", WIDTH, HEIGHT);

    int y; for(y = 0; y < HEIGHT; ++y){
        int x; for(x = 0; x < WIDTH; ++x){
            Color pixel = canvas[y][x];
            uint8_t bytes[3] = {
                pixel.r,
                pixel.g,
                pixel.b
            };
            fwrite(bytes, sizeof(bytes), 1, file);
        }
    }

    fclose(file);
}

// Euclidean distance
int edist(int x0, int y0, int x1, int y1)
{
    int dx = x0 - x1;
    int dy = y0 - y1;
    return dx*dx + dy*dy;
}


// Manhattan distance
int mdist(int x0, int y0, int x1, int y1)
{
    int dx = x0 - x1;
    int dy = y0 - y1;
    dx = dx<0?dx*-1:dx;
    dy = dy<0?dy*-1:dy;
    return dx + dy;
}

void render_voronoi(void)
{
    int y; for(y = 0; y < HEIGHT; ++y) {
        int x; for(x = 0; x < WIDTH; ++x) {
            int j = 0;
            int i; for(i = 1; i < SEED_COUNT; ++i) {
                if(edist(seeds[i].x, seeds[i].y, x, y) < edist(seeds[j].x, seeds[j].y, x, y)) {
                    j = i;
                }
            }
            canvas[y][x] = palette[j%SEED_COUNT];
        }
    }
}

void render_seeds(void)
{
    int i; for(i = 0; i < SEED_COUNT; ++i) {
        fill_circle(seeds[i].x, seeds[i].y, SEED_SIZE, (Color) {18, 18, 18});
    }
}

void fill_circle(int cx, int cy, int r, Color color)
{
    int x0 = cx - r; int y0 = cy - r;
    int x1 = cx + r; int y1 = cy + r;
    int x; for(x = x0; x <= x1; ++x){
        int y; for(y = y0; y <= y1; ++y) {
            if(edist(cx, cy ,x, y) <= r*r) {
                canvas[y][x] = color;
            }
        }
    }
}

void generate_random_seeds(void)
{
    int i; for(i = 0; i < SEED_COUNT; ++i) {
        seeds[i].x = rand()%(WIDTH-(SEED_SIZE*2))+SEED_SIZE;
        seeds[i].y = rand()%(HEIGHT-(SEED_SIZE*2))+SEED_SIZE;
    }
}

void fill_background(Color color)
{
    int y; for(y = 0; y < HEIGHT; ++y){
        int x; for(x = 0; x < WIDTH; ++x){
            canvas[y][x] = color;
        }
    }
}

int main(void)
{
    srand(time(NULL));
    fill_background((Color) {0, 0, 0});
    generate_random_seeds();
    render_voronoi();
    render_seeds();
    saveToPPM("canvas2.ppm");
    return 0;
}
