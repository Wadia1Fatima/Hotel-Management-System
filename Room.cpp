#include "Room.h"

using namespace std;

int Room::totalRooms = 0;

// Saves the room data and counts the new room
Room::Room(int roomNumber, double basePrice)
    : roomNumber(roomNumber), basePrice(basePrice), available(true) {
    totalRooms++;
}

// Uncounts the room when it is destroyed
Room::~Room() {
    totalRooms--;
}

// Returns how many Room objects exist now
int Room::getTotalRooms() {
    return totalRooms;
}

// Returns the room number
int Room::getRoomNumber() const {
    return roomNumber;
}

// Returns true if the room is free
bool Room::isAvailable() const {
    return available;
}

// Marks the room as free or booked
void Room::setAvailable(bool value) {
    available = value;
}

// Returns the base price before type multiplier
double Room::getBasePrice() const {
    return basePrice;
}

// Builds one line of text describing the room
string Room::toString() const {
    ostringstream out;
    out << "Room " << roomNumber
        << " | " << getType()
        << " | Price/night: " << fixed << setprecision(2) << getPricePerNight()
        << " | Capacity: " << getCapacity()
        << " | Facilities: " << getFacilities()
        << " | " << (available ? "Available" : "Booked");
    return out.str();
}

// Two rooms are equal if their room numbers are equal
bool Room::operator==(const Room& other) const {
    return roomNumber == other.roomNumber;
}

// Prints the room using toString()
ostream& operator<<(ostream& os, const Room& r) {
    os << r.toString();
    return os;
}

// Prints toString() to the screen
void Room::display() const {
    cout << toString() << endl;
}

// Passes data to the base class
SingleRoom::SingleRoom(int roomNumber, double basePrice)
    : Room(roomNumber, basePrice) {
}

// Single room price is the base price
double SingleRoom::getPricePerNight() const {
    return basePrice;
}

// Returns the type name
string SingleRoom::getType() const {
    return "Single";
}

// Single room holds 1 guest
int SingleRoom::getCapacity() const {
    return 1;
}

// Returns the facilities text
string SingleRoom::getFacilities() const {
    return "WiFi, TV";
}

// Passes data to the base class
DoubleRoom::DoubleRoom(int roomNumber, double basePrice)
    : Room(roomNumber, basePrice) {
}

// Double room price is base price times 1.5
double DoubleRoom::getPricePerNight() const {
    return basePrice * 1.5;
}

// Returns the type name
string DoubleRoom::getType() const {
    return "Double";
}

// Double room holds 2 guests
int DoubleRoom::getCapacity() const {
    return 2;
}

// Returns the facilities text
string DoubleRoom::getFacilities() const {
    return "WiFi, TV, Balcony";
}

// Passes data to the base class
Suite::Suite(int roomNumber, double basePrice)
    : Room(roomNumber, basePrice) {
}

// Suite price is base price times 2.5
double Suite::getPricePerNight() const {
    return basePrice * 2.5;
}

// Returns the type name
string Suite::getType() const {
    return "Suite";
}

// Suite holds 4 guests
int Suite::getCapacity() const {
    return 4;
}

// Returns the facilities text
string Suite::getFacilities() const {
    return "WiFi, TV, Balcony, Mini Bar, Lounge";
}

// Starts with an empty room list and no error
RoomManager::RoomManager() {
    lastError = "";
}

// Deletes every room the manager owns
RoomManager::~RoomManager() {
    for (size_t i = 0; i < rooms.size(); i++) {
        delete rooms[i];
    }
    rooms.clear();
}

// Adds a room if it is not null and the number is new
bool RoomManager::addRoom(Room* room) {
    if (room == nullptr) {
        lastError = "Cannot add a null room.";
        return false;
    }
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->getRoomNumber() == room->getRoomNumber()) {
            lastError = "Room number already exists.";
            return false;
        }
    }
    rooms.push_back(room);
    lastError = "";
    return true;
}

// Finds a room by number, or returns nullptr
Room* RoomManager::findRoom(int roomNumber) {
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->getRoomNumber() == roomNumber) {
            lastError = "";
            return rooms[i];
        }
    }
    lastError = "Room not found.";
    return nullptr;
}

// Removes and deletes a room only if it is available
bool RoomManager::removeRoom(int roomNumber) {
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->getRoomNumber() == roomNumber) {
            if (!rooms[i]->isAvailable()) {
                lastError = "Room is booked and cannot be removed.";
                return false;
            }
            delete rooms[i];
            rooms.erase(rooms.begin() + i);
            lastError = "";
            return true;
        }
    }
    lastError = "Room not found.";
    return false;
}

// Returns a copy of the list of all room pointers
vector<Room*> RoomManager::getAllRooms() const {
    return rooms;
}

// Returns only the rooms that are available
vector<Room*> RoomManager::getAvailableRooms() const {
    vector<Room*> result;
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->isAvailable()) {
            result.push_back(rooms[i]);
        }
    }
    return result;
}

// Returns only the rooms of the given type
vector<Room*> RoomManager::getRoomsByType(const string& type) const {
    vector<Room*> result;
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->getType() == type) {
            result.push_back(rooms[i]);
        }
    }
    return result;
}

// Returns how many rooms the manager has
int RoomManager::getTotalRoomCount() const {
    return (int)rooms.size();
}

// Returns how many rooms are available
int RoomManager::getAvailableCount() const {
    int count = 0;
    for (size_t i = 0; i < rooms.size(); i++) {
        if (rooms[i]->isAvailable()) {
            count++;
        }
    }
    return count;
}

// Adds 10 demo rooms: 4 single, 4 double, 2 suites
void RoomManager::loadDefaultRooms() {
    for (int n = 101; n <= 104; n++) {
        addRoom(new SingleRoom(n, 5000));
    }
    for (int n = 201; n <= 204; n++) {
        addRoom(new DoubleRoom(n, 5000));
    }
    addRoom(new Suite(301, 5000));
    addRoom(new Suite(302, 5000));
}

// Returns the last error message
string RoomManager::getLastError() const {
    return lastError;
}

// Prints all rooms
void RoomManager::display() const {
    for (size_t i = 0; i < rooms.size(); i++) {
        rooms[i]->display();
    }
}