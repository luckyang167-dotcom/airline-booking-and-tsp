#ifndef TSP_H
#define TSP_H

#define MAX_CITIES 100
#define MAX_CITY_NAME 30
#define INF 1e9

typedef struct {
    char name[MAX_CITY_NAME];
    double x;
    double y;
} City;

typedef struct {
    City cities[MAX_CITIES];
    int count;
} CityList;

typedef struct {
    int route[MAX_CITIES];
    double totalDistance;
} TSPResult;

/* Distance calculation */
double euclideanDistance(City a, City b);

/* TSP solvers */
TSPResult solveTSPBruteForce(CityList *cl);
TSPResult solveTSPNearestNeighbor(CityList *cl);
TSPResult solveTSPGenetic(CityList *cl, int popSize, int generations);

/* City data management */
void initCityList(CityList *cl);
int addCity(CityList *cl, const char *name, double x, double y);
void printCities(CityList *cl);
void printRoute(CityList *cl, int *route, int count);
int loadCitiesFromFile(CityList *cl, const char *filename);
int saveRouteToFile(CityList *cl, int *route, int count, const char *filename);

#endif /* TSP_H */
