#ifndef GUEST_H
#define GUEST_H

#include <string>
#include <vector>
#include <iostream>
#include "Person.h"

// Guest: a hotel guest (a Person with a CNIC number)
class Guest : public Person {
private:
    std::string cnic;   // national ID card number

public:
    // Sets id, name, phone (in Person) and cnic
    Guest(int id, const std::string& name, const std::string& phone, const std::string& cnic);

    // Returns the CNIC
    std::string getCnic() const;

    // Returns the role name of this person
    std::string getRole() const override;

    // Returns discount rate (0.0 for a regular guest)
    virtual double getDiscountRate() const;

    // Returns the guest type text ("Regular")
    virtual std::string getGuestType() const;

    // Returns all guest data as one string
    std::string toString() const override;

    // Two guests are equal if their ids are equal
    bool operator==(const Guest& other) const;

    // Lets us write: cout << guest;
    friend std::ostream& operator<<(std::ostream& os, const Guest& g);

    // Prints toString()
    void display() const;
};

// VIPGuest: a guest who gets a 10% discount
class VIPGuest : public Guest {
public:
    // Passes all data to the Guest constructor
    VIPGuest(int id, const std::string& name, const std::string& phone, const std::string& cnic);

    // Returns 0.10
    double getDiscountRate() const override;

    // Returns "VIP"
    std::string getGuestType() const override;

    // Returns "VIP Guest"
    std::string getRole() const override;
};

// GuestManager: stores and manages all guests of the hotel
class GuestManager {
private:
    std::vector<Guest*> guests;   // pointers, so VIPGuest is not sliced
    std::string lastError;        // text of the last error
    static int nextGuestId;       // next id to give out (starts at 1001)

    // Copying is blocked, because the manager owns the guest pointers
    GuestManager(const GuestManager&) = delete;
    GuestManager& operator=(const GuestManager&) = delete;

public:
    // Creates an empty manager
    GuestManager();

    // Deletes all guests
    ~GuestManager();

    // Returns the next free guest id and increases the counter
    static int generateGuestId();

    // Adds a guest; false if g is nullptr or the id already exists
    bool addGuest(Guest* g);

    // Returns the guest with this id, or nullptr
    Guest* findGuest(int id);

    // Deletes the guest with this id; false if not found
    bool removeGuest(int id);

    // Changes the phone of a guest; false if not found
    bool updateGuestPhone(int id, const std::string& newPhone);

    // Returns guests whose name contains the text (case-sensitive)
    std::vector<Guest*> searchByName(const std::string& text);

    // Returns a copy of the pointer list
    std::vector<Guest*> getAllGuests() const;

    // Returns how many guests are stored
    int getGuestCount() const;

    // Returns the last error message
    std::string getLastError() const;

    // Prints all guests
    void display() const;
};

#endif