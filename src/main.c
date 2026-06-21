/**
 * airline-booking-and-tsp
 * ========================
 * An integrated system combining:
 *   1) Airline Ticket Booking System
 *   2) Traveling Salesman Problem (TSP) Solver
 *
 * This program demonstrates C programming skills including:
 *   - Structured data management (linked data, file I/O)
 *   - Classical algorithms (Brute Force, Greedy, Genetic Algorithm)
 *   - Interactive CLI with a clean menu system
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "flight.h"
#include "booking.h"
#include "tsp.h"
#include <unistd.h>

static void printBanner(void) {
    printf("\n");
    printf("  =============================================\n");
    printf("  ||      Airline Booking & TSP Solver       ||\n");
    printf("  ||         C Language Project              ||\n");
    printf("  =============================================\n");
}

static void bookingMenu(FlightDB *fdb, BookingDB *bdb) {
    int choice;
    do {
        printf("\n--- Booking System ---\n");
        printf("1. List all flights\n");
        printf("2. Search flights by route\n");
        printf("3. Create booking\n");
        printf("4. Cancel booking\n");
        printf("5. List active bookings\n");
        printf("6. Find booking by ID\n");
        printf("0. Back to main menu\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            listFlights(fdb);
            break;
        case 2: {
            char from[MAX_CITY_NAME], to[MAX_CITY_NAME];
            printf("Departure city: ");
            fgets(from, sizeof(from), stdin);
            from[strcspn(from, "\n")] = 0;
            printf("Arrival city: ");
            fgets(to, sizeof(to), stdin);
            to[strcspn(to, "\n")] = 0;
            listFlightsByRoute(fdb, from, to);
            break;
        }
        case 3: {
            char name[MAX_PASSENGER_NAME], code[MAX_FLIGHT_CODE];
            int seats;
            printf("Passenger name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            printf("Flight code: ");
            fgets(code, sizeof(code), stdin);
            code[strcspn(code, "\n")] = 0;
            printf("Number of seats: ");
            scanf("%d", &seats);
            getchar();
            createBooking(bdb, fdb, name, code, seats);
            break;
        }
        case 4: {
            char bid[MAX_ID_LEN];
            printf("Booking ID: ");
            fgets(bid, sizeof(bid), stdin);
            bid[strcspn(bid, "\n")] = 0;
            cancelBooking(bdb, fdb, bid);
            break;
        }
        case 5:
            listActiveBookings(bdb);
            break;
        case 6: {
            char bid[MAX_ID_LEN];
            printf("Booking ID: ");
            fgets(bid, sizeof(bid), stdin);
            bid[strcspn(bid, "\n")] = 0;
            Booking *b = findBooking(bdb, bid);
            if (b) {
                printf("Booking: %s | Passenger: %s | Flight: %s | Seats: %d | $%.2f | %s\n",
                       b->bookingId, b->passengerName, b->flightCode,
                       b->seatCount, b->totalPrice,
                       b->status == 0 ? "Active" : "Cancelled");
            } else {
                printf("Booking not found.\n");
            }
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

static void tspMenu(void) {
    CityList cl;
    initCityList(&cl);

    /* Pre-load some sample cities if file exists */
    if (access("data/cities.txt", F_OK) == 0) {
        loadCitiesFromFile(&cl, "data/cities.txt");
    } else {
        /* Add default sample cities */
        addCity(&cl, "Beijing",    116.40, 39.90);
        addCity(&cl, "Shanghai",   121.47, 31.23);
        addCity(&cl, "Guangzhou",  113.27, 23.13);
        addCity(&cl, "Shenzhen",   114.06, 22.54);
        addCity(&cl, "Chengdu",    104.07, 30.67);
        addCity(&cl, "Wuhan",      114.30, 30.60);
        addCity(&cl, "Xi'an",      108.95, 34.27);
        addCity(&cl, "Kunming",    102.68, 25.04);
    }

    int choice;
    do {
        printf("\n--- TSP Solver ---\n");
        printf("1. List cities\n");
        printf("2. Add a city\n");
        printf("3. Solve TSP (Brute Force)\n");
        printf("4. Solve TSP (Nearest Neighbor)\n");
        printf("5. Solve TSP (Genetic Algorithm)\n");
        printf("6. Load cities from file\n");
        printf("0. Back to main menu\n");
        printf("Cities: %d | Choice: ", cl.count);
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            printCities(&cl);
            break;
        case 2: {
            char name[MAX_CITY_NAME];
            double x, y;
            printf("City name: ");
            fgets(name, sizeof(name), stdin);
            name[strcspn(name, "\n")] = 0;
            printf("X coordinate: ");
            scanf("%lf", &x);
            printf("Y coordinate: ");
            scanf("%lf", &y);
            getchar();
            addCity(&cl, name, x, y);
            printf("City added.\n");
            break;
        }
        case 3:
            if (cl.count > 10) {
                printf("Warning: Brute Force is O(n!). "
                       "n=%d may be very slow.\n", cl.count);
            }
            if (cl.count > 1) {
                printf("Solving with Brute Force...\n");
                TSPResult r = solveTSPBruteForce(&cl);
                printRoute(&cl, r.route, cl.count);
            } else {
                printf("Need at least 2 cities.\n");
            }
            break;
        case 4:
            if (cl.count > 1) {
                printf("Solving with Nearest Neighbor...\n");
                TSPResult r = solveTSPNearestNeighbor(&cl);
                printRoute(&cl, r.route, cl.count);
            } else {
                printf("Need at least 2 cities.\n");
            }
            break;
        case 5:
            if (cl.count > 1) {
                int popSize = 100;
                int gens = 500;
                printf("Solving with Genetic Algorithm (pop=%d, gen=%d)...\n",
                       popSize, gens);
                TSPResult r = solveTSPGenetic(&cl, popSize, gens);
                printRoute(&cl, r.route, cl.count);
                saveRouteToFile(&cl, r.route, cl.count,
                                "output/tsp_result.txt");
                printf("Result saved to output/tsp_result.txt\n");
            } else {
                printf("Need at least 2 cities.\n");
            }
            break;
        case 6: {
            char fname[100];
            printf("Filename: ");
            fgets(fname, sizeof(fname), stdin);
            fname[strcspn(fname, "\n")] = 0;
            int n = loadCitiesFromFile(&cl, fname);
            if (n > 0)
                printf("Loaded %d cities.\n", n);
            else
                printf("Failed to load.\n");
            break;
        }
        case 0:
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);
}

