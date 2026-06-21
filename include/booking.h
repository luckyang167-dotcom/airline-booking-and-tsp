#ifndef BOOKING_H
#define BOOKING_H

#include "flight.h"

#define MAX_BOOKINGS 500
#define MAX_PASSENGER_NAME 50
#define MAX_ID_LEN 20

typedef struct {
    char bookingId[MAX_ID_LEN];
    char passengerName[MAX_PASSENGER_NAME];
    char flightCode[MAX_FLIGHT_CODE];
    int seatCount;
    double totalPrice;
    char bookingDate[MAX_DATE_LEN];
    int status;  /* 0 = active, 1 = cancelled */
} Booking;

typedef struct {
    Booking bookings[MAX_BOOKINGS];
    int count;
} BookingDB;

/* Booking operations */
void initBookingDB(BookingDB *db);
char *generateBookingId(void);
int createBooking(BookingDB *bdb, FlightDB *fdb,
                  const char *passenger, const char *flightCode,
                  int seats);
int cancelBooking(BookingDB *bdb, FlightDB *fdb, const char *bookingId);
Booking *findBooking(BookingDB *db, const char *bookingId);
void listActiveBookings(BookingDB *db);
void listPassengerBookings(BookingDB *db, const char *name);
int loadBookingsFromFile(BookingDB *db, const char *filename);
int saveBookingsToFile(BookingDB *db, const char *filename);

#endif /* BOOKING_H */
