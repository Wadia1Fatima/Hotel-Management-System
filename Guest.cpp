#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "Guest.h"
using namespace std;

// ---------------- Guest ----------------

// Pass id, name, phone to Person; store cnic
Guest::Guest(int id, const std::string& name, const std::string& phone, const std::string& cnic)
    : Person(id, name, phone), cnic(cnic) {
}

// Returns the CNIC
std::string Guest::getCnic() const {
    return cnic;
}

// Role of a normal guest
std::string Guest::getRole() const {
    return "Guest";
}

// Regular guests get no discount
double Guest::getDiscountRate() const {
    return 0.0;
}

// Type text of a normal guest
std::string Guest::getGuestType() const {
    return "Regular";
}

// Builds one line with all guest data
std::string Guest::toString() const {
    ostringstream oss;
    oss << "ID: " << getId()
        << " | Name: " << getName()
        << " | Phone: " << getPhone()
        << " | CNIC: " << cnic
        << " | Type: " << getGuestType();
    return oss.str();
}

// Equal if the ids are the same
bool Guest::operator==(const Guest& other) const {
    return getId() == other.getId();
}

// Sends toString() to the output stream
std::ostream& operator<<(std::ostream& os, const Guest& g) {
    os << g.toString();
    return os;
}

// Prints the guest
void Guest::display() const {
    cout << toString() << endl;
}

// ---------------- VIPGuest ----------------

// Reuse the Guest constructor
VIPGuest::VIPGuest(int id, const std::string& name, const std::string& phone, const std::string& cnic)
    : Guest(id, name, phone, cnic) {
}

// VIP guests get 10% discount
double VIPGuest::getDiscountRate() const {
    return 0.10;
}

// Type text of a VIP guest
std::string VIPGuest::getGuestType() const {
    return "VIP";
}

// Role of a VIP guest
std::string VIPGuest::getRole() const {
    return "VIP Guest";
}

// ---------------- GuestManager ----------------

// Define the static counter (declared in the .h)
int GuestManager::nextGuestId = 1001;

// Start with no guests and no error
GuestManager::GuestManager() : lastError("") {
}

// Delete every guest object we own
GuestManager::~GuestManager() {
    for (size_t i = 0; i < guests.size(); i++) {
        delete guests[i];
    }
    guests.clear();
}

// Give the current id, then move to the next one
int GuestManager::generateGuestId() {
    int id = nextGuestId;
    nextGuestId++;
    return id;
}

// Add a guest to the list
bool GuestManager::addGuest(Guest* g) {
    if (g == nullptr) {
        lastError = "Guest pointer is null";
        return false;
    }
    for (size_t i = 0; i < guests.size(); i++) {
        if (guests[i]->getId() == g->getId()) {
            lastError = "Guest id already exists";
            return false;
        }
    }
    guests.push_back(g);
    return true;
}

// Look for a guest by id
Guest* GuestManager::findGuest(int id) {
    for (size_t i = 0; i < guests.size(); i++) {
        if (guests[i]->getId() == id) {
            return guests[i];
        }
    }
    lastError = "Guest not found";
    return nullptr;
}

// Delete the guest object and remove its pointer
bool GuestManager::removeGuest(int id) {
    for (size_t i = 0; i < guests.size(); i++) {
        if (guests[i]->getId() == id) {
            delete guests[i];
            guests.erase(guests.begin() + i);
            return true;
        }
    }
    lastError = "Guest not found";
    return false;
}

// Change the phone number of one guest
bool GuestManager::updateGuestPhone(int id, const std::string& newPhone) {
    Guest* g = findGuest(id);
    if (g == nullptr) {
        return false;   // findGuest already set lastError
    }
    g->setPhone(newPhone);
    return true;
}

// Collect all guests whose name contains the text
std::vector<Guest*> GuestManager::searchByName(const std::string& text) {
    std::vector<Guest*> result;
    for (size_t i = 0; i < guests.size(); i++) {
        if (guests[i]->getName().find(text) != std::string::npos) {
            result.push_back(guests[i]);
        }
    }
    return result;
}

// Return a copy of the pointer list
std::vector<Guest*> GuestManager::getAllGuests() const {
    return guests;
}

// Number of guests
int GuestManager::getGuestCount() const {
    return (int)guests.size();
}

// Last error text
std::string GuestManager::getLastError() const {
    return lastError;
}

// Print every guest
void GuestManager::display() const {
    for (size_t i = 0; i < guests.size(); i++) {
        guests[i]->display();
    }
}