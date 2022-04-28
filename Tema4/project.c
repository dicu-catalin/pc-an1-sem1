#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "include/project.h"

int *arrayProduct(int *array, int arrayLength) {
	int *v = (int *) malloc (sizeof(int) * arrayLength);
	for (int i = 0; i < arrayLength; i++){
		v[i] = 1;  //fiecare numar din vector ia valoarea 1 pentru a-l inmulti
		for (int j = 0; j < arrayLength; j++)
			if(i != j)  //exclud cazul in care se inmulteste cu el insusi
				v[i] = v[i] * array[j];
	}

	return v;
}

int **rotateMatrix(int **matrix, int matrixSize) {
    int **mat = (int**) malloc (matrixSize * sizeof(int*));

    for (int i = 0; i < matrixSize; i++)
    	mat[i] = (int*) malloc (matrixSize * sizeof(int));
    
    for (int i = 0; i < matrixSize; i++)
    	for (int j = 0; j < matrixSize; j++)
    		/*liniile din noua matrice sunt coloanele din prima matrice de la
    		dreapta la stanga*/
    		mat[i][j] = matrix[j][matrixSize-i-1];

    return mat;
}

int *subMatrixesSums(int **matrix, int queriesNo, int *queries) {
    int *v = (int*) malloc(queriesNo * sizeof(int));
    for(int i = 0; i < queriesNo; i++)
    	/*folosesc querry-urile din 4 in 4, matricea oprindu-se din calculat
    	in momentul in care se termina coloanele de pe linia respectiva*/
    	for(int x = queries[i*4]; x <= queries[i*4+2]; x++)
    		for(int y = queries[i*4+1]; y <= queries[i*4+3]; y++)
    			v[i] = v[i] + matrix[x][y];

    return v;
}

TDriver *allocDriver(int ridesNo) {
    TDriver *driver = malloc(sizeof(TDriver));
    
    if(!driver){
    	printf("Memoria petru sofer nu a fost alocata\n");
    	return -1;
    }
    
    driver->nr_rides = ridesNo;  //pastrez numarul de curse ale soferului
    driver->rides = malloc(sizeof(TRide*) * ridesNo);  //aloc vectorul de curse
    
    if(!driver->rides){
    	printf("Memoria pentru curse nu a fost alocata\n");
    	return -1;
    }

    for(int i = 0; i < ridesNo; i++)
    	driver->rides[i] = malloc(sizeof(TRide));  //aloc memorie pentru fiecare cursa

    return driver;
}

TDriver **allocDrivers(int driversNo, int *driversRidesNo) {
	TDriver **drivers = malloc(driversNo * sizeof(TDriver*));
	if(!drivers){
		printf("Memoria pentru soferi nu a fost alocata\n");
		return -1;
	}

    for(int i = 0; i < driversNo; i++)
    	drivers[i] = allocDriver(driversRidesNo[i]);  //aloc fiecare sofer

    return drivers;
}

TDriver **readDrivers(FILE *inputFile, int *driversNo) {
	int nr_drivers;
	fread(&nr_drivers, sizeof(int), 1, inputFile);
	*driversNo = nr_drivers;  //pastrez numarul de soferi
	int *driversRidesNo = malloc(sizeof(int) * (*driversNo));
	fread(driversRidesNo, sizeof(int), *driversNo, inputFile);
	TDriver **drivers = allocDrivers(*driversNo, driversRidesNo);
	for(int i = 0; i < *driversNo; i++){
		fread(drivers[i]->name, sizeof(char), 20, inputFile);
		fread(drivers[i]->lic_plate, sizeof(char), 8, inputFile);
		fread(&drivers[i]->location_curr_x, sizeof(double), 1, inputFile);
		fread(&drivers[i]->location_curr_y, sizeof(double), 1, inputFile);
		for(int j = 0; j < driversRidesNo[i]; j++){
			fread(&drivers[i]->rides[j]->location_x, sizeof(double), 1, inputFile);
			fread(&drivers[i]->rides[j]->location_y, sizeof(double), 1, inputFile);
			fread(&drivers[i]->rides[j]->ride_status, sizeof(unsigned char), 1, inputFile);
		}
	}
	free(driversRidesNo);
	fclose(inputFile);
	return drivers;
}

