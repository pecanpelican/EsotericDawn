//
// platform_mac.m
//

#import <Cocoa/Cocoa.h>
#include "platform/platform.h"
#include <mach/mach_time.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static uint64_t start_time;
typedef struct MacPlatformState {
    NSApplication *application;
    NSWindow *window;
    mach_timebase_info_data_t timebase;
    uint64_t start_time;
} MacPlatformState;

//todo dont make this silly thing global...
const static char* key_code[0x7E] = {[0x00] = "A", [0x01] = "S", [0x0D] = "W", [0x02] = "D", [0x35] = "ESC",[0x31] = "Space"};

bool platform_start(PlatformState *state, const char *application_name, int32_t x, int32_t y, int32_t width, int32_t height) {
    //Do not belive ARC is needed here? But should verify this with debugger/ analyser later today 10/02
    @autoreleasepool{ // I mean it works....
    if (state == nullptr) {
        LATHE_FATAL("The platform state was not provided!", stderr);     
        return false;
    }
    
    application_name = (application_name == nullptr) ? default_application_name : application_name; 
    width = (width <= 0) ? default_win_width : width;
    height = (height <= 0) ? default_win_height : height;

    MacPlatformState *mac = platform_alloc(sizeof(MacPlatformState),false);

    if (mac == nullptr) {
        LATHE_FATAL("MacOSX failed to allocate memory for interal state!", stderr);
        return false;
    }

    mac->application = [NSApplication sharedApplication];
    [mac->application setActivationPolicy:NSApplicationActivationPolicyRegular];

    NSRect window_rect = NSMakeRect((CGFloat)x,(CGFloat)y,(CGFloat)width,(CGFloat)height);

    NSWindowStyleMask style =
        NSWindowStyleMaskTitled |
        NSWindowStyleMaskClosable |
        NSWindowStyleMaskMiniaturizable |
        NSWindowStyleMaskResizable;

    mac->window = [[NSWindow alloc] initWithContentRect:window_rect
                      styleMask:style
                        backing:NSBackingStoreBuffered
                          defer:NO];

    if (mac->window == nil) {
        LATHE_FATAL("MacOSX failed to allocate NSWindow !", stderr);
        platform_free(mac,false);
        return false;
    }

    NSString *title =
        [NSString stringWithUTF8String:application_name];

    if (title == nil) {
         LATHE_FATAL("MacOSX failed to convert Title ...huh..", stderr);
        [mac->window close];//Now is this really needed? Cant see this releastically failing...
        platform_free(mac,false);
        return false;
    }

    [mac->window setTitle:title];
    [mac->window makeKeyAndOrderFront:nil];
    [mac->application activateIgnoringOtherApps:YES];


    if (mach_timebase_info(&mac->timebase) != KERN_SUCCESS) {
         LATHE_FATAL("Macos did something odd with timebase !", stderr);
        [mac->window close];
        platform_free(mac,false);
        return false;
    }

    mac->start_time = mach_absolute_time(); // Esentially the engines start time
    start_time = mac->start_time;

    state->internal_state = mac;
    
    return true;        
    }
}

void platform_shutdown(PlatformState *state)
{
    if (state != NULL && state->internal_state != NULL) {
        MacPlatformState *mac = (MacPlatformState *)state->internal_state;
        if (mac->window != nil) {
            [mac->window close];
             mac->window = nil;
        }
    
        platform_free(mac,false);
    
        state->internal_state = NULL;
    }
}

