#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp_header.h"
#pragma pack(1)

typedef struct {
	unsigned char blue, green, red;
}pixel;

void write_image(pixel **bitMap, bmp_fileheader *fileheader, 
	bmp_infoheader *infoheader, char *image_name){
	unsigned char padding = 0;
	//octetii de padding sunt egali cu restul impartirii lungimii la 4
	int padding_no = infoheader->width % 4;
	FILE *f = fopen(image_name, "wb");
	fwrite(fileheader, sizeof(bmp_fileheader), 1, f);
	fwrite(infoheader, sizeof(bmp_infoheader), 1, f);
	fseek(f, fileheader->imageDataOffset, SEEK_SET);
	for(int i = infoheader->height - 1; i >= 0; i--){
		for(int j = 0; j < infoheader->width; j++)
			fwrite(&bitMap[i][j], sizeof(pixel), 1, f);
		
		for(int k = 0; k < padding_no; k++)
			fwrite(&padding, sizeof(unsigned char), 1, f);
	}
	fclose(f);
}

void black_white(pixel **bitMap, bmp_fileheader *fileheader, 
	bmp_infoheader *infoheader, char *image_name){

	int avg;
	//creez o noua matrice de bitMap pentru a nu modifica poza initiala
	pixel **bitMap_black_white = malloc(infoheader->height * sizeof(pixel*));
	for(int i = 0; i < infoheader->height; i++)
		bitMap_black_white[i] = malloc(infoheader->width * sizeof(pixel));

	for(int i = 0; i < infoheader->height; i++)
		for(int j = 0; j < infoheader->width; j++){
			avg = (bitMap[i][j].blue + bitMap[i][j].green + bitMap[i][j].red) / 3;
			bitMap_black_white[i][j].blue = avg;
			bitMap_black_white[i][j].green = avg;
			bitMap_black_white[i][j].red = avg;
		}
	strcpy(image_name + 5, "_black_white.bmp");
	write_image(bitMap_black_white, fileheader, infoheader, image_name);
	
	for (int i = 0; i < infoheader->height; i++)
		free(bitMap_black_white[i]);
	free(bitMap_black_white);
}

void no_crop(pixel **bitMap, bmp_fileheader fileheader,
	bmp_infoheader infoheader, char *image_name){

	/*verific cine e mai mare dintre inaltime si latime si creez o noua matrice
	care va avea forma patratica*/
	if(infoheader.height > infoheader.width){
		pixel **bitMap_no_crop = malloc(infoheader.height * sizeof(pixel*));
		for(int i = 0; i < infoheader.height; i++)
			bitMap_no_crop[i] = malloc(infoheader.height * sizeof(pixel));

		/*in noua matrice de pixeli, pun valorile 255 la fiecare pixel de pe
		coloanele care depasesc dimensiunea matricei initiale*/
		for(int i = 0; i < infoheader.height - infoheader.width; i++)
			for(int j = 0; j < infoheader.height; j++)
				if(i%2 == 0){
					bitMap_no_crop[j][infoheader.height - i/2 - 1].blue = 255;
					bitMap_no_crop[j][infoheader.height - i/2 - 1].green = 255;
					bitMap_no_crop[j][infoheader.height - i/2 - 1].red = 255;
				}
				else{
					bitMap_no_crop[j][i/2].blue = 255;
					bitMap_no_crop[j][i/2].green = 255;
					bitMap_no_crop[j][i/2].red = 255;
				}

		//pun coloanele din matricea initiala pe coloanele ramase din noua matrice
		int crop = (infoheader.height - infoheader.width)/2;
		for(int i = crop; i < crop + infoheader.width; i++)
			for(int j = 0; j < infoheader.height; j++)
				bitMap_no_crop[j][i] = bitMap[j][i-crop];

		infoheader.width = infoheader.height;
		strcpy(image_name + 5, "_nocrop.bmp");  //modifica maaaa
		write_image(bitMap_no_crop, &fileheader, &infoheader, image_name);
		for(int i = 0; i < infoheader.height; i++)
			free(bitMap_no_crop[i]);
		free(bitMap_no_crop);
	}
	
	else{
		pixel **bitMap_no_crop = malloc(infoheader.width * sizeof(pixel*));
		for(int i = 0; i < infoheader.width; i++)
			bitMap_no_crop[i] = malloc(infoheader.width * sizeof(pixel));

		/*in noua matrice de pixeli, pun valorile 255 la fiecare pixel de pe
		liniile care depasesc dimensiunea matricei initiale*/
		for(int i = 0; i < infoheader.width - infoheader.height; i++)
			for(int j = 0; j < infoheader.width; j++)
				if(i%2 == 0){
					bitMap_no_crop[infoheader.width - i/2 - 1][j].blue = 255;
					bitMap_no_crop[infoheader.width - i/2 - 1][j].green = 255;
					bitMap_no_crop[infoheader.width - i/2 - 1][j].red = 255;
				}
				else{
					bitMap_no_crop[i/2][j].blue = 255;
					bitMap_no_crop[i/2][j].green = 255;
					bitMap_no_crop[i/2][j].red = 255;
				}

		//pun coloanele din matricea initiala pe liniile ramase din noua matrice
		int crop = (infoheader.width - infoheader.height)/2;
		for(int i = crop; i < crop + infoheader.height; i++)
			for(int j = 0; j < infoheader.width; j++)
				bitMap_no_crop[i][j] = bitMap[i-crop][j];

		infoheader.height = infoheader.width;
		strcpy(image_name + 5, "_nocrop.bmp");  //modifica maaaa
		write_image(bitMap_no_crop, &fileheader, &infoheader, image_name);
		for(int i = 0; i < infoheader.height; i++)
			free(bitMap_no_crop[i]);
		free(bitMap_no_crop);
	}

}

