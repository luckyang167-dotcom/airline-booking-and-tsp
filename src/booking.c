#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "booking.h"

void initBookingDB(BookingDB *db) {
    db->count = 0;
}

char *generateBookingId(void) {
    static char id[MAX_ID_LEN];
    static int counter = 1000;
    snprintf(id, sizeof(id), "BK-%04d", counter++);
    return id;
}

int createBooking(BookingDB *bdb, FlightDB *fdb,
                  const char *passenger, const char *flightCode,
                  int seats) {
    Flight *f = findFlight(fdb, flightCode);
    if (!f) {
        fprintf(stderr, "Error: flight %s not found\n", flightCode);
        return -1;
    }
    if (f->availableSeats < seats) {
        fprintf(stderr, "Error: insufficient seats (requested %d, available %d)\n",
                seats, f->availableSeats);
        return -1;
    }
    if (bdb->count >= MAX_BOOKINGS) {
        fprintf(stderr, "Error: booking database is full\n");
        return -1;
    }

    Booking b;
    strcpy(b.bookingId, generateBookingId());
    strncpy(b.passengerName, passenger, MAX_PASSENGER_NAME - 1);
    strncpy(b.flightCode, flightCode, MAX_FLIGHT_CODE - 1);
    b.seatCount = seats;
    b.totalPrice = f->price * seats;
    b.status = 0;

    time_t now = time(NULL);
    struct tm *tm_now = localtime(&now);
    strftime(b.bookingDate, sizeof(b.bookingDate), "%Y-%m-%d %H:%M:%S", tm_now);

    f->availableSeats -= seats;
    bdb->bookings[bdb->count++] = b;

    printf("Booking created successfully!\n");
    printf("  Booking ID: %s\n", b.bookingId);
    printf("  Passenger:  %s\n", b.passengerName);
    printf("  Flight:     %s\n", b.flightCode);
    printf("  Seats:      %d\n", b.seatCount);
    printf("  Total:      $%.2f\n", b.totalPrice);
    return 0;
}

int cancelBooking(BookingDB *bdb, FlightDB *fdb, const char *bookingId) {
    for (int i = 0; i < bdb->count; i++) {
        if (strcmp(bdb->bookings[i].bookingId, bookingId) == 0) {
            if (bdb->bookings[i].status == 1) {
                fprintf(stderr, "Error: booking already cancelled\n");
                return -1;
            }
            bdb->bookings[i].status = 1;

            Flight *f = findFlight(fdb, bdb->bookings[i].flightCode);
            if (f) {
                f->availableSeats += bdb->bookings[i].seatCount;
            }
            printf("Booking %s cancelled. Refund: $%.2f\n",
                   bookingId, bdb->bookings[i].totalPrice * 0.9);
            return 0;
        }
    }
    fprintf(stderr, "Error: booking %s not found\n", bookingId);
    return -1;
}

Booking *findBooking(BookingDB *db, const char *bookingId) {
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->bookings[i].bookingId, bookingId) == 0) {
            return &db->bookings[i];
        }
    }
    return NULL;
}

void listActiveBookings(BookingDB *db) {
    printf("\n%s\n", "========== ACTIVE BOOKINGS ==========");
    printf("%-12s %-20s %-10s %-5s %10s  %s\n",
           "ID", "Passenger", "Flight", "Seats", "Total", "Date");
    int active = 0;
    for (int i = 0; i < db->count; i++) {
        if (db->bookings[i].status == 0) {
            Booking *b = &db->bookings[i];
            printf("%-12s %-20s %-10s %-5d %10.2f  %s\n",
                   b->bookingId, b->passengerName, b->flightCode,
                   b->seatCount, b->totalPrice, b->bookingDate);
            active++;
        }
    }
    printf("Active bookings: %d\n\n", active);
}

void listPassengerBookings(BookingDB *db, const char *name) {
    printf("\nBookings for %s:\n", name);
    printf("%-12s %-10s %-5s %10s  %s\n",
           "ID", "Flight", "Seats", "Total", "Status");
    for (int i = 0; i < db->count; i++) {
        if (strcmp(db->bookings[i].passengerName, name) == 0) {
            Booking *b = &db->bookings[i];
            printf("%-12s %-10s %-5d %10.2f  %s\n",
                   b->bookingId, b->flightCode,
                   b->seatCount, b->totalPrice,
                   b->status == 0 ? "Active" : "Cancelled");
        }
    }
}

int loadBookingsFromFile(BookingDB *db, const char *filename) {
    FILE *fp = fopen(filename, "r");
    if (!fp) return -1;
    char line[256];
    db->count = 0;
    while (fgets(line, sizeof(line), fp) && db->count < MAX_BOOKINGS) {
        line[strcspn(line, "\n")] = 0;
        if (line[0] == '#' || line[0] == '\0') continue;
        Booking b;
        if (sscanf(line, "%19[^,],%49[^,],%9[^,],%d,%lf,%19[^,],%d",
                   b.bookingId, b.passengerName, b.flightCode,
                   &b.seatCount, &b.totalPrice, b.bookingDate,
                   &b.status) >= 7) {
            db->bookings[db->count++] = b;
        }
    }
    fclose(fp);
    return db->count;
}

int saveBookingsToFile(BookingDB *db, const char *filename) {
    FILE *fp = fopen(filename, "w");
    if (!fp) return -1;
    for (int i = 0; i < db->count; i++) {
        Booking *b = &db->bookings[i];
        fprintf(fp, "%s,%s,%s,%d,%.2f,%s,%d\n",
                b->bookingId, b->passengerName, b->flightCode,
                b->seatCount, b->totalPrice, b->bookingDate, b->status);
    }
    fclose(fp);
    return db->count;
}
