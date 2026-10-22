//
//  test_main.c
//  just test the basics of the engine really.
//  lathe_tools
//
//  Created by Christopher Scott on 9/20/26.
//

#include "math/lathe_mat3.h"
#include "math/lathe_vec3.h"
#include "memory/lathe_memory.h"
#include "system/lathe_controller.h"
#include "system/lathe_logger.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h> 
#include "platform/platform.h"
#include "render/renderer.h"
#include <math.h>

uint64_t time_freq;
constexpr unsigned int screen_width = 800;
constexpr unsigned int screen_height = 600;
//Can just make this a simple game
int main(void) {
        
    PlatformState platform = {};
    timer_init();
    
    if (!platform_start(&platform, nullptr,0,0,screen_width,screen_height)) {
        LATHE_FATAL("Platform was not able to start!", nullptr);
        return EXIT_FAILURE;
    }
    
    if(renderer_start(&platform)){
        LATHE_INFO("Renderer was started", nullptr);
    }else{
        LATHE_FATAL("RENDERER was not able to start!", nullptr);
        return EXIT_FAILURE;
    }
    
    
    //Pre loop setup
    double previous_frame = lathe_get_time();
    double fps_timer = previous_frame;
    uint64_t fps_frames = 0;
    const double target_fps = 75.0;
    const double target_frame_time = 1.0 / target_fps;
    [[maybe_unused]] float t = 0.0f;
    if(!shader_init("./core/assets/shaders/uniform.vert", "./core/assets/shaders/uniform.frag")){
          LATHE_FATAL("Shaders failed to init!", nullptr);
          return EXIT_FAILURE;
    }
    
    //Main Loop
    while (platform_handle_os_events(&platform)) { 
        double frame_start = lathe_get_time();
        
        
        //update
        controller_print_buttons();
        
        //render a new frame
        float red   = (sinf(t)        + 1.0f) / 2.0f;
        float green = (sinf(t + 2.0f) + 1.0f) / 2.0f;
        float blue  = (sinf(t + 4.0f) + 1.0f) / 2.0f;   
        //float alpha = 1.0f;
        
        lathe_vec3 color = {{red,green,blue}};
        
        lathe_vec3 color_red = {{red,0.0f,0.0f}};
        lathe_vec3 color_blue = {{0.0f,0.0f,blue}};
        lathe_vec3 color_green = {{0.0f,green,0.0f}};
        renderer_begin_frame();
        
        lathe_vec3 a = {{-0.5f, -0.5f, 0.0f}};
        lathe_vec3 b = {{0.5f, -0.5, 0.0f}};
        lathe_vec3 c = {{0.0f, 0.5f, 0.0f}};
        
        renderer_clear(0.0f,0.0f,0.0f,1.0f);//macro for these pls...
        renderer_render_triangle(&a,&b,&c,&color);
        
        
        renderer_render_triangle_per_vertex(&a,&color_red,&b,&color_blue,&c,&color_green);
        
        
        renderer_end_frame();
        
        t += 0.01f;
        fps_frames++;
        
        double now = lathe_get_time();
        double fps_elapsed = now - fps_timer;
        
        // Print FPS 30 times per second
        //turn into method...
        if (fps_elapsed >= 1.0 / 30.0) {
            double fps = (double)fps_frames / fps_elapsed;
            LATHE_INFO_F(nullptr,"FPS: %.2f\r FPS:\r", fps);
            fps_frames = 0;
            fps_timer = now;
        }
        
        //limit framerate
        double frame_time = lathe_get_time() - frame_start;
        
        if (frame_time < target_frame_time) {
            double remaining = target_frame_time - frame_time;
            usleep((useconds_t)((uint64_t)(remaining * 1000000.0)));
        }
        
        previous_frame = frame_start;
    }  
    
    renderer_shutdown();
    platform_shutdown(&platform);
    LATHE_INFO("Ending Lathe Simple Run Test",nullptr);
    
    LATHE_INFO("Test Completed",nullptr);
}
