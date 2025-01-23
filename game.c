#include <stdio.h>
#include <raylib.h>
#include <math.h>
#include <stdlib.h>

#define BACKGROUND_COLOR (Color){186, 149, 127}
#define CAR_COLOR BLACK
#define CAMERA_FOLLOW_THRESH 400

int main() {
    int width = 1300;
    int height = 1000;
    InitWindow(width, height, "Racer");
    SetTargetFPS(60);

    int world_width = 5000;
    int world_height = 5000;

    Image soil_image = LoadImage("craftpix-889156-free-racing-game-kit/PNG/Background_Tiles/Soil_Tile.png");
    ImageRotateCW(&soil_image);
    Texture2D soil_texture = LoadTextureFromImage(soil_image);

    Image car_image = LoadImage("craftpix-889156-free-racing-game-kit/PNG/Car_1_Main_Positions/Car_1_01.png");
    ImageRotateCW(&car_image);
    Texture2D car_texture = LoadTextureFromImage(car_image);
    Rectangle car_texture_rec = {
        .x = 0,
        .y = 0,
        .width = car_texture.width,
        .height = car_texture.height,
    };

    int car_width = 80;
    int car_height = 150;
    float car_x = width/2-car_width/2;
    float car_y = height/2-car_height/2;
    float car_speed = 0;
    float car_max_speed = 7;
    int car_direction = -1;
    float car_angle = -90;
    float car_speedup = 10;
    float car_slowdown = 0.97;

    float drift_angle = car_angle;
    float drift_bias = 15;

    float steering = 0;
    float steering_speed = 2;
    float max_steering = 4;
    float steer_back_speed = 0.04;

    Camera2D camera = {
        .offset = (Vector2){0, 0},
        .target = (Vector2){0, 0},
        .rotation = 0,
        .zoom = 1.0,
    };

    while( ! WindowShouldClose() ) {
        float dt = GetFrameTime();

        BeginDrawing();
        ClearBackground( BACKGROUND_COLOR );

        BeginMode2D(camera);

        int tile_count_col = ceil( world_width / soil_texture.width );
        int tile_count_row = ceil( world_height / soil_texture.height );

        for( int x = 0; x < tile_count_col; x++ ) {
            for( int y = 0; y < tile_count_row; y++ ) {
                DrawTexture(soil_texture, x * soil_texture.width, y * soil_texture.height, WHITE);
            }
        }

        if( IsKeyDown( KEY_UP ) ) {
            car_direction = -1;
            car_speed += car_speedup * dt;
            if( car_speed > car_max_speed ) {
                car_speed = car_max_speed;
            }
        } else if( IsKeyDown( KEY_DOWN ) ) {
            car_direction = 1;
            car_speed -= car_speedup * dt;
            if( car_speed > car_max_speed ) {
                car_speed = car_max_speed;
            }
        } else {
            car_speed = car_slowdown * car_speed;
        }

        if( IsKeyDown( KEY_LEFT ) ) {
            steering -= steering_speed * dt * abs( car_speed );
            if( steering < -max_steering ) {
                steering = -max_steering;
            }
        } else if( IsKeyDown( KEY_RIGHT ) ) {
            steering += steering_speed * dt * abs( car_speed );
            if( steering > max_steering ) {
                steering = max_steering;
            }
        }

        steering = steering * (1 - steer_back_speed );

        car_angle += steering;

        drift_angle = (car_angle + drift_angle * drift_bias) / (1 + drift_bias);

        // Move car forward
        float radians = PI * car_angle / 180;
        car_x += car_speed * cosf( radians );
        car_y += car_speed * sinf( radians );

        // Move car to direction of drift
        radians = PI * drift_angle / 180;
        car_x += car_speed * cosf( radians );
        car_y += car_speed * sinf( radians );

        if( car_x + camera.offset.x < CAMERA_FOLLOW_THRESH ) {
            camera.offset.x = -car_x + CAMERA_FOLLOW_THRESH;
        }

        if( car_x + camera.offset.x > (width - CAMERA_FOLLOW_THRESH) ) {
            camera.offset.x = -car_x + (width - CAMERA_FOLLOW_THRESH);
        }

        if( car_y + camera.offset.y < CAMERA_FOLLOW_THRESH ) {
            camera.offset.y = -car_y + CAMERA_FOLLOW_THRESH;
        }

        if( car_y + camera.offset.y > (height - CAMERA_FOLLOW_THRESH) ) {
            camera.offset.y = -car_y + (height - CAMERA_FOLLOW_THRESH);
        }

        if( camera.offset.x > 0 ) {
            camera.offset.x = 0;
        }

        if( camera.offset.x < -world_width+width+CAMERA_FOLLOW_THRESH ) {
            camera.offset.x = -world_width+width+CAMERA_FOLLOW_THRESH;
        }

        if( camera.offset.y > 0 ) {
            camera.offset.y = 0;
        }

        if( camera.offset.y < -world_height+height+CAMERA_FOLLOW_THRESH ) {
            camera.offset.y = -world_height+height+CAMERA_FOLLOW_THRESH;
        }

        Rectangle car_rec = {
            .x = car_x,
            .y = car_y,
            .width = car_height,
            .height = car_width,
        };
        Vector2 car_origin = {
            .x = car_height/2,
            .y = car_width/2,
        };
        DrawTexturePro(car_texture, car_texture_rec, car_rec, car_origin, car_angle, WHITE);

        EndDrawing();
    }

    UnloadImage(car_image);
    UnloadTexture(car_texture);

    UnloadImage(soil_image);
    UnloadTexture(soil_texture);

    CloseWindow();

    return 0;
}