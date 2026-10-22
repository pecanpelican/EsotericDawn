#include "math/lathe_vec3.h"
#include "system/lathe_logger.h"
#include <assert.h>
#include <stdio.h>

int main(void){
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
}