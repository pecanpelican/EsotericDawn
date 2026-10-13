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

int main(void) {
      char buffer[FILENAME_MAX];
    
    if (getcwd(buffer, FILENAME_MAX) != NULL) {
        printf("Your current working directory is:\n%s\n", buffer);
    } else {
        perror("getcwd() error");
    }
        LATHE_INFO("Starting Vec3 Test...",nullptr);
    lathe_vec3 v = {{5, 25, 5}};
    lathe_vec3 v_negated = {{-5,-25,-5}};
    v = vec3_negate(&v);
    assert(vec3_eq(&v,&v_negated));
    
    lathe_vec3 n = vec3_normalize(&v);
    vec3_print(&n, nullptr);
    lathe_vec3 d = vec3_add(&v, &v);
    vec3_print(&d, nullptr);
    
    //angle between <1,2,3> and <4,-1,2> should be 0.46666
    lathe_vec3 a1 = {{1,2,3}};
    lathe_vec3 a2 = {{4,-1,2}};
    printf("%f was the angle in radians\n" ,vec3_angle(&a1,&a2));
    
    //TODO: Test this function  later
    //lathe_vec3 vec3_projection(const lathe_vec3 *a, const lathe_vec3 *b);
    LATHE_INFO("Ending Vec3 Test",nullptr);
    
    LATHE_INFO("Starting Mat3 Test...",nullptr);
    FILE *fptr;
    fptr = fopen("LogFile", "a");
    
    lathe_mat_3x3 m = create_3x3_matrix_vec3(&d, &d, &d);
    lathe_mat_3x3 m2 = create_3x3_matrix_vec3(&d, &d, &d);
    
    mat_3x3_print(&m, NULL);
    mat_3x3_print(&identity_3x3, fptr);
    
    lathe_mat_3x3 i = mat_3x3_mul_mat_3x3(&m, &m2);
    mat_3x3_print(&i, NULL);
    
    i.m[2][0] = 1.6f;
    
    lathe_vec3 tw = get_mat_3x3_row(&i, 1);
    
    vec3_print(&tw, NULL);
    float mag = vec3_magnitude(&n);
    printf("%f\n", mag);
    lathe_mat_3x3 ma = {
      .m = {{1.0f, 2.0f, 3.0f}, {4.0f, 5.0f, 6.0f}, {7.0f, 8.0f, 9.0f}}};
    
    lathe_vec3 ve = {{1.0f, 2.0f, 3.0f}};
    
    lathe_vec3 result = mat_3x3_mul_vec3(&ma, &ve);
    assert(result.x == 14.0f);
    assert(result.y == 32.0f);
    assert(result.z == 50.0f);
    vec3_print(&result, NULL);
    
    LATHE_INFO("Ending Mat3 Test",nullptr);
    
    LATHE_INFO("Starting Arena Test...",nullptr);
    #ifdef LATHE_HEAP_BACKING_MEMORY_TEST
        void *backing_buf = malloc(LATHE_1KB);//platform_alloc(LATHE_1KB,true)
    #else
        unsigned char backing_buf[LATHE_1KB];
    #endif
    lathe_arena arena = {};
    arena_init(&arena, backing_buf, LATHE_1KB);
    int *z = (int *)arena_allocate(&arena, sizeof(int));
    if (z == nullptr) {
        printf("Theres nothing we can do\n");
    } else {
        *z = 1;
        printf("%p %d\n", (void *)z, *z);
    }
    
    [[maybe_unused]] void *should_fail = arena_allocate(&arena, SIZE_MAX);
    
    arena_free_all(&arena);
    
    printf("%p %c\n", (void *)z, *z);
    
    
    lathe_vec3 v1 = {{2, 7, 1}};
    lathe_vec3 v2 = {{8, 2, 8}};/*
    -0.5f, -0.5f, 0.0f,
     0.5f, -0.5f, 0.0f,
     0.0f,  0.5f, 0.0f
    */
    assert(vec3_dot_product(&v1, &v2) == 38.0f);
    fprintf(stderr, "dot is: %f\n", vec3_dot_product(&v1, &v2));
    // free(backing_buf); //not really needed 
    fclose(fptr);
    LATHE_INFO("Ending Arena Test",nullptr);
    
    LATHE_INFO("Starting Lathe Logger Test...",nullptr);
    
    LATHE_FATAL("This is FATAL!", nullptr);
    LATHE_WARN("This is a warning!", nullptr);
    LATHE_ERROR("This is an ERROR", stdout);
    LATHE_INFO("This is just for information", NULL);
    LATHE_TRACE("This is a trace msg", NULL);
    LATHE_DEBUG("debugger msg", nullptr);
    
    LATHE_INFO("Ending Lathe Logger Test",nullptr);
    
    LATHE_INFO("Starting Lathe Simple Run Test...",nullptr);
    PlatformState platform = {};
    timer_init();
    
    if (!platform_start(&platform, nullptr,0,0,screen_width,screen_height)) {
        LATHE_FATAL("Platform was not able to start!", nullptr);
        return 1;
    }
    
    if(renderer_start(&platform)){
        LATHE_INFO("Renderer was started", nullptr);
    }else{
        LATHE_FATAL("RENDERER was not able to start!", nullptr);
        return 1;
    }
    
    bool running = true;
    
    //Pre loop setup
    double previous_frame = lathe_get_time();
    double fps_timer = previous_frame;
    uint64_t fps_frames = 0;
    const double target_fps = 75.0;
    const double target_frame_time = 1.0 / target_fps;
    [[maybe_unused]] float t = 0.0f;
    shader_init("./core/assets/shaders/quad.vert", "./core/assets/shaders/shader.frag");
    
    //Main Loop
    while (running) { 
        double frame_start = lathe_get_time();
        
        running = platform_handle_os_events(&platform);
        
        //update
        controller_print_buttons();
        
        //render a new frame
        float red   = (sinf(t)        + 1.0f) / 2.0f;
        float green = (sinf(t + 2.0f) + 1.0f) / 2.0f;
        float blue  = (sinf(t + 4.0f) + 1.0f) / 2.0f;   
        float alpha = 1.0f;
        
        renderer_begin_frame();
        
        lathe_vec3 a = {{-0.5f, -0.5f, 0.0f}};
        lathe_vec3 b = {{0.5f, -0.5, 0.0f}};
        lathe_vec3 c = {{0.0f, 0.5f, 0.0f}};
        
         renderer_clear(red,green,blue,alpha);
        renderer_render_triangle(&a,&b,&c);
      
        
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
