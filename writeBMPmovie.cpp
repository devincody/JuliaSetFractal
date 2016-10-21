#include <iostream>
#include <string>
#include <math.h>
#include <unistd.h>
#include <string.h>

using namespace std;

struct imaginary {
	float real;
	float im;
};

typedef struct imaginary imaginary;

int Zmult(imaginary *A, imaginary *B,imaginary *C){
	float temp = A->real*B->real - A->im*B->im;
	C->im = A->real*B->im + A->im * B->real;
	C->real = temp;
	return 0;
}

int Zadd(imaginary *A, imaginary *B,imaginary *C){
	float temp = A->real+B->real;
	C->im = A->im+B->im;
	C->real = temp;
	return 0;
}
int Zmag2(imaginary *A){
	int ans = A->real*A->real + A->im*A->im;
	return ans;
}

float map2axis(int numb, float min, float max, int tot){
	 return ( ((float) numb) / ((float) tot) ) *(max - min) + (float) min;
}

int main(){
	imaginary *c;
	c = (imaginary *)malloc(sizeof(imaginary));
	for(int w = 0; w < 1000; w++){
		c->real = ((float) w)/500. - 1.;
		c->im = .6;
		FILE *f;
		unsigned char *img = NULL;
		int width;
		int height;
		if (img){
			free(img);
		}

		width = 1440;
		height = 900; //
		int filesize = 54 + 3*width*height;
		int imgsize = 3*width*height;
		img = (unsigned char *)malloc(imgsize);
		memset(img,0,imgsize);


		float ans;
		float I;
		float R;
		float max = 255;
		float value;
		int iterations = 255;

		for(int i= 0 ;i <width; i++){
			for (int j = 0; j < height; j++){
				//Map i onto real part, j onto imaginary part
				imaginary *N;
				N = (imaginary *)malloc(sizeof(imaginary));
				N->real = map2axis(i, -1.6, 1.6, width);
				N->im = map2axis(j, -1.0, 1.0, height);
				ans = 0;

				for (int z = 0; z < iterations; z++){
					Zmult(N,N,N);
					Zadd(N,c,N);
					ans = pow(Zmag2(N),.5);
					
					if (ans > max){
						ans = z;
						break;
					} 
				}

				int x = i;
				int y = height - 1 - j;

				I = N -> im;
				R = N->real;
				
				if (I > 255){
					I = 255;
				}
				if (R > 255){
					R = 255;
				}
				//ans = ans*4;
				if (ans > 255){
					ans = 255;
				}


				img[(x + y*width)*3+2] = (unsigned char)(ans); //red
				img[(x + y*width)*3+1] = (unsigned char)(ans); //green
				img[(x + y*width)*3+0] = (unsigned char)(ans); //blue
			}
		}


		unsigned char bmpfileheader[14] = {'B','M', 0,0,0,0, 0,0, 0,0, 54,0,0,0};
		unsigned char bmpinfoheader[40] = {40,0,0,0, 0,0,0,0, 0,0,0,0, 1,0, 24,0};
		unsigned char bmppad[3] = {0,0,0};

		bmpfileheader[ 2] = (unsigned char)(filesize    );
		bmpfileheader[ 3] = (unsigned char)(filesize>> 8);
		bmpfileheader[ 4] = (unsigned char)(filesize>>16);
		bmpfileheader[ 5] = (unsigned char)(filesize>>24);

		bmpinfoheader[ 4] = (unsigned char)(       width    );
		bmpinfoheader[ 5] = (unsigned char)(       width>> 8);
		bmpinfoheader[ 6] = (unsigned char)(       width>>16);
		bmpinfoheader[ 7] = (unsigned char)(       width>>24);
		bmpinfoheader[ 8] = (unsigned char)(       height    );
		bmpinfoheader[ 9] = (unsigned char)(       height>> 8);
		bmpinfoheader[10] = (unsigned char)(       height>>16);
		bmpinfoheader[11] = (unsigned char)(       height>>24);

		char filename[] = "/data2/frame";
		char fileend[] = ".bmp";
		char cwda[1024];
		getcwd(cwda,1024);
		strcat(cwda, filename);
		char numberstring[4];
		//printf("about to write\n");
		sprintf(numberstring, "%04d", w);
		
		strcat(cwda,numberstring);
		strcat(cwda, ".bmp");
		f = fopen(cwda, "w+");
		printf("%s\n",cwda );
		if (f == NULL) {
	    	printf ("Error opening input file %s\n", cwda);
	    	exit (1);
		}

		//f = fopen("fractal.bmp","wb");
		fwrite(bmpfileheader,1,14,f);
		fwrite(bmpinfoheader,1,40,f);
		for(int i=0; i<height; i++){
		    fwrite(img+(width*(height-i-1)*3),3,width,f);
		    fwrite(bmppad,1,(4-(width*3)%4)%4,f);
		}
		fclose(f);

		
	}
	return 0;
}


//  ffmpeg -i frames/%4d.bmp -c:v libx264 -r 15 -pix_fmt yuv420p Julia.mp4



