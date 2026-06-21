#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "flight.h"

void initFlightDB(FlightDB *db) {
    db->count = 0;
}

int addFlight(FlightDB *db, Flight f) {
    if (db->count >= MAX_FLIGHTS) {
        fprintf(stderr, "Error: flight database is full\n");
        return -1;
    }
    if (findFlight(db, f.flightCode) != NULL) {
        fprintf(stderr, "Error: flight %s already exists\n", f.flightCode);
        return -1;
    }
    db->flights[db->count++] = f;
    return 0;
}

int removeFlight(FlightDB *db, const char *code) {
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->flights[i].flightCode, code) == 0) {
            db->flights[i] = db->flights[--db->count];
            return 0;
        }
    }
    fprintf(stderr, "Error: flight %s not found\n", code);
    return -1;
}

Flight *findFlight(FlightDB *db, const char *code) {
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->flights[i].flightCode, code) == 0) {
            return &db->flights[i];
        }
    }
    return NULL;
}

void listFlights(FlightDB *db) {
    printf("\n%s\n", "========== ALL FLIGHTS ==========");
    printf("%-10s %-15s %-15s -> %-15s %-6s %-6s %4s  %8s  %s\n",
           "Code", "Airline", "Departure", "Arrival",
           "Dep", "Arr", "Min", "Price", "Seats");
    for (int i = 0; i < db->count; i++) {
        Flight *f = &db->flights[i];
        printf("%-10s %-15s %-15s -> %-15s %-6s %-6s %4d  %8.2f  %d/%d\n",
               f->flightCode, f->airline, f->departure, f->arrival,
               f->departTime, f->arriveTime, f->durationMinutes,
               f->price, f->availableSeats, f->totalSeats);
    }
    printf("Total: %d flights\n\n", db->count);
}

void listFlightsByRoute(FlightDB *db, const char *from, const char *to) {
    printf("\nFlights from %s to %s:\n", from, to);
    printf("%-10s %-15s %-6s %-6s %4s  %8s  %s\n",
           "Code", "Airline", "Dep", "Arr", "Min", "Price", "Seats");
    int found = 0;
    for (int i = 0; i < db->count; i++) {
        Flight *f = &db->flights[i];
        if (strcmp(f->departure, from) == 0 && strcmp(f->arrival, to) == 0) {
            printf("%-10s %-15s %-6s %-6s %4d  %8.2f  %d\n",
                   f->flightCode, f->airline,
                   f->departTime, f->arriveTime,
                   f->durationMinutes, f->price, f->availableSeats);
            found++;
        }
    }
    printf("Found %d flight(s)\n\n", found);
}

int loadFlightsFromFile(FlightDB *db, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) {
        fprintf(stderr, "Error: cannot open %s\n", filename);
        return -1;
    }
    char line[256];
    db->count = 0;
    while (fgets(line, sizeof(line), fp) && db->count < MAX_FLIGHTS) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') continue;
        Flight f;
        if (sscanf(line, "%9[^,],%49[^,],%29[^,],%29[^,],%5[^,],%5[^,],%d,%lf,%d,%d,%19s",
                   f.flightCode, f.airline, f.departure, f.arrival,
                   f.departTime, f.arriveTime,
                   &f.durationMinutes, &f.price,
                   &f.totalSeats, &f.availableSeats, f.date) >= 10) {
            db->flights[db->count++] = f;
        }
    }
    fclose(fp);
    return db->count;
}

int saveFlightsToFile(FlightDB *db, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) {
        fprintf(stderr, "Error: cannot write %s\n", filename);
        return -1;
    }
    for (int i = 0; i < db->count; i++) {
        Flight *f = &db->flights[i];
        fprintf(fp, "%s,%s,%s,%s,%s,%s,%d,%.2f,%d,%d,%s\n",
                f->flightCode, f->airline, f->departure, f->arrival,
                f->departTime, f->arriveTime,
                f->durationMinutes, f->price,
                f->totalSeats, f->availableSeats, f->date);
    }
    fclose(fp);
    return db->count;
}
