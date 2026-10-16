// lathe_renderer.h

#pragma once 

#include <stdint.h>
#include "platform/platform.h"
#include "math/lathe_vec3.h"

//temp

    
typedef enum {
    RENDERER_OPENGL,
} renderer_api;

typedef struct {
    void *internal_state;
} renderer;



bool renderer_start(PlatformState *state);

void renderer_begin_frame();

void renderer_clear(
    float red,
    float green,
    float blue,
    float alpha
);

void renderer_render_triangle(const lathe_vec3 *a,const lathe_vec3 *b,const lathe_vec3 *c,[[maybe_unused]]const lathe_vec3 *color);

/*
class Shader
{
public:
    // the program ID
    unsigned int ID;
  
    // constructor reads and builds the shader
    Shader(const char* vertexPath, const char* fragmentPath);
    // use/activate the shader
    void use();
    // utility uniform functions
    void setBool(const std::string &name, bool value) const;  
    void setInt(const std::string &name, int value) const;   
    void setFloat(const std::string &name, float value) const;
};
  */
//
//So just gonna make it opengl may need to mess around to get it working with Metal and Vulkan, but one day at a time!
//

bool shader_init(const char* vertex_path,const char* fragment_path);
void shader_use();
void shader_set_bool(const char* name, bool value);
void shader_set_int(const char* name, int value);
void shader_set_float(const char* name, float value);


void renderer_end_frame();

void renderer_shutdown();
