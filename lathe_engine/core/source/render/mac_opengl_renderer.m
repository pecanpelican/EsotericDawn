#define GL_SILENCE_DEPRECATION //sigh 2026... RIP my sweet summer child

#import <Cocoa/Cocoa.h>
#import <OpenGL/gl3.h>

#include "render/renderer.h"

#include <mach/mach_time.h>
#include <stdlib.h>
#include "platform/platform.h"
#include "system/lathe_logger.h"

typedef struct MacPlatformState {
    NSApplication *application;
    NSWindow *window;
    mach_timebase_info_data_t timebase;
    uint64_t start_time;
} MacPlatformState;


//TODO: move into renderer internal state
static NSOpenGLView *gl_view = nil;
static NSOpenGLContext *gl_context = nil;
//gonna do gl stuff, maybe metal another time
static unsigned int opengl_program_id;

bool renderer_start(PlatformState *state) {
    if (state == nullptr || state->internal_state == nullptr) {
        return false;
    }

    MacPlatformState *mac = (MacPlatformState *)state->internal_state;

    if (mac->window == nil) {
        return false;
    }

    NSOpenGLPixelFormatAttribute attrs[] = {
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,

        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFAAccelerated,

        NSOpenGLPFAColorSize, 24,
        NSOpenGLPFAAlphaSize, 8,
        NSOpenGLPFADepthSize, 24,

        0
    };

    NSOpenGLPixelFormat *format = [[NSOpenGLPixelFormat alloc] initWithAttributes:attrs];

    if (format == nil) {
        LATHE_FATAL("Failed to create OpenGL pixel format!", nullptr);
        return false;
    }

 
    gl_view = [[NSOpenGLView alloc] initWithFrame:mac->window.contentView.bounds 
                                    pixelFormat:format];

    if (gl_view == nil) {
        LATHE_FATAL("Failed to create NSOpenGLView!", nullptr);
        return false;
    }


    gl_view.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;

    [mac->window setContentView:gl_view];

   
    gl_context = [gl_view openGLContext];

    if (gl_context == nil) {
        LATHE_FATAL("Failed to create OpenGL context!", nullptr);
        return false;
    }

    [gl_context makeCurrentContext];

    const GLubyte *version = glGetString(GL_VERSION);

    if (version != NULL) {
        LATHE_INFO_F(nullptr,"OpenGL Version: %s\n", (const char *)version);
    }
    [[maybe_unused]] GLuint vert_shader_id = glCreateShader(GL_VERTEX_SHADER);
    [[maybe_unused]] GLuint frag_shader_id = glCreateShader(GL_FRAGMENT_SHADER);
    return true;
}

void renderer_begin_frame() { 
    [gl_context makeCurrentContext];
}

void renderer_render_triangle(const lathe_vec3 *a,const lathe_vec3 *b,const lathe_vec3 *c){
    //Going to turn  the vec3 into a vertices list
    float vertices [] = {a->x,a->y,a->z,b->x,b->y,b->z,c->x,c->y,c->z};
    // for(int i = 0; i < 9;i++){
    //     printf("index %d is %f\n",i,vertices[i]);
    // } //Good
    unsigned int VBO, VAO;
    glGenVertexArrays(1,&VAO);
    glGenBuffers(1, &VBO);  
    
    glBindVertexArray(VAO);//first
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    //glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    //glEnableVertexAttribArray(1);
    
    // render the triangle
    shader_use();
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);
    
}

void shader_init(const char* vertex_path,const char* fragment_path)
{
    FILE* vertex_file = fopen(vertex_path,"r");
    FILE* fragment_file = fopen(fragment_path,"r");
    if(vertex_file == nullptr){
         LATHE_FATAL("Failed to open the vertex file", nullptr);
    }
    if(fragment_file == nullptr){
         LATHE_FATAL("Failed to open the fragment file", nullptr);
    }
    
    fseek(vertex_file,0,SEEK_END);
    fseek(fragment_file,0,SEEK_END);
    
    uint64_t vert_file_len = (uint64_t)ftell(vertex_file);
    uint64_t frag_file_len = (uint64_t)ftell(fragment_file);
    
    rewind(vertex_file);
    rewind(fragment_file);
    
    char* v_buf = (char*)platform_alloc(vert_file_len + 1,false);
    if(v_buf == nullptr){
        fclose(vertex_file);
        fclose(fragment_file);
        return;
    }
    
    char* f_buf = (char*)platform_alloc(frag_file_len + 1,false);
        if(f_buf == nullptr){
        platform_free(v_buf,false);
        fclose(vertex_file);
        fclose(fragment_file);
        
        return;
    }
    
    const GLchar *vert_sources[] = {v_buf};
    const GLchar *frag_sources[] = {f_buf};
    
    v_buf[vert_file_len] = '\0';
    f_buf[frag_file_len] = '\0';
    
     fread(v_buf, sizeof(char), vert_file_len, vertex_file);
     fread(f_buf,sizeof(char),frag_file_len,fragment_file);
    
    GLuint vert, frag;
    int success;
    char infoLog[512];
    
    vert = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vert,1,vert_sources,NULL);
    glCompileShader(vert);
    
    //error stuff
    
    frag = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(frag,1,frag_sources,NULL);
    glCompileShader(frag);
    
    //error stuff
    
    opengl_program_id = glCreateProgram();
    glAttachShader(opengl_program_id,vert);
    glAttachShader(opengl_program_id,frag);
    glLinkProgram(opengl_program_id);
    
    glGetProgramiv(opengl_program_id, GL_LINK_STATUS, &success);
if(!success)
{
    glGetProgramInfoLog(opengl_program_id, 512, NULL, infoLog);
    LATHE_FATAL("ERROR::SHADER::PROGRAM::LINKING_FAILED", nullptr);
}
    glDeleteShader(vert);
    glDeleteShader(frag);
    
    platform_free(v_buf,false);
    platform_free(f_buf,false);
    
}
void shader_use(){
    glUseProgram(opengl_program_id);
}
void shader_set_bool(const char* name, bool value){
    glUniform1i(glGetUniformLocation(opengl_program_id, name), (int)value); 
}
void shader_set_int(const char* name, int value){
    glUniform1i(glGetUniformLocation(opengl_program_id, name), value);
}
void shader_set_float(const char* name, float value){
    glUniform1f(glGetUniformLocation(opengl_program_id, name), value); 
}


void renderer_clear(float red, float green, float blue, float alpha) {
    glClearColor(red, green, blue, alpha);
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);    
}

void renderer_end_frame() {
    [gl_context flushBuffer];
}

void renderer_shutdown() {
    [NSOpenGLContext clearCurrentContext];
    gl_context = nil;
    gl_view = nil;
}