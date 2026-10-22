#include <assert.h>
#include "system/lathe_logger.h"

int main(void){
    
	LATHE_INFO("Starting Lathe Logger Test...",nullptr);
    
    LATHE_FATAL("This is FATAL!", nullptr);
    LATHE_WARN("This is a warning!", nullptr);
    LATHE_ERROR("This is an ERROR", stdout);
    LATHE_INFO("This is just for information", NULL);
    LATHE_TRACE("This is a trace msg", NULL);
    LATHE_DEBUG("debugger msg", nullptr);
    
    LATHE_INFO("Ending Lathe Logger Test",nullptr);

	//return -1;
}