bool platform_handle_os_events(PlatformState *state)
{
    if (state == NULL || state->internal_state == NULL) {
        return false;
    }

    MacPlatformState *mac =
        (MacPlatformState *)state->internal_state;
    NSEvent * event;
    do{
        event = [mac->application
                    nextEventMatchingMask:NSEventMaskAny
                        untilDate:nil
                            inMode:NSDefaultRunLoopMode
                                dequeue:YES];
                                
        switch(event.type){
            case NSEventTypeLeftMouseDown:
                LATHE_DEBUG("left mouse down",nullptr);
                break;
            case NSEventTypeRightMouseDown:
                LATHE_DEBUG("right mouse down",nullptr);
                break;
            case NSEventTypeKeyDown:
                unsigned short keyCode = event.keyCode;
                //Todo: Translate virtual keycode to key!
                //rn wasd space and esc are all I care about
                const char* key = key_code[keyCode];
                LATHE_DEBUG_F(nullptr, "%s key was pressed down\n",key);
                if(key != nullptr && strncmp(key,"ESC",3) == 0){
                    LATHE_DEBUG_F(nullptr,"ESC comparision passed, closing window\n");
                    mac->window.isVisible = false;
                    //could just return false.... but may need some cleanup later...
                }
                break;
            //Should handle default window close event, probably best to handle here instead...
            default:
            
        }
    
    [mac->application sendEvent:event]; 
    [mac->application updateWindows];
    
    }while(event != nil);


    // Window was closed.
    if (![mac->window isVisible]) {
        LATHE_DEBUG_F(nullptr,"The window was closed!\n");
        return false;
    }

    return true;
}

void *platform_alloc(uint64_t size, bool aligned)
{
    if (size <= 0) {
        LATHE_ERROR("Size asked for is invalid",stderr);
        return nullptr;
    }

    if (!aligned) {
        return malloc((size_t)size);
    }

    constexpr size_t alignment = 16; // NOTE: sizeof(void *)  MIN and power of 2

    void *block = NULL;

    if (posix_memalign(&block, alignment, (size_t)size) != 0) {
        return nullptr;
    }

    return block;
}

void *platform_zero_alloc(uint64_t size, bool aligned)
{
    void *block = platform_alloc(size, aligned);

    if (block == NULL) {
        return NULL;
    }

    memset(block, 0, (size_t)size);

    return block;
}

void platform_free(void *block, [[maybe_unused]] bool aligned)
{
    //posix_memalign lets me not worry about details of alignment on free
    free(block);
}

void *platform_zero_mem(void *block, uint64_t size)
{
    if (block == nullptr) {
        return nullptr;
    }

    return memset(block, 0, (size_t)size);
}

void *platform_copy_mem(void *dest, const void *src, uint64_t size)
{
    if (dest == nullptr || src == nullptr) {
        return nullptr;
    }

    return memcpy(dest, src, (size_t)size);
}

void *platform_set_mem(void *dest,
                       uint64_t val,
                       uint64_t size)
{
    if (dest == NULL) {
        return NULL;
    }

    return memset(dest, (int)(unsigned char)val, (size_t)size);
}

void platform_console_write(const char *msg,
                            lathe_log_level lvl)
{
    if (msg == NULL) {
        return;
    }

    FILE *out = stdout;

    switch (lvl) {

        case LOG_LEVEL_FATAL:
        case LOG_LEVEL_ERROR:
            out = stderr;
            break;

        default:
            out = stdout;
            break;
    }

    fputs(msg, out);
    fflush(out);
}

double platform_get_time(void)
{
    static mach_timebase_info_data_t timebase = {0};

    if (timebase.denom == 0) {
        if (mach_timebase_info(&timebase) != KERN_SUCCESS) {
            return 0.0;
        }
    }

    uint64_t ticks = mach_absolute_time();

    double nanoseconds =
        (double)ticks *
        (double)timebase.numer /
        (double)timebase.denom;

    return nanoseconds / 1000000000.0;
}

void platform_sleep(uint64_t ms)
{
    while (ms > 0) {

        uint64_t chunk_ms = ms;

        if (chunk_ms > 1000) {
            chunk_ms = 1000;
        }

        usleep((useconds_t)(chunk_ms * 1000));

        ms -= chunk_ms;
    }
}



void timer_init(void) {
   // start_time = platform_get_timer_value();
}

uint64_t platform_get_timer_value(void) {
    return mach_absolute_time();
}

uint64_t platform_get_timer_freq(void) {
    mach_timebase_info_data_t info;
    mach_timebase_info(&info);

    return (uint64_t)(1e9 * (double)info.denom /
                     (double)info.numer);
}

double lathe_get_time(void) {
    return (double)(platform_get_timer_value() - start_time) /
           (double)platform_get_timer_freq();
}