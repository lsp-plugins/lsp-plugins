#include <stdio.h>
#include <math.h>

#ifndef LSP_IDE_DEBUG
int main()
{
    // Cosines
    printf("A_RE:\n");
    for (int i=0; i<16; ++i)
    {
        int n = 4 << i;
        for (size_t k=0; (k<8); ++k)
            printf("%.16f, ", cos(M_PI * (k%n) / n));
        printf("\n");
    }
    
    // Sines
    printf("\nA_IM:\n");
    for (int i=0; i<16; ++i)
    {
        int n = 4 << i;
        for (size_t k=0; (k<8); ++k)
            printf("%.16f, ", sin(M_PI * (k%n) / n));
        printf("\n");
    }
    
    // Both
    printf("\nA:\n");
    for (int i=0; i<16; ++i)
    {
        int n = 4 << i;
        for (size_t k=0; (k<8); ++k)
            printf("%.16f, ", cos(M_PI * (k%n) / n));
        printf("\n");
        for (size_t k=0; (k<8); ++k)
            printf("%.16f, ", sin(M_PI * (k%n) / n));
        printf("\n");
    }
    
    printf("\nDW_RE:\n");
    for (int i=0; i<16; ++i)
        printf("X8VEC(%.16f),\n", cos(M_PI / (4 << i)));
        
    printf("\nDW_IM:\n");
    for (int i=0; i<16; ++i)
        printf("X8VEC(%.16f),\n", sin(M_PI / (4 << i)));

    printf("\nDW:\n");
    for (int i=0; i<16; ++i)
        printf("X8VEC(%.16f), X8VEC(%.16f),\n", cos(M_PI / (1 << i)), sin(M_PI / (1 << i)));


    return 0;
}
#endif
