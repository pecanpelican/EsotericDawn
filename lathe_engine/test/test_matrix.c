#include "math/lathe_vec3.h"
#include "system/lathe_logger.h"
#include <assert.h>
#include <stdio.h>
#include "math/lathe_mat3.h"

int main(void){
	LATHE_INFO("Starting Mat3 Test...",nullptr);
    FILE *fptr;
    fptr = fopen("LogFile", "a");
           lathe_vec3 v = {{5, 25, 5}};
    //lathe_vec3 v_negated = {{-5,-25,-5}};
    v = vec3_negate(&v);
        lathe_vec3 d = vec3_add(&v, &v);
   // vec3_print(&d, nullptr);
    
        lathe_vec3 n = vec3_normalize(&v);
   // vec3_print(&n, nullptr);

    
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

}