int sum_filter_blue(pixel **bitMap, int lines, int columns, int **filter,
	int n, int k, int l){  //k si l sunt valorile pixelului pe care il modific
	/*adun valoarea pixelilor inmultiti cu filtrul care se afla la o distanta
	mai mica decat n/2 de pixelul pe care il mmodific*/
	int sum = 0;
	for(int i = k - n/2; i <= k + n/2; i++)
		for(int j = l - n/2; j <= l + n/2; j++)
			if(i >= 0 && j >= 0 && i < lines && j < columns)
				sum = sum + bitMap[i][j].blue * filter[i - k + n/2][j - l + n/2];
	if(sum > 255)
		return 255;
	else if(sum < 0)
		return 0;
	
	return sum;
}

int sum_filter_green(pixel **bitMap, int lines, int columns, int **filter,
	int n, int k, int l){
	
	int sum = 0;
	for(int i = k - n/2; i <= k + n/2; i++)
		for(int j = l - n/2; j <= l + n/2; j++)
			if(i >= 0 && j >= 0 && i < lines && j < columns)
				sum = sum + bitMap[i][j].green * filter[i - k + n/2][j - l + n/2];
	if(sum > 255)
		return 255;
	if(sum < 0)
		return 0;

	return sum;
}

int sum_filter_red(pixel **bitMap, int lines, int columns, int **filter,
	int n, int k, int l){
	
	int sum = 0;
	for(int i = k - n/2; i <= k + n/2; i++)
		for(int j = l - n/2; j <= l + n/2; j++)
			if(i >= 0 && j >= 0 && i < lines && j < columns)
				sum = sum + bitMap[i ][j].red * filter[i - k + n/2][j - l + n/2];
	if(sum > 255)
		return 255;
	else if(sum < 0)
		return 0;
	
	return sum;
}

void filter(pixel **bitMap, bmp_fileheader *fileheader,
	bmp_infoheader *infoheader, char *image_name, char *filter_name){

	FILE *f = fopen(filter_name, "rt");
	int n;
	//citesc matricea de filtru din fisier
	fscanf(f, "%d\n", &n);
	int **filter = malloc(n * sizeof(int *));
	for(int i = 0; i < n; i++)
		filter[i] = malloc(n * sizeof(int));

	for(int i = 0; i < n; i++)
		for(int j = 0; j < n; j++)
			fscanf(f, "%d\n", &filter[i][j]);

	fclose(f);


	pixel **bitMap_filter = malloc(infoheader->height * sizeof(pixel*));
	for(int i = 0; i < infoheader->height; i++)
		bitMap_filter[i] = malloc(infoheader->width * sizeof(pixel));

	for(int i = 0; i < infoheader->height; i++)
		for(int j = 0; j < infoheader->width; j++){
			bitMap_filter[i][j].blue = sum_filter_blue(bitMap, infoheader->height,
			infoheader->width, filter, n, i, j);
			bitMap_filter[i][j].green = sum_filter_green(bitMap, infoheader->height,
			infoheader->width, filter, n, i, j);
			bitMap_filter[i][j].red = sum_filter_red(bitMap, infoheader->height,
			infoheader->width, filter, n, i, j);
		}

	strcpy(image_name + 5, "_filter.bmp");
	write_image(bitMap_filter, fileheader, infoheader, image_name);
	
	for(int i = 0; i < infoheader->height; i++)
		free(bitMap_filter[i]);
	free(bitMap_filter);

	for (int i = 0; i < n; i++)
		free(filter[i]);
	free(filter);
}

void pooling(pixel **bitMap, bmp_fileheader *fileheader,
	bmp_infoheader *infoheader, char *image_name, char pool_type, int pool_dim){

	pixel **bitMap_pool = malloc(infoheader->height * sizeof(pixel*));
	for(int i = 0; i < infoheader->height; i++)
		bitMap_pool[i] = malloc(infoheader->width * sizeof(pixel));

	if(pool_type == 'm'){
		for(int i = 0; i < infoheader->height; i++)
			for(int j = 0; j < infoheader->width; j++){
				bitMap_pool[i][j].blue = bitMap[i][j].blue;
				bitMap_pool[i][j].green = bitMap[i][j].green;
				bitMap_pool[i][j].red = bitMap[i][j].red;
				/*compar fiecare culoare din pixel cu pixelii care se afla la o
				distanta mai mica de (dimensiunea pooling-ului)/2 de el si pastrez
				cea mai mica valoare*/
				for(int k = i - pool_dim/2; k <= i + pool_dim/2; k++)
					for(int l = j - pool_dim/2; l <= j + pool_dim/2; l++){
						if(k >= 0 && l >=0 &&
						   k < infoheader->height && l < infoheader->width){
							if(bitMap_pool[i][j].blue > bitMap[k][l].blue)
								bitMap_pool[i][j].blue = bitMap[k][l].blue;
							if(bitMap_pool[i][j].green > bitMap[k][l].green)
								bitMap_pool[i][j].green = bitMap[k][l].green;
							if(bitMap_pool[i][j].red > bitMap[k][l].red)
								bitMap_pool[i][j].red = bitMap[k][l].red;}
						/*pixelii de pe margine au vecini (0,0,0), deci aceea
						va fi valoarea lor*/
						else{
							bitMap_pool[i][j].blue = 0;
							bitMap_pool[i][j].green = 0;
							bitMap_pool[i][j].red = 0;
					}
				}
		}
	}
	else{
		for(int i = 0; i < infoheader->height; i++)
			for(int j = 0; j < infoheader->width; j++){
				bitMap_pool[i][j].blue = bitMap[i][j].blue;
				bitMap_pool[i][j].green = bitMap[i][j].green;
				bitMap_pool[i][j].red = bitMap[i][j].red;
				/*compar fiecare culoare din pixel cu pixelii care se afla la o
				distanta mai mica de (dimensiunea pooling-ului)/2 de el si pastrez
				cea mai mare valoare*/
				for(int k = i - pool_dim/2; k <= i + pool_dim/2; k++)
					for(int l = j - pool_dim/2; l <= j + pool_dim/2; l++){
						if(k >= 0 && l >= 0 && 
						   k < infoheader->height && l < infoheader->width){
							if(bitMap_pool[i][j].blue < bitMap[k][l].blue)
								bitMap_pool[i][j].blue = bitMap[k][l].blue;
							if(bitMap_pool[i][j].green < bitMap[k][l].green)
								bitMap_pool[i][j].green = bitMap[k][l].green;
							if(bitMap_pool[i][j].red < bitMap[k][l].red)
								bitMap_pool[i][j].red = bitMap[k][l].red;
					}
				}
		}
	}


	strcpy(image_name + 5, "_pooling.bmp");
	write_image(bitMap_pool, fileheader, infoheader, image_name);

	for (int i = 0; i < infoheader->height; i++)
		free(bitMap_pool[i]);
	free(bitMap_pool);
}
/*verifica daca suma modulelor diferentelor dintre cele doua numere este
mai mica decat threshold*/
int ok(pixel **bitMap, int i, int j , int k, int l, int threshold){
	int val = abs(bitMap[i][j].blue - bitMap[k][l].blue) +
			  abs(bitMap[i][j].green - bitMap[k][l].green) +
			  abs(bitMap[i][j].red - bitMap[k][l].red);
	
	return threshold - val;
}
/*Verifica fiecare vecin al unui pixel care corespunde cerintei, iar in cazul
in care gaseste un vecin, vecinii acestuia vor fi verificati. Pixelii care nu
corespund cerintei vor avea valoarea 2 in matricea de zone pentru a fi 
verificat o singura data.*/
void ngh(pixel ** bitMap, int start_i, int start_j, int i, int j,
		 int threshold, int **mat_zone, int height, int width){

	if(j - 1 >= 0 && mat_zone[i][j-1] == 0 && ok(bitMap, start_i, start_j,
	   i, j - 1, threshold) >= 0){
		mat_zone[i][j-1] = 1;
		ngh(bitMap, start_i, start_j, i, j-1, threshold, mat_zone, height, width);
	}
	else if (j - 1 >= 0 && mat_zone[i][j-1] == 0)
		mat_zone[i][j-1] = 2;

	if(j + 1 < width && mat_zone[i][j+1] == 0 && ok(bitMap, start_i, start_j,
	   i, j + 1, threshold) >= 0 ){
		mat_zone[i][j+1] = 1;
		ngh(bitMap, start_i, start_j, i, j+1, threshold, mat_zone, height, width);
	}
	else if(j + 1 < width && mat_zone[i][j+1] == 0)
		mat_zone[i][j+1] = 2;

	if(i - 1 >= 0 && mat_zone[i-1][j] == 0 && ok(bitMap, start_i, start_j,
	   i - 1, j, threshold) >= 0){
		mat_zone[i-1][j] = 1;
		ngh(bitMap, start_i, start_j, i-1, j, threshold, mat_zone, height, width);
	}
	else if(i-1 >= 0 && mat_zone[i-1][j] == 0)
		mat_zone[i-1][j] = 2;

	if(i + 1 < height && mat_zone[i+1][j] == 0 && ok(bitMap, start_i, start_j,
	   i + 1, j, threshold) >= 0){
		mat_zone[i+1][j] = 1;
		ngh(bitMap, start_i, start_j, i+1, j, threshold, mat_zone, height, width);
	}
	else if(i+1 < height && mat_zone[i+1][j] == 0)
		mat_zone[i+1][j] = 2;
}	

