#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "Booking.h"
using namespace std;

// ---------- statusToString ----------

// Converts a BookingStatus to text
string statusToString(BookingStatus s) {
    if (s == BookingStatus::Confirmed) return "Confirmed";
    if (s == BookingStatus::CheckedIn) return "CheckedIn";
    if (s == BookingStatus::CheckedOut) return "CheckedOut";
    return "Cancelled";
}

// ---------- Booking ----------

// Static counter starts at 1
int Booking::nextBookingId = 1;

// Takes the next id, status starts as Confirmed
Booking::Booking(int guestId, int roomNumber, int checkInDay, int nights, double pricePerNight)
    : bookingId(nextBookingId++), guestId(guestId), roomNumber(roomNumber),
      checkInDay(checkInDay), nights(nights), pricePerNight(pricePerNight),
      status(BookingStatus::Confirmed) {}

// Returns the booking id
int Booking::getBookingId() const { return bookingId; }

// Returns the guest id
int Booking::getGuestId() const { return guestId; }

// Returns the room number
int Booking::getRoomNumber() const { return roomNumber; }

// Returns the check-in day
int Booking::getCheckInDay() const { return checkInDay; }

// Returns the number of nights
int Booking::getNights() const { return nights; }

// Returns the saved price per night
double Booking::getPricePerNight() const { return pricePerNight; }

// Returns the current status
BookingStatus Booking::getStatus() const { return status; }

// Check-out day = check-in day + nights
int Booking::getCheckOutDay() const { return checkInDay + nights; }

// Cost = nights * price per night
double Booking::getEstimatedCost() const { return nights * pricePerNight; }

// Sets a new status
void Booking::setStatus(BookingStatus s) { status = s; }

// Builds a text line for the booking
string Booking::toString() const {
    ostringstream out;
    out << "Booking #" << bookingId
        << ", Guest ID: " << guestId
        << ", Room: " << roomNumber
        << ", Check-in day: " << checkInDay
        << ", Check-out day: " << getCheckOutDay()
        << ", Nights: " << nights
        << ", Price/night: " << fixed << setprecision(2) << pricePerNight
        << ", Estimated cost: " << getEstimatedCost()
        << ", Status: " << statusToString(status);
    return out.str();
}

// Equal if the bookingId is the same
bool Booking::operator==(const Booking& other) const {
    return bookingId == other.bookingId;
}

// Friend function: prints toString() to the stream
ostream& operator<<(ostream& os, const Booking& b) {
    os << b.toString();
    return os;
}

// ---------- BookingManager ----------

// Keeps references to the two managers
BookingManager::BookingManager(RoomManager& rm, GuestManager& gm)
    : roomManager(rm), guestManager(gm), lastError("") {}

// Validates, then creates and saves a booking
int BookingManager::createBooking(int guestId, int roomNumber, int checkInDay, int nights) {
    if (nights <= 0) {
        lastError = "Nights must be greater than 0.";
        return -1;
    }
    if (checkInDay < 1 || checkInDay > 365) {
        lastError = "Check-in day must be between 1 and 365.";
        return -1;
    }
    if (checkInDay + nights > 365) {
        lastError = "Check-out day cannot be after day 365.";
        return -1;
    }
    Guest* guest = guestManager.findGuest(guestId);
    if (guest == nullptr) {
        lastError = "Guest not found.";
        return -1;
    }
    Room* room = roomManager.findRoom(roomNumber);
    if (room == nullptr) {
        lastError = "Room not found.";
        return -1;
    }
    if (!room->isAvailable()) {
        lastError = "Room is not available.";
        return -1;
    }
    // Polymorphic call: the price depends on the real room type
    double price = room->getPricePerNight();
    Booking b(guestId, roomNumber, checkInDay, nights, price);
    room->setAvailable(false);
    bookings.push_back(b);
    lastError = "";
    return b.getBookingId();
}

// Cancels a Confirmed booking and frees the room
bool BookingManager::cancelBooking(int bookingId) {
    Booking* b = findBooking(bookingId);
    if (b == nullptr) {
        lastError = "Booking not found.";
        return false;
    }
    if (b->getStatus() != BookingStatus::Confirmed) {
        lastError = "Only a Confirmed booking can be cancelled.";
        return false;
    }
    b->setStatus(BookingStatus::Cancelled);
    Room* room = roomManager.findRoom(b->getRoomNumber());
    if (room != nullptr) room->setAvailable(true);
    lastError = "";
    return true;
}

// Checks in a Confirmed booking
bool BookingManager::checkIn(int bookingId) {
    Booking* b = findBooking(bookingId);
    if (b == nullptr) {
        lastError = "Booking not found.";
        return false;
    }
    if (b->getStatus() != BookingStatus::Confirmed) {
        lastError = "Only a Confirmed booking can be checked in.";
        return false;
    }
    b->setStatus(BookingStatus::CheckedIn);
    lastError = "";
    return true;
}

// Checks out a CheckedIn booking and frees the room
bool BookingManager::checkOut(int bookingId) {
    Booking* b = findBooking(bookingId);
    if (b == nullptr) {
        lastError = "Booking not found.";
        return false;
    }
    if (b->getStatus() != BookingStatus::CheckedIn) {
        lastError = "Only a CheckedIn booking can be checked out.";
        return false;
    }
    b->setStatus(BookingStatus::CheckedOut);
    Room* room = roomManager.findRoom(b->getRoomNumber());
    if (room != nullptr) room->setAvailable(true);
    lastError = "";
    return true;
}

// Searches the vector for the booking id
Booking* BookingManager::findBooking(int bookingId) {
    for (size_t i = 0; i < bookings.size(); i++) {
        if (bookings[i].getBookingId() == bookingId) return &bookings[i];
    }
    lastError = "Booking not found.";
    return nullptr;
}

// Returns a copy of all bookings
vector<Booking> BookingManager::getAllBookings() const {
    return bookings;
}

// Collects the bookings of one guest
vector<Booking> BookingManager::getBookingsByGuest(int guestId) const {
    vector<Booking> result;
    for (size_t i = 0; i < bookings.size(); i++) {
        if (bookings[i].getGuestId() == guestId) result.push_back(bookings[i]);
    }
    return result;
}

// Collects the bookings with one status
vector<Booking> BookingManager::getBookingsByStatus(BookingStatus s) const {
    vector<Booking> result;
    for (size_t i = 0; i < bookings.size(); i++) {
        if (bookings[i].getStatus() == s) result.push_back(bookings[i]);
    }
    return result;
}

// Returns the last error message
string BookingManager::getLastError() const {
    return lastError;
}

// Prints all bookings using toString()
void BookingManager::display() const {
    for (size_t i = 0; i < bookings.size(); i++) {
        cout << bookings[i].toString() << endl;
    }
}
