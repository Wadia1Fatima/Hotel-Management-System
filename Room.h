#ifndef ROOM_H
#define ROOM_H

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>

// Abstract base class: common data and rules for every room
class Room {
protected:
    int roomNumber;
    double basePrice;
    bool available;

private:
    static int totalRooms;

public:
    Room(int roomNumber, double basePrice);
    virtual ~Room();

    static int getTotalRooms();

    int getRoomNumber() const;
    bool isAvailable() const;
    void setAvailable(bool value);
    double getBasePrice() const;

    virtual double getPricePerNight() const = 0;
    virtual std::string getType() const = 0;
    virtual int getCapacity() const = 0;
    virtual std::string getFacilities() const = 0;

    virtual std::string toString() const;
    bool operator==(const Room& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Room& r);
    void display() const;
};

// Single room: 1 guest, price equals base price
class SingleRoom : public Room {
public:
    SingleRoom(int roomNumber, double basePrice);
    double getPricePerNight() const override;
    std::string getType() const override;
    int getCapacity() const override;
    std::string getFacilities() const override;
};

// Double room: 2 guests, price is 1.5 times base price
class DoubleRoom : public Room {
public:
    DoubleRoom(int roomNumber, double basePrice);
    double getPricePerNight() const override;
    std::string getType() const override;
    int getCapacity() const override;
    std::string getFacilities() const override;
};

// Suite: 4 guests, price is 2.5 times base price
class Suite : public Room {
public:
    Suite(int roomNumber, double basePrice);
    double getPricePerNight() const override;
    std::string getType() const override;
    int getCapacity() const override;
    std::string getFacilities() const override;
};

// Manager: owns all room objects and deletes them in its destructor
class RoomManager {
private:
    std::vector<Room*> rooms;
    std::string lastError;

public:
    RoomManager();
    ~RoomManager();
    RoomManager(const RoomManager&) = delete;
    RoomManager& operator=(const RoomManager&) = delete;

    bool addRoom(Room* room);
    Room* findRoom(int roomNumber);
    bool removeRoom(int roomNumber);
    std::vector<Room*> getAllRooms() const;
    std::vector<Room*> getAvailableRooms() const;
    std::vector<Room*> getRoomsByType(const std::string& type) const;
    int getTotalRoomCount() const;
    int getAvailableCount() const;
    void loadDefaultRooms();
    std::string getLastError() const;
    void display() const;
};

#endif