void zone(pixel **bitMap, int **mat_zone, int start_i, int start_j, int height,
		  int width, int threshold){

	int no_in_zone = 0, sum_blue = 0, sum_green = 0, sum_red = 0;

	mat_zone[start_i][start_j] = 1;
	/*porneste de la primul pixel ii verifica pe toti daca fac parte din zona,
	pixelii care apartin zonei primesc valoarea 1 in matricea zonei, iar dupa
	verificare se calculeaza suma pixelilor din zona si numarul lor. In final 
	pixelii sunt inlocuiti, cei care fac parte din zona primesc valoarea -1,
	iar cei care nu sunt setati pe 0*/
	
	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(mat_zone[i][j] == 1)
				ngh(bitMap, start_i, start_j, i, j,
					threshold, mat_zone, height, width);

	for(int i = 0; i < height; i++)
		for(int j = 0; j < width; j++)
			if(mat_zone[i][j] == 1){
				no_in_zone++;
				sum_blue = sum_blue + bitMap[i][j].blue;
				sum_green = sum_green + bitMap[i][j].green;
				sum_red = sum_red + bitMap[i][j].red;
			}

	for(int i = 0; i < height; i++)
		for (int j = 0; j < width; j++){
			if(mat_zone[i][j] == 1){
				bitMap[i][j].blue = sum_blue / no_in_zone;
				bitMap[i][j].green = sum_green / no_in_zone;
				bitMap[i][j].red = sum_red / no_in_zone;
				mat_zone[i][j] = -1;
			}
			if(mat_zone[i][j] == 2)
				mat_zone[i][j] = 0;
		}
}

void clustering(pixel **bitMap, bmp_fileheader *fileheader,
	bmp_infoheader *infoheader, char *image_name, int threshold){

	int **mat_zone = calloc(infoheader->height, sizeof(int*));
	for(int i = 0; i < infoheader->height; i++)
		mat_zone[i] = calloc(infoheader->width, sizeof(int));
	
	/*daca matricea de zona mai are valori de 0, inseamna ca mai exista zone
	nemodificate*/
	if(threshold > 0)
		for(int i = 0; i < infoheader->height; i++)
			for(int j = 0; j < infoheader->width; j++)
				if(mat_zone[i][j] == 0)
					zone(bitMap, mat_zone, i, j, infoheader->height,
						 infoheader->width, threshold);

	strcpy(image_name + 5, "_clustered.bmp");
	write_image(bitMap, fileheader, infoheader, image_name);

	for (int i = 0; i < infoheader->height; i++){
		free(bitMap[i]);
		free(mat_zone[i]);
	}
	free(bitMap);
	free(mat_zone);
}

int main(){
	
	FILE *g = fopen("input.txt", "rt");
	char *image = malloc(24 * sizeof(char));
	fscanf(g, "%s\n", image);  //citesc numele imaginii

	FILE *f = fopen(image, "rb");  //inceput citire
	bmp_fileheader *fileheader = malloc(sizeof(bmp_fileheader));
	bmp_infoheader *infoheader = malloc(sizeof(bmp_infoheader));
	fread(fileheader, sizeof(bmp_fileheader), 1, f);
	fread(infoheader, sizeof(bmp_infoheader), 1, f);
	fseek(f, fileheader->imageDataOffset, SEEK_SET);

	pixel **bitMap = malloc(infoheader->height * sizeof(pixel*));
	//aloc fiecare linie din matricea de pixeli
	for(int i = 0; i < infoheader->height; i++)
		bitMap[i] = malloc(infoheader->width * sizeof(pixel));

	int padding_no = infoheader->width % 4;
	unsigned char padding;
	for(int i = infoheader->height - 1; i >= 0; i--){
		for(int j = 0; j < infoheader->width; j++)
			fread(&bitMap[i][j], sizeof(pixel), 1, f);

		for(int k = 0; k < padding_no; k++)
			fread(&padding, sizeof(unsigned char), 1, f);
	}
	fclose(f);  //sfarsit citire
	//black & white
	black_white(bitMap, fileheader, infoheader, image);
	//no crop
	no_crop(bitMap, *fileheader, *infoheader, image);
	//filter
	char *filter_name = malloc(28 * sizeof(char));
	fscanf(g, "%s\n", filter_name);
	filter(bitMap, fileheader, infoheader, image, filter_name);
	free(filter_name);
	//pooling
	char *pooling_name = malloc(35 * sizeof(char)), pool_type;
	int pool_dim;
	fscanf(g, "%s\n", pooling_name);
	f = fopen(pooling_name, "r");
	fscanf(f, "%c\n", &pool_type);
	fscanf(f, "%d\n", &pool_dim);
	fclose(f);
	pooling(bitMap, fileheader, infoheader, image, pool_type, pool_dim);
	free(pooling_name);
	//clustering
	int threshold;
	char *clustering_name = malloc(35 * sizeof(char));
	fscanf(g, "%s\n", clustering_name);
	fclose(g);
	f = fopen(clustering_name, "r");
	fscanf(f, "%d\n", &threshold);
	fclose(f);
	clustering(bitMap, fileheader, infoheader, image, threshold);
	free(clustering_name);

	free(image);
	free(fileheader);
	free(infoheader);
}