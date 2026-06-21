#ifndef FLIGHT_H
#define FLIGHT_H

#define MAX_FLIGHTS 100
#define MAX_AIRLINE_NAME 50
#define MAX_FLIGHT_CODE 10
#define MAX_CITY_NAME 30
#define MAX_DATE_LEN 20

typedef struct {
    char flightCode[MAX_FLIGHT_CODE];
    char airline[MAX_AIRLINE_NAME];
    char departure[MAX_CITY_NAME];
    char arrival[MAX_CITY_NAME];
    char departTime[6];   /* HH:MM */
    char arriveTime[6];   /* HH:MM */
    int durationMinutes;
    double price;
    int totalSeats;
    int availableSeats;
    char date[MAX_DATE_LEN];
} Flight;

typedef struct {
    Flight flights[MAX_FLIGHTS];
    int count;
} FlightDB;

/* Flight database operations */
void initFlightDB(FlightDB *db);
int addFlight(FlightDB *db, Flight f);
int removeFlight(FlightDB *db, const char *code);
Flight *findFlight(FlightDB *db, const char *code);
void listFlights(FlightDB *db);
void listFlightsByRoute(FlightDB *db, const char *from, const char *to);
int loadFlightsFromFile(FlightDB *db, const char *filename);
int saveFlightsToFile(FlightDB *db, const char *filename);

#endif /* FLIGHT_H */
