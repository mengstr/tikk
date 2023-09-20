#include <stdio.h>
#include <math.h>

int target[]={
	// 1,
	// 5,
	// 10,
	// 50,
	// 100,
	// 500,
	// 1000,
	// 5000,
	// 10000,
	// 50000,
	// 100000,
	// 500000,
	// 1000000,
	// 5000000,
	// 8000000
	2000000,
	3000000,
	4000000,
	5000000,
	6000000,
	7000000,
	8000000,
	9000000,
	10000000,
	11000000,
	12000000,
	13000000,
	14000000,
	15000000,
	16000000,
	17000000,
	18000000,
	19000000,
	20000000,
	21000000,
	22000000,
	23000000,
	24000000
};

// Function to calculare percentage difference between two numbers
float percentage(float a, float b) {
    return ((a-b)/a)*100.0;
}


int  main() {
	printf("Hello World! int %lu long %lu\n",sizeof(int),sizeof(long)) ;
	for (int i=0; i<sizeof(target)/sizeof(int); i++) {
        float best=100.0;
		for (int prescaler=0; prescaler<65536; prescaler++) {
			for (int divider=1; divider<65536; divider++) {
				float factor=(prescaler+1)* (divider+1);
				float freq = 48000000.0 / factor;
                float diff=percentage(target[i],freq);            
				if (fabs(diff)<fabs(best) && fabs(diff)<5) {
                    best=diff;
					printf("prescaler=%-6d divider=%-6d target=%-7d found %8f percentage=%06f\n", 
                    prescaler,divider,target[i],freq,diff);
				}
			}
		}
	}	
    return 0;
}


