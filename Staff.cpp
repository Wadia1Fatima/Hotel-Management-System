#include <iostream>
#include <sstream>
#include <iomanip>
#include "Staff.h"
using namespace std;




Staff::Staff(int id, const string& name, const string& phone,
    double baseSalary, const string& password)
    : Person(id, name, phone), baseSalary(baseSalary), password(password) {
}


Staff::~Staff() {}

double Staff::getBaseSalary() const { return baseSalary; }


bool Staff::checkPassword(const string& p) const { return password == p; }


bool Staff::canManageStaff() const { return false; }
bool Staff::canHandleBookings() const { return false; }
bool Staff::canManageRooms() const { return false; }


string Staff::toString() const {
    ostringstream out;
    out << "ID: " << id << " | Name: " << name << " | Phone: " << phone
        << " | Role: " << getRole()
        << " | Salary: " << fixed << setprecision(2) << calculateSalary();
    return out.str();
}

bool Staff::operator==(const Staff& other) const { return id == other.id; }


ostream& operator<<(ostream& os, const Staff& s) {
    os << s.toString();
    return os;
}



Manager::Manager(int id, const string& name, const string& phone,
    double baseSalary, const string& password)
    : Staff(id, name, phone, baseSalary, password) {
}

string Manager::getRole() const { return "Manager"; }


double Manager::calculateSalary() const { return baseSalary + baseSalary * 0.20; }

bool Manager::canManageStaff() const { return true; }
bool Manager::canHandleBookings() const { return true; }
bool Manager::canManageRooms() const { return true; }



Receptionist::Receptionist(int id, const string& name, const string& phone,
    double baseSalary, const string& password)
    : Staff(id, name, phone, baseSalary, password) {
}

string Receptionist::getRole() const { return "Receptionist"; }


double Receptionist::calculateSalary() const { return baseSalary + 5000; }

bool Receptionist::canManageStaff() const { return false; }
bool Receptionist::canHandleBookings() const { return true; }
bool Receptionist::canManageRooms() const { return false; }



Housekeeper::Housekeeper(int id, const string& name, const string& phone,
    double baseSalary, const string& password)
    : Staff(id, name, phone, baseSalary, password) {
}

string Housekeeper::getRole() const { return "Housekeeper"; }

// No bonus.
double Housekeeper::calculateSalary() const { return baseSalary; }

bool Housekeeper::canManageStaff() const { return false; }
bool Housekeeper::canHandleBookings() const { return false; }
bool Housekeeper::canManageRooms() const { return true; }




StaffManager::StaffManager() : lastError("") {}


StaffManager::~StaffManager() {
    for (size_t i = 0; i < staffList.size(); i++) {
        delete staffList[i];
    }
    staffList.clear();
}

// Rejects nullptr and duplicate ids.
bool StaffManager::addStaff(Staff* s) {
    if (s == nullptr) {
        lastError = "Cannot add a null staff.";
        return false;
    }
    if (findStaff(s->getId()) != nullptr) {
        lastError = "Staff with this ID already exists.";
        return false;
    }
    staffList.push_back(s);
    lastError = "";
    return true;
}

// Finds the staff, deletes the object and erases the pointer.
bool StaffManager::removeStaff(int id) {
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i]->getId() == id) {
            delete staffList[i];
            staffList.erase(staffList.begin() + i);
            lastError = "";
            return true;
        }
    }
    lastError = "Staff not found.";
    return false;
}

// Linear search by id.
Staff* StaffManager::findStaff(int id) {
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i]->getId() == id) {
            return staffList[i];
        }
    }
    return nullptr;
}

// Checks id first, then password.
Staff* StaffManager::login(int id, const string& password) {
    Staff* s = findStaff(id);
    if (s == nullptr) {
        lastError = "Login failed: staff not found.";
        return nullptr;
    }
    if (!s->checkPassword(password)) {
        lastError = "Login failed: wrong password.";
        return nullptr;
    }
    lastError = "";
    return s;
}

// Collects staff whose getRole() matches.
vector<Staff*> StaffManager::getStaffByRole(const string& role) {
    vector<Staff*> result;
    for (size_t i = 0; i < staffList.size(); i++) {
        if (staffList[i]->getRole() == role) {
            result.push_back(staffList[i]);
        }
    }
    return result;
}

// Returns a copy of the pointer list.
vector<Staff*> StaffManager::getAllStaff() const { return staffList; }

// Each calculateSalary() call runs the version of the real object type.
double StaffManager::getTotalSalaryExpense() const {
    double total = 0;
    for (size_t i = 0; i < staffList.size(); i++) {
        total += staffList[i]->calculateSalary();
    }
    return total;
}

// Returns the last error text.
string StaffManager::getLastError() const { return lastError; }

// Prints every staff using toString().
void StaffManager::display() const {
    for (size_t i = 0; i < staffList.size(); i++) {
        cout << staffList[i]->toString() << endl;
    }
}