int main(void) {
    FlightDB fdb;
    BookingDB bdb;

    initFlightDB(&fdb);
    initBookingDB(&bdb);

    srand(time(NULL));

    /* Load data files if they exist */
    if (access("data/flights.txt", F_OK) == 0) {
        int n = loadFlightsFromFile(&fdb, "data/flights.txt");
        printf("Loaded %d flights.\n", n);
    } else {
        /* Pre-populate with sample data */
        Flight sample[] = {
            {"CA1234", "Air China",    "Beijing",   "Shanghai",  "07:00", "09:15", 135, 1200.0, 180, 180, "2026-07-01"},
            {"MU5678", "China Eastern","Shanghai",  "Guangzhou", "10:00", "12:30", 150, 980.0,  200, 200, "2026-07-01"},
            {"CZ9012", "China Southern","Guangzhou","Shenzhen",  "08:30", "09:15", 45,  450.0,  120, 120, "2026-07-01"},
            {"3U3456", "Sichuan Airlines","Chengdu","Beijing",   "14:00", "16:30", 150, 1100.0, 160, 160, "2026-07-02"},
            {"HU7890", "Hainan Airlines","Xi'an",   "Wuhan",     "09:00", "10:40", 100, 680.0,  150, 150, "2026-07-02"},
            {"ZH2345", "Shenzhen Airlines","Shenzhen","Kunming", "11:00", "13:20", 140, 890.0,  170, 170, "2026-07-02"},
            {"MF6789", "Xiamen Airlines","Shanghai","Chengdu",   "16:00", "19:10", 190, 1350.0, 190, 190, "2026-07-03"},
            {"CA4321", "Air China",    "Beijing",   "Guangzhou", "06:30", "09:50", 200, 1580.0, 200, 200, "2026-07-03"},
            {"MU1111", "China Eastern","Kunming",   "Xi'an",     "13:00", "15:20", 140, 750.0,  140, 140, "2026-07-03"},
            {"CZ2222", "China Southern","Wuhan",    "Shanghai",  "17:00", "18:50", 110, 620.0,  160, 160, "2026-07-04"},
        };
        int ns = sizeof(sample) / sizeof(sample[0]);
        for (int i = 0; i < ns; i++) {
            addFlight(&fdb, sample[i]);
        }
        printf("Loaded %d sample flights.\n", ns);
    }

    int choice;
    do {
        printBanner();
        printf("1. Booking System\n");
        printf("2. TSP Solver\n");
        printf("0. Exit\n");
        printf("Choice: ");
        scanf("%d", &choice);
        getchar();

        switch (choice) {
        case 1:
            bookingMenu(&fdb, &bdb);
            break;
        case 2:
            tspMenu();
            break;
        case 0:
            saveFlightsToFile(&fdb, "data/flights.txt");
            saveBookingsToFile(&bdb, "data/bookings.txt");
            printf("Data saved. Goodbye!\n");
            break;
        default:
            printf("Invalid choice.\n");
        }
    } while (choice != 0);

    return 0;
}
