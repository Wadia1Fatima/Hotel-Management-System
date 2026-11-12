#ifndef BOOKING_H
#define BOOKING_H
#include <string>
#include <vector>
#include <iostream>
#include "Room.h"
#include "Guest.h"

// The life-cycle states of a booking
enum class BookingStatus { Confirmed, CheckedIn, CheckedOut, Cancelled };

// Converts a BookingStatus to text
std::string statusToString(BookingStatus s);

// One reservation of one room by one guest
class Booking {
private:
    int bookingId;           // unique id taken from the static counter
    int guestId;             // id of the guest who booked
    int roomNumber;          // room that was booked
    int checkInDay;          // day of year (1 to 365)
    int nights;              // number of nights
    double pricePerNight;    // price saved at booking time
    BookingStatus status;    // current state
    static int nextBookingId; // counter shared by all bookings
public:
    // Creates a Confirmed booking with the next id
    Booking(int guestId, int roomNumber, int checkInDay, int nights, double pricePerNight);

    // Getters
    int getBookingId() const;
    int getGuestId() const;
    int getRoomNumber() const;
    int getCheckInDay() const;
    int getNights() const;
    double getPricePerNight() const;
    BookingStatus getStatus() const;
    int getCheckOutDay() const;       // checkInDay + nights
    double getEstimatedCost() const;  // nights * pricePerNight

    // Changes the status
    void setStatus(BookingStatus s);

    // Returns the booking as text
    std::string toString() const;

    // Two bookings are equal if they have the same bookingId
    bool operator==(const Booking& other) const;

    // Prints a booking with cout
    friend std::ostream& operator<<(std::ostream& os, const Booking& b);
};

// Manages all bookings and uses the room and guest managers
class BookingManager {
private:
    RoomManager& roomManager;        // reference to the room manager
    GuestManager& guestManager;      // reference to the guest manager
    std::vector<Booking> bookings;   // all bookings
    std::string lastError;           // message of the last failure
public:
    // Keeps references to the room and guest managers
    BookingManager(RoomManager& rm, GuestManager& gm);

    // Creates a booking, returns its id or -1 on failure
    int createBooking(int guestId, int roomNumber, int checkInDay, int nights);

    // Cancels a Confirmed booking and frees the room
    bool cancelBooking(int bookingId);

    // Checks in a Confirmed booking
    bool checkIn(int bookingId);

    // Checks out a CheckedIn booking and frees the room
    bool checkOut(int bookingId);

    // Finds a booking (safe only until the next createBooking)
    Booking* findBooking(int bookingId);

    // Returns a copy of all bookings
    std::vector<Booking> getAllBookings() const;

    // Returns the bookings of one guest
    std::vector<Booking> getBookingsByGuest(int guestId) const;

    // Returns the bookings with one status
    std::vector<Booking> getBookingsByStatus(BookingStatus s) const;

    // Returns the message of the last failure
    std::string getLastError() const;

    // Prints all bookings
    void display() const;
};

#endif
