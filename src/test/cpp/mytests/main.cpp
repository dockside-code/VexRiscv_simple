//#define REGINT(address) *(volatile int*)(address)
#define REGUINT(address) *(volatile unsigned int*)(address)
#define REGINTPOINT(address) (volatile int*)(address)
#define REGFLOAT(address) *(volatile float*)(address)
#define DMEM_BASE  (0xA0030000)

int main(){
	int a[201];
    for (int i = 0; i <= 200; i ++)
	{
       	a[i] = i;
	}
    int b = a[1];  	  //B = 1
	int c = a[200];	  //C = 200
	int d = a[50];	  //D = 50
	int e = a[100];	  //E = 100
	int f = a[150];   //F = 150
	int g = a[175];   //G = 175
	int h = a[25];	  //H = 25
	a[25] = 7;
	a[25] = 8;
	a[25] = 9;
	a[25] = 10;
	a[24] = 3;
	a[24] = 6;
	a[24] = 12;
	a[50] = 23;
	a[100] = 25;
	int res = b + c + d + e + f + g + h;//E = 701
	int res2 = a[25] + a[24] + a[50] + a[100];
   
    	//REGINT(0xA0030004) = res;
	int *rua = (int*) 0xA0030004;
	int *rua2 = (int*) 0xA0030008;
	*rua = res;
	*rua2 = res2;


    while(1){

    }
    return 1;

}
