#ifndef STAFF_H
#define STAFF_H

#include <string>
#include <vector>
#include <ostream>
#include "Person.h"

// Abstract base class for all hotel employees.
class Staff : public Person {
protected:
    double baseSalary;
private:
    std::string password;
public:
    Staff(int id, const std::string& name, const std::string& phone,
        double baseSalary, const std::string& password);
    virtual ~Staff();

    // Returns the base salary.
    double getBaseSalary() const;
    // Returns true if the password matches.
    bool checkPassword(const std::string& p) const;

    virtual double calculateSalary() const = 0;
    virtual bool canManageStaff() const;
    virtual bool canHandleBookings() const;
    virtual bool canManageRooms() const;

    std::string toString() const override;
    bool operator==(const Staff& other) const;

    friend std::ostream& operator<<(std::ostream& os, const Staff& s);
};

class Manager : public Staff {
public:
    Manager(int id, const std::string& name, const std::string& phone,
        double baseSalary, const std::string& password);
    std::string getRole() const override;
    double calculateSalary() const override;
    bool canManageStaff() const override;
    bool canHandleBookings() const override;
    bool canManageRooms() const override;
};

// Receptionist: base salary + 5000 allowance, handles bookings.
class Receptionist : public Staff {
public:
    Receptionist(int id, const std::string& name, const std::string& phone,
        double baseSalary, const std::string& password);
    std::string getRole() const override;
    double calculateSalary() const override;
    bool canManageStaff() const override;
    bool canHandleBookings() const override;
    bool canManageRooms() const override;
};

// Housekeeper: base salary only, manages rooms.
class Housekeeper : public Staff {
public:
    // Creates a housekeeper.
    Housekeeper(int id, const std::string& name, const std::string& phone,
        double baseSalary, const std::string& password);
    // Returns "Housekeeper".
    std::string getRole() const override;
    // Salary is just the base salary.
    double calculateSalary() const override;
    bool canManageStaff() const override;
    bool canHandleBookings() const override;
    bool canManageRooms() const override;
};

// Owns and manages all staff objects through Staff pointers.
class StaffManager {
private:
    std::vector<Staff*> staffList;   // all staff (owned by this manager)
    std::string lastError;           // message of the last failure

    // Copying is blocked because the manager owns raw pointers.
    StaffManager(const StaffManager&);
    StaffManager& operator=(const StaffManager&);
public:
    // Creates an empty manager.
    StaffManager();
    // Deletes every staff object.
    ~StaffManager();

    // Adds staff; false if nullptr or duplicate id.
    bool addStaff(Staff* s);
    bool removeStaff(int id);
    Staff* findStaff(int id);
    Staff* login(int id, const std::string& password);
    std::vector<Staff*> getStaffByRole(const std::string& role);
    std::vector<Staff*> getAllStaff() const;
    double getTotalSalaryExpense() const;
    std::string getLastError() const;

    void display() const;
};

#endif