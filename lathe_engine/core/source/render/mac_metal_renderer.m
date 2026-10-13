// #include <Metal/Metal.h>
// #include <MetalKit/MetalKit.h>
// #include <QuartzCore/CAMetalLayer.h>
// #include <QuartzCore/QuartzCore.h>
// #include <mach/mach_time.h>
// #include "platform/platform.h"
// #include "render/renderer.h"
// 
// typedef struct MetalRenderer {
//     id<MTLDevice> device; //GPU of mac
//     //= MTLCreateSystemDefaultDevice();
//     NSWindow* app_window; //window handle
//     CAMetalLayer* metal_layer;
// }MetalRenderer;
// 
// typedef struct MacPlatformState {
//     NSApplication *application;
//     NSWindow *window;
//     mach_timebase_info_data_t timebase;
//     uint64_t start_time;
// } MacPlatformState;
// 
// bool renderer_start(PlatformState *state, [[maybe_unused]] renderer *r, [[maybe_unused]] renderer_api api){
//     if (state == nullptr || state->internal_state == nullptr) {
//         return false;//SESE maybe instead?
//     }
// 
//     MacPlatformState *mac = (MacPlatformState *)state->internal_state;
//     NSView *view = mac->window.contentView;
//     [[maybe_unused]] CAMetalLayer *metalLayer = (CAMetalLayer *)view.layer;
//     return false;
// }
// 
// void renderer_begin_frame([[maybe_unused]] renderer *r){
//     
// }
// 
// void renderer_clear(
//     [[maybe_unused]] renderer *r,
//     [[maybe_unused]] float red,
//     [[maybe_unused]] float green,
//     [[maybe_unused]] float blue,
//     [[maybe_unused]] float alpha
// ){
//     
// }
// 
// void renderer_end_frame([[maybe_unused]] renderer *r){
//     
// }
// 
// void renderer_shutdown([[maybe_unused]] renderer *r){
//     
// }
