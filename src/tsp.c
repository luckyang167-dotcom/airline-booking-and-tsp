#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <float.h>
#include <time.h>
#include "tsp.h"

double euclideanDistance(City a, City b) {
    double dx = a.x - b.x;
    double dy = a.y - b.y;
    return sqrt(dx * dx + dy * dy);
}

static double routeDistance(CityList *cl, int *route, int count) {
    double dist = 0.0;
    for (int i = 0; i < count - 1; i++) {
        dist += euclideanDistance(cl->cities[route[i]], cl->cities[route[i + 1]]);
    }
    dist += euclideanDistance(cl->cities[route[count - 1]], cl->cities[route[0]]);
    return dist;
}

/* ---------- Brute Force (for small N) ---------- */
static int *bestRoute;
static double bestDist;
static int *currentPerm;
static int *used;

static void permute(CityList *cl, int pos, int n) {
    if (pos == n) {
        double d = routeDistance(cl, currentPerm, n);
        if (d < bestDist) {
            bestDist = d;
            memcpy(bestRoute, currentPerm, n * sizeof(int));
        }
        return;
    }
    for (int i = 0; i < n; i++) {
        if (!used[i]) {
            used[i] = 1;
            currentPerm[pos] = i;
            permute(cl, pos + 1, n);
            used[i] = 0;
        }
    }
}

TSPResult solveTSPBruteForce(CityList *cl) {
    TSPResult res;
    res.totalDistance = INF;
    int n = cl->count;

    bestRoute = malloc(n * sizeof(int));
    currentPerm = malloc(n * sizeof(int));
    used = calloc(n, sizeof(int));

    bestDist = INF;
    permute(cl, 0, n);

    memcpy(res.route, bestRoute, n * sizeof(int));
    res.totalDistance = bestDist;

    free(bestRoute);
    free(currentPerm);
    free(used);
    return res;
}

/* ---------- Nearest Neighbor (greedy) ---------- */
TSPResult solveTSPNearestNeighbor(CityList *cl) {
    int n = cl->count;
    int *visited = calloc(n, sizeof(int));
    TSPResult res;

    res.route[0] = 0;
    visited[0] = 1;
    int current = 0;

    for (int step = 1; step < n; step++) {
        int nearest = -1;
        double minDist = DBL_MAX;
        for (int j = 0; j < n; j++) {
            if (!visited[j]) {
                double d = euclideanDistance(cl->cities[current], cl->cities[j]);
                if (d < minDist) {
                    minDist = d;
                    nearest = j;
                }
            }
        }
        res.route[step] = nearest;
        visited[nearest] = 1;
        current = nearest;
    }

    res.totalDistance = routeDistance(cl, res.route, n);
    free(visited);
    return res;
}

/* ---------- Genetic Algorithm ---------- */
typedef struct {
    int *chromosome;
    double fitness;
} Individual;

static int cmpIndiv(const void *a, const void *b) {
    double fa = ((Individual *)a)->fitness;
    double fb = ((Individual *)b)->fitness;
    return (fa > fb) - (fa < fb);
}

TSPResult solveTSPGenetic(CityList *cl, int popSize, int generations) {
    int n = cl->count;
    if (n < 2) {
        TSPResult res = {.totalDistance = 0};
        if (n == 1) res.route[0] = 0;
        return res;
    }

    srand(time(NULL));

    /* Initialize population */
    Individual *pop = malloc(popSize * sizeof(Individual));
    for (int i = 0; i < popSize; i++) {
        pop[i].chromosome = malloc(n * sizeof(int));
        for (int j = 0; j < n; j++) pop[i].chromosome[j] = j;
        /* Fisher-Yates shuffle */
        for (int j = n - 1; j > 0; j--) {
            int k = rand() % (j + 1);
            int tmp = pop[i].chromosome[j];
            pop[i].chromosome[j] = pop[i].chromosome[k];
            pop[i].chromosome[k] = tmp;
        }
    }

    double mutationRate = 0.15;
    int *bestChrom = malloc(n * sizeof(int));
    double bestFitness = -INF;

    for (int gen = 0; gen < generations; gen++) {
        /* Evaluate */
        double totalDist = 0;
        double maxDist = 0;
        for (int i = 0; i < popSize; i++) {
            double d = routeDistance(cl, pop[i].chromosome, n);
            totalDist += d;
            if (d > maxDist) maxDist = d;
            pop[i].fitness = 1.0 / (d + 1.0);
        }

        /* Track best */
        for (int i = 0; i < popSize; i++) {
            if (pop[i].fitness > bestFitness) {
                bestFitness = pop[i].fitness;
                memcpy(bestChrom, pop[i].chromosome, n * sizeof(int));
            }
        }

        /* Selection via tournament */
        Individual *newPop = malloc(popSize * sizeof(Individual));
        for (int i = 0; i < popSize; i++) {
            int i1 = rand() % popSize;
            int i2 = rand() % popSize;
            int winner = (pop[i1].fitness > pop[i2].fitness) ? i1 : i2;
            newPop[i].chromosome = malloc(n * sizeof(int));
            memcpy(newPop[i].chromosome, pop[winner].chromosome, n * sizeof(int));
        }

        /* Crossover (Ordered Crossover) */
        for (int i = 0; i < popSize; i += 2) {
            int *p1 = newPop[i].chromosome;
            int *p2 = newPop[i + 1].chromosome;
            int start = rand() % n;
            int end = start + rand() % (n - start);

            int *child1 = malloc(n * sizeof(int));
            int *child2 = malloc(n * sizeof(int));
            for (int j = 0; j < n; j++) child1[j] = child2[j] = -1;

            /* Copy segment */
            for (int j = start; j <= end; j++) {
                child1[j] = p1[j];
                child2[j] = p2[j];
            }

            /* Fill remaining from other parent in order */
            int pos1 = (end + 1) % n;
            int pos2 = (end + 1) % n;
            for (int j = 0; j < n; j++) {
                int idx = (end + 1 + j) % n;
                int v1 = p2[idx];
                int v2 = p1[idx];
                int found1 = 0, found2 = 0;
                for (int k = 0; k < n; k++) {
                    if (child1[k] == v1) found1 = 1;
                    if (child2[k] == v2) found2 = 1;
                }
                if (!found1) { child1[pos1] = v1; pos1 = (pos1 + 1) % n; }
                if (!found2) { child2[pos2] = v2; pos2 = (pos2 + 1) % n; }
            }

            /* Mutation (swap) */
            for (int j = 0; j < n; j++) {
                if ((double)rand() / RAND_MAX < mutationRate) {
                    int k = rand() % n;
                    int t = child1[j]; child1[j] = child1[k]; child1[k] = t;
                }
                if ((double)rand() / RAND_MAX < mutationRate) {
                    int k = rand() % n;
                    int t = child2[j]; child2[j] = child2[k]; child2[k] = t;
                }
            }

            memcpy(newPop[i].chromosome, child1, n * sizeof(int));
            if (i + 1 < popSize)
                memcpy(newPop[i + 1].chromosome, child2, n * sizeof(int));
            free(child1);
            free(child2);
        }

        /* Replace old population */
        for (int i = 0; i < popSize; i++) free(pop[i].chromosome);
        free(pop);
        pop = newPop;
    }

    /* Cleanup */
    for (int i = 0; i < popSize; i++) free(pop[i].chromosome);
    free(pop);

    TSPResult res;
    memcpy(res.route, bestChrom, n * sizeof(int));
    res.totalDistance = 1.0 / bestFitness - 1.0;
    free(bestChrom);
    return res;
}

/* ---------- Utility ---------- */
void initCityList(CityList *cl) {
    cl->count = 0;
}

int addCity(CityList *cl, const char *name, double x, double y) {
    if (cl->count >= MAX_CITIES) return -1;
    strncpy(cl->cities[cl->count].name, name, MAX_CITY_NAME - 1);
    cl->cities[cl->count].x = x;
    cl->cities[cl->count].y = y;
    cl->count++;
    return 0;
}

void printCities(CityList *cl) {
    printf("\n%s\n", "========== CITY LIST ==========");
    printf("%-4s %-20s %10s %10s\n", "No.", "City", "X", "Y");
    for (int i = 0; i < cl->count; i++) {
        printf("%-4d %-20s %10.2f %10.2f\n",
               i + 1, cl->cities[i].name, cl->cities[i].x, cl->cities[i].y);
    }
}

void printRoute(CityList *cl, int *route, int count) {
    printf("\nOptimized Route:\n");
    for (int i = 0; i < count; i++) {
        printf("  %s", cl->cities[route[i]].name);
        if (i < count - 1) printf(" ->");
    }
    printf(" -> %s\n", cl->cities[route[0]].name);
    printf("Total distance: %.2f\n", routeDistance(cl, route, count));
}

int loadCitiesFromFile(CityList *cl, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    char line[128];
    cl->count = 0;
    while (fgets(line, sizeof(line), fp) && cl->count < MAX_CITIES) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') continue;
        char name[MAX_CITY_NAME];
        double x, y;
        if (sscanf(line, "%29[^,],%lf,%lf", name, &x, &y) >= 3) {
            addCity(cl, name, x, y);
        }
    }
    fclose(fp);
    return cl->count;
}

int saveRouteToFile(CityList *cl, int *route, int count, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    fprintf(fp, "# Optimal Route for TSP\n");
    fprintf(fp, "# City Sequence:\n");
    for (int i = 0; i < count; i++) {
        fprintf(fp, "%d,%s\n", i + 1, cl->cities[route[i]].name);
    }
    fprintf(fp, "# Total Distance: %.2f\n", routeDistance(cl, route, count));
    fclose(fp);
    return 0;
}