void printDrivers(FILE *outputFile, TDriver **drivers, int driversNo) {
    for(int i = 0; i < driversNo; i++){
   		fwrite(drivers[i]->name, sizeof(char), 20, outputFile);
		fwrite(drivers[i]->lic_plate, sizeof(char), 8, outputFile);
		fwrite(&drivers[i]->location_curr_x, sizeof(double), 1, outputFile);
		fwrite(&drivers[i]->location_curr_y, sizeof(double), 1, outputFile);
		for(unsigned int j = 0; j < drivers[i]->nr_rides; j++){
			fwrite(&drivers[i]->rides[j]->location_x, sizeof(double), 1, outputFile);
			fwrite(&drivers[i]->rides[j]->location_y, sizeof(double), 1, outputFile);
			fwrite(&drivers[i]->rides[j]->ride_status, sizeof(unsigned char), 1, outputFile);	
    	}
    }
    fclose(outputFile);
}

char *maxRatingDriverName(TDriver **drivers, int driversNo) {
	int no_maxRatingDriver = 0;
	float max_stars = 0;
	for(int i = 0; i < driversNo; i++){
		float no_rides_ended = 0, stars = 0;
		for (unsigned int j = 0; j < drivers[i]->nr_rides; j++)
		{
			unsigned char status = drivers[i]->rides[j]->ride_status;
			if(status <= 5){
				stars = stars + status;  //pastrez numarul de stele adunate
				no_rides_ended++;  //pastrez numarul de curse terminate
			}
		}
		if(stars/no_rides_ended > max_stars){
			max_stars = stars/no_rides_ended;  //pastrez cea mai mare medie de stele
			no_maxRatingDriver = i;
		}
	}
	return drivers[no_maxRatingDriver]->name;
}

double distance(double desiredX, double desiredY, double location_x,
	double location_y) {
	double dist;
	//calculez distanta euclidiana
	dist = (location_x - desiredX)*(location_x - desiredX) +
		   (location_y - desiredY)*(location_y - desiredY);
	return dist;
}

TDriver **getClosestDrivers(TDriver **drivers, int driversNo, double desiredX,
    double desiredY, int resultsNo) {
	double *dist = malloc(driversNo * sizeof(double));
	TDriver **closestDrivers = malloc(resultsNo * sizeof(TDriver*));

	for(int i = 0; i < driversNo; i++){
		//pastrez toate distantele intr-un vector
		dist[i] = distance(desiredX, desiredY, drivers[i]->location_curr_x,
				  drivers[i]->location_curr_y);
	}

	int aux;
	TDriver *auxDriver;
	//sortez vectorul de soferi in functie de distanta si lexicografic
	for(int i = 0; i < driversNo - 1; i++)
		for (int j = 0; j < driversNo - i - 1; j++){
			if(dist[j] > dist[j+1]){
				aux = dist[j];
				dist[j] = dist[j+1];
				dist[j+1] = aux;
				auxDriver = drivers[j];
				drivers[j] = drivers[j+1];
				drivers[j+1] = auxDriver;
			}
			else if(dist[j] == dist[j+1] && strcmp(drivers[j]->name, drivers[j+1]->name) > 0){
				auxDriver = drivers[j];
				drivers[j] = drivers[j+1];
				drivers[j+1] = auxDriver;
			}
		}
	free(dist);

	for (int i = 0; i < resultsNo; ++i)
	{
		closestDrivers[i] = allocDriver(drivers[i]->nr_rides);
		closestDrivers[i] = drivers[i];  //pastrez primii resultsNo soferi
	}

	return closestDrivers;
}

void freeDriver(TDriver *driver) {
	for(unsigned int i = 0; i < driver->nr_rides; i++)
		free(driver->rides[i]);  //eliberez fiecare cursa
	free(driver->rides);  //eliberez vectorul de curse
	free(driver);  //eliberez soferul

	return;
}

void freeDrivers(TDriver **drivers, int driversNo) {
	for(int i = 0; i < driversNo; i++)
		freeDriver(drivers[i]);  //eliberez fiecare sofer
	free(drivers);  //eliberez vectorul de soferi

	return;
}
