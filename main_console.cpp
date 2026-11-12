#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include "Room.h"
#include "Staff.h"
#include "Guest.h"
#include "Booking.h"
#include "Billing.h"

using namespace std;

int readInt(const string& prompt) {
    int value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid number. Try again: ";
    }
    cin.ignore(10000, '\n');
    return value;
}

double readDouble(const string& prompt) {
    double value;
    cout << prompt;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(10000, '\n');
        cout << "Invalid number. Try again: ";
    }
    cin.ignore(10000, '\n');
    return value;
}

string readText(const string& prompt) {
    string text;
    cout << prompt;
    getline(cin, text);
    return text;
}

double roundMoney(double value) {
    return (int)(value * 100 + 0.5) / 100.0;
}

void printRooms(const vector<Room*>& list) {
    if (list.empty()) {
        cout << "No rooms found." << endl;
        return;
    }
    for (size_t i = 0; i < list.size(); i++) {
        cout << *list[i] << endl;
    }
}

void printGuests(const vector<Guest*>& list) {
    if (list.empty()) {
        cout << "No guests found." << endl;
        return;
    }
    for (size_t i = 0; i < list.size(); i++) {
        cout << *list[i] << endl;
    }
}

void printStaff(const vector<Staff*>& list) {
    if (list.empty()) {
        cout << "No staff found." << endl;
        return;
    }
    for (size_t i = 0; i < list.size(); i++) {
        cout << *list[i] << endl;
    }
}

void printBookings(const vector<Booking>& list) {
    if (list.empty()) {
        cout << "No bookings found." << endl;
        return;
    }
    for (size_t i = 0; i < list.size(); i++) {
        cout << list[i] << endl;
    }
}

void loadDemoData(RoomManager& rooms, StaffManager& staff, GuestManager& guests) {
    rooms.loadDefaultRooms();
    staff.addStaff(new Manager(1, "Ali", "0300-1111111", 100000, "admin123"));
    staff.addStaff(new Receptionist(2, "Sara", "0300-2222222", 40000, "rec123"));
    staff.addStaff(new Housekeeper(3, "Bilal", "0300-4444444", 30000, "house123"));
    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Ahmed Khan", "0301-5555555", "35202-1234567-1"));
    guests.addGuest(new VIPGuest(GuestManager::generateGuestId(), "Zara Malik", "0302-6666666", "35202-7654321-2"));
}

Staff* loginScreen(StaffManager& staff) {
    int attempts = 0;
    while (attempts < 3) {
        int id = readInt("Staff ID (0 to exit): ");
        if (id == 0) {
            return nullptr;
        }
        string password = readText("Password: ");
        Staff* s = staff.login(id, password);
        if (s != nullptr) {
            return s;
        }
        cout << staff.getLastError() << endl;
        attempts++;
    }
    cout << "Too many failed attempts." << endl;
    return nullptr;
}

void showRoomsByType(RoomManager& rooms) {
    int t = readInt("1 = Single, 2 = Double, 3 = Suite: ");
    string type = "";
    if (t == 1) {
        type = "Single";
    }
    else if (t == 2) {
        type = "Double";
    }
    else if (t == 3) {
        type = "Suite";
    }
    else {
        cout << "Invalid type." << endl;
        return;
    }
    printRooms(rooms.getRoomsByType(type));
}

void findRoomMenu(RoomManager& rooms) {
    int number = readInt("Room number: ");
    Room* r = rooms.findRoom(number);
    if (r == nullptr) {
        cout << rooms.getLastError() << endl;
    }
    else {
        cout << *r << endl;
    }
}

void addRoomMenu(RoomManager& rooms) {
    int type = readInt("Type (1 = Single, 2 = Double, 3 = Suite): ");
    int number = readInt("Room number: ");
    double price = readDouble("Base price: ");
    Room* r = nullptr;
    if (type == 1) {
        r = new SingleRoom(number, price);
    }
    else if (type == 2) {
        r = new DoubleRoom(number, price);
    }
    else if (type == 3) {
        r = new Suite(number, price);
    }
    else {
        cout << "Invalid type." << endl;
        return;
    }
    if (rooms.addRoom(r)) {
        cout << "Room added." << endl;
    }
    else {
        cout << rooms.getLastError() << endl;
        delete r;
    }
}

void removeRoomMenu(RoomManager& rooms) {
    int number = readInt("Room number to remove: ");
    if (rooms.removeRoom(number)) {
        cout << "Room removed." << endl;
    }
    else {
        cout << rooms.getLastError() << endl;
    }
}

void roomsMenu(const Staff& user, RoomManager& rooms) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "--- ROOMS ---" << endl;
        cout << "1. Show all rooms" << endl;
        cout << "2. Show available rooms" << endl;
        cout << "3. Show rooms by type" << endl;
        cout << "4. Find a room" << endl;
        if (user.canManageStaff()) {
            cout << "5. Add a room" << endl;
            cout << "6. Remove a room" << endl;
        }
        cout << "0. Back" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            printRooms(rooms.getAllRooms());
        }
        else if (choice == 2) {
            printRooms(rooms.getAvailableRooms());
            cout << "Available: " << rooms.getAvailableCount()
                << " of " << rooms.getTotalRoomCount() << endl;
        }
        else if (choice == 3) {
            showRoomsByType(rooms);
        }
        else if (choice == 4) {
            findRoomMenu(rooms);
        }
        else if (choice == 5 && user.canManageStaff()) {
            addRoomMenu(rooms);
        }
        else if (choice == 6 && user.canManageStaff()) {
            removeRoomMenu(rooms);
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

void registerGuestMenu(GuestManager& guests) {
    int type = readInt("1 = Regular guest, 2 = VIP guest: ");
    if (type != 1 && type != 2) {
        cout << "Invalid type." << endl;
        return;
    }
    string name = readText("Name: ");
    string phone = readText("Phone: ");
    string cnic = readText("CNIC: ");
    int id = GuestManager::generateGuestId();
    Guest* g = nullptr;
    if (type == 2) {
        g = new VIPGuest(id, name, phone, cnic);
    }
    else {
        g = new Guest(id, name, phone, cnic);
    }
    if (guests.addGuest(g)) {
        cout << "Guest registered. Guest ID: " << id << endl;
    }
    else {
        cout << guests.getLastError() << endl;
        delete g;
    }
}

void findGuestMenu(GuestManager& guests) {
    int id = readInt("Guest ID: ");
    Guest* g = guests.findGuest(id);
    if (g == nullptr) {
        cout << guests.getLastError() << endl;
    }
    else {
        cout << *g << endl;
    }
}

void searchGuestMenu(GuestManager& guests) {
    string text = readText("Part of the name (case-sensitive): ");
    printGuests(guests.searchByName(text));
}

void updatePhoneMenu(GuestManager& guests) {
    int id = readInt("Guest ID: ");
    string phone = readText("New phone: ");
    if (guests.updateGuestPhone(id, phone)) {
        cout << "Phone updated." << endl;
    }
    else {
        cout << guests.getLastError() << endl;
    }
}

void removeGuestMenu(GuestManager& guests, BookingManager& bookings) {
    int id = readInt("Guest ID to remove: ");
    vector<Booking> list = bookings.getBookingsByGuest(id);
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].getStatus() == BookingStatus::Confirmed ||
            list[i].getStatus() == BookingStatus::CheckedIn) {
            cout << "This guest has an active booking and cannot be removed." << endl;
            return;
        }
    }
    if (guests.removeGuest(id)) {
        cout << "Guest removed." << endl;
    }
    else {
        cout << guests.getLastError() << endl;
    }
}

void guestsMenu(const Staff& user, GuestManager& guests, BookingManager& bookings) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "--- GUESTS ---" << endl;
        cout << "1. Register a guest" << endl;
        cout << "2. Show all guests" << endl;
        cout << "3. Find a guest by ID" << endl;
        cout << "4. Search guests by name" << endl;
        cout << "5. Update a guest phone" << endl;
        if (user.canManageStaff()) {
            cout << "6. Remove a guest" << endl;
        }
        cout << "0. Back" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            registerGuestMenu(guests);
        }
        else if (choice == 2) {
            printGuests(guests.getAllGuests());
        }
        else if (choice == 3) {
            findGuestMenu(guests);
        }
        else if (choice == 4) {
            searchGuestMenu(guests);
        }
        else if (choice == 5) {
            updatePhoneMenu(guests);
        }
        else if (choice == 6 && user.canManageStaff()) {
            removeGuestMenu(guests, bookings);
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

void createBookingMenu(BookingManager& bookings, RoomManager& rooms) {
    int guestId = readInt("Guest ID: ");
    cout << "Available rooms:" << endl;
    printRooms(rooms.getAvailableRooms());
    int roomNumber = readInt("Room number: ");
    int day = readInt("Check-in day (1 to 365): ");
    int nights = readInt("Number of nights: ");
    int bookingId = bookings.createBooking(guestId, roomNumber, day, nights);
    if (bookingId == -1) {
        cout << bookings.getLastError() << endl;
        return;
    }
    Booking* b = bookings.findBooking(bookingId);
    cout << "Booking created." << endl;
    cout << *b << endl;
}

void checkInMenu(BookingManager& bookings) {
    int bookingId = readInt("Booking ID: ");
    if (bookings.checkIn(bookingId)) {
        cout << "Guest checked in." << endl;
    }
    else {
        cout << bookings.getLastError() << endl;
    }
}

void checkOutMenu(BookingManager& bookings, GuestManager& guests, BillingManager& billing) {
    int bookingId = readInt("Booking ID: ");
    if (!bookings.checkOut(bookingId)) {
        cout << bookings.getLastError() << endl;
        return;
    }
    cout << "Guest checked out. The room is free again." << endl;

    Booking* b = bookings.findBooking(bookingId);
    double price = b->getPricePerNight();
    Guest* g = guests.findGuest(b->getGuestId());
    if (g != nullptr && g->getDiscountRate() > 0) {
        price = roundMoney(price * (1 - g->getDiscountRate()));
        cout << "Discount applied (" << g->getGuestType() << " guest). New price per night: " << price << endl;
    }

    int billId = billing.generateBill(b->getBookingId(), b->getGuestId(), price, b->getNights());
    if (billId == -1) {
        cout << billing.getLastError() << endl;
        return;
    }
    cout << "Bill created. Bill ID: " << billId << endl;
    cout << billing.getReceipt(billId) << endl;
}

void cancelBookingMenu(BookingManager& bookings) {
    int bookingId = readInt("Booking ID: ");
    if (bookings.cancelBooking(bookingId)) {
        cout << "Booking cancelled. The room is free again." << endl;
    }
    else {
        cout << bookings.getLastError() << endl;
    }
}

void bookingsByGuestMenu(BookingManager& bookings) {
    int guestId = readInt("Guest ID: ");
    printBookings(bookings.getBookingsByGuest(guestId));
}

void bookingsByStatusMenu(BookingManager& bookings) {
    int s = readInt("1 = Confirmed, 2 = CheckedIn, 3 = CheckedOut, 4 = Cancelled: ");
    BookingStatus status = BookingStatus::Confirmed;
    if (s == 1) {
        status = BookingStatus::Confirmed;
    }
    else if (s == 2) {
        status = BookingStatus::CheckedIn;
    }
    else if (s == 3) {
        status = BookingStatus::CheckedOut;
    }
    else if (s == 4) {
        status = BookingStatus::Cancelled;
    }
    else {
        cout << "Invalid status." << endl;
        return;
    }
    printBookings(bookings.getBookingsByStatus(status));
}

void bookingsMenu(BookingManager& bookings, RoomManager& rooms, GuestManager& guests, BillingManager& billing) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "--- BOOKINGS ---" << endl;
        cout << "1. Create a booking" << endl;
        cout << "2. Check in" << endl;
        cout << "3. Check out (creates the bill)" << endl;
        cout << "4. Cancel a booking" << endl;
        cout << "5. Show all bookings" << endl;
        cout << "6. Show bookings of a guest" << endl;
        cout << "7. Show bookings by status" << endl;
        cout << "0. Back" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            createBookingMenu(bookings, rooms);
        }
        else if (choice == 2) {
            checkInMenu(bookings);
        }
        else if (choice == 3) {
            checkOutMenu(bookings, guests, billing);
        }
        else if (choice == 4) {
            cancelBookingMenu(bookings);
        }
        else if (choice == 5) {
            printBookings(bookings.getAllBookings());
        }
        else if (choice == 6) {
            bookingsByGuestMenu(bookings);
        }
        else if (choice == 7) {
            bookingsByStatusMenu(bookings);
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

void addServiceMenu(BillingManager& billing) {
    int billId = readInt("Bill ID: ");
    string name = readText("Service name: ");
    double price = readDouble("Service price: ");
    if (billing.addServiceToBill(billId, ServiceCharge(name, price))) {
        cout << "Service added." << endl;
    }
    else {
        cout << billing.getLastError() << endl;
    }
}

void payBillMenu(BillingManager& billing) {
    int billId = readInt("Bill ID: ");
    Bill* b = billing.findBill(billId);
    if (b == nullptr) {
        cout << billing.getLastError() << endl;
        return;
    }
    if (b->isPaid()) {
        cout << "Bill is already paid." << endl;
        return;
    }
    CashPayment cashInfo(0);
    CardPayment cardInfo(0);
    cout << "Due with cash: " << billing.getAmountDue(billId, cashInfo) << endl;
    cout << "Due with card: " << billing.getAmountDue(billId, cardInfo) << endl;

    int method = readInt("1 = Cash, 2 = Card: ");
    if (method != 1 && method != 2) {
        cout << "Invalid method." << endl;
        return;
    }
    double given = readDouble("Amount given: ");
    bool ok = false;
    if (method == 1) {
        CashPayment payment(given);
        ok = billing.recordPayment(billId, payment);
    }
    else {
        CardPayment payment(given);
        ok = billing.recordPayment(billId, payment);
    }
    if (ok) {
        cout << "Payment accepted." << endl;
        cout << billing.getReceipt(billId) << endl;
    }
    else {
        cout << billing.getLastError() << endl;
    }
}

void showReceiptMenu(BillingManager& billing) {
    int billId = readInt("Bill ID: ");
    string receipt = billing.getReceipt(billId);
    if (receipt == "") {
        cout << "Bill not found." << endl;
    }
    else {
        cout << receipt << endl;
    }
}

void findBillByBookingMenu(BillingManager& billing) {
    int bookingId = readInt("Booking ID: ");
    Bill* b = billing.findBillByBooking(bookingId);
    if (b == nullptr) {
        cout << billing.getLastError() << endl;
    }
    else {
        cout << *b << endl;
    }
}

void showAllBills(BillingManager& billing) {
    vector<Bill> all = billing.getAllBills();
    if (all.empty()) {
        cout << "No bills yet." << endl;
        return;
    }
    for (size_t i = 0; i < all.size(); i++) {
        cout << all[i] << endl;
    }
}

void billingMenu(BillingManager& billing) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "--- BILLING ---" << endl;
        cout << "1. Add a service to a bill" << endl;
        cout << "2. Pay a bill" << endl;
        cout << "3. Show a receipt" << endl;
        cout << "4. Find a bill by booking ID" << endl;
        cout << "5. Show all bills" << endl;
        cout << "0. Back" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            addServiceMenu(billing);
        }
        else if (choice == 2) {
            payBillMenu(billing);
        }
        else if (choice == 3) {
            showReceiptMenu(billing);
        }
        else if (choice == 4) {
            findBillByBookingMenu(billing);
        }
        else if (choice == 5) {
            showAllBills(billing);
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

void addStaffMenu(StaffManager& staff) {
    int type = readInt("Role (1 = Manager, 2 = Receptionist, 3 = Housekeeper): ");
    if (type < 1 || type > 3) {
        cout << "Invalid role." << endl;
        return;
    }
    int id = readInt("Staff ID: ");
    string name = readText("Name: ");
    string phone = readText("Phone: ");
    double salary = readDouble("Base salary: ");
    string password = readText("Password: ");
    Staff* s = nullptr;
    if (type == 1) {
        s = new Manager(id, name, phone, salary, password);
    }
    else if (type == 2) {
        s = new Receptionist(id, name, phone, salary, password);
    }
    else {
        s = new Housekeeper(id, name, phone, salary, password);
    }
    if (staff.addStaff(s)) {
        cout << "Staff added." << endl;
    }
    else {
        cout << staff.getLastError() << endl;
        delete s;
    }
}

void removeStaffMenu(StaffManager& staff, const Staff& user) {
    int id = readInt("Staff ID to remove: ");
    if (id == user.getId()) {
        cout << "You cannot remove your own account." << endl;
        return;
    }
    if (staff.removeStaff(id)) {
        cout << "Staff removed." << endl;
    }
    else {
        cout << staff.getLastError() << endl;
    }
}

void staffByRoleMenu(StaffManager& staff) {
    int r = readInt("1 = Manager, 2 = Receptionist, 3 = Housekeeper: ");
    string role = "";
    if (r == 1) {
        role = "Manager";
    }
    else if (r == 2) {
        role = "Receptionist";
    }
    else if (r == 3) {
        role = "Housekeeper";
    }
    else {
        cout << "Invalid role." << endl;
        return;
    }
    printStaff(staff.getStaffByRole(role));
}

void staffMenu(const Staff& user, StaffManager& staff) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "--- STAFF ---" << endl;
        cout << "1. Show all staff" << endl;
        cout << "2. Add staff" << endl;
        cout << "3. Remove staff" << endl;
        cout << "4. Show staff by role" << endl;
        cout << "5. Total salary expense" << endl;
        cout << "0. Back" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            printStaff(staff.getAllStaff());
        }
        else if (choice == 2) {
            addStaffMenu(staff);
        }
        else if (choice == 3) {
            removeStaffMenu(staff, user);
        }
        else if (choice == 4) {
            staffByRoleMenu(staff);
        }
        else if (choice == 5) {
            cout << "Total salary expense: " << staff.getTotalSalaryExpense() << endl;
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

void mainMenu(const Staff& user, RoomManager& rooms, StaffManager& staff, GuestManager& guests,
    BookingManager& bookings, BillingManager& billing) {
    int choice = -1;
    while (choice != 0) {
        cout << endl << "===== HOTEL MENU (" << user.getRole() << ": " << user.getName() << ") =====" << endl;
        cout << "1. Rooms" << endl;
        if (user.canHandleBookings()) {
            cout << "2. Guests" << endl;
            cout << "3. Bookings" << endl;
            cout << "4. Billing" << endl;
        }
        if (user.canManageStaff()) {
            cout << "5. Staff" << endl;
        }
        cout << "0. Logout" << endl;
        choice = readInt("Choose: ");

        if (choice == 1) {
            roomsMenu(user, rooms);
        }
        else if (choice == 2 && user.canHandleBookings()) {
            guestsMenu(user, guests, bookings);
        }
        else if (choice == 3 && user.canHandleBookings()) {
            bookingsMenu(bookings, rooms, guests, billing);
        }
        else if (choice == 4 && user.canHandleBookings()) {
            billingMenu(billing);
        }
        else if (choice == 5 && user.canManageStaff()) {
            staffMenu(user, staff);
        }
        else if (choice != 0) {
            cout << "Invalid choice." << endl;
        }
    }
}

int main() {
    cout << fixed << setprecision(2);

    RoomManager rooms;
    StaffManager staff;
    GuestManager guests;
    BookingManager bookings(rooms, guests);
    BillingManager billing;
    loadDemoData(rooms, staff, guests);

    cout << "===== HOTEL MANAGEMENT SYSTEM =====" << endl;
    bool running = true;
    while (running) {
        Staff* user = loginScreen(staff);
        if (user == nullptr) {
            running = false;
        }
        else {
            cout << "Welcome " << user->getName() << " (" << user->getRole() << ")" << endl;
            mainMenu(*user, rooms, staff, guests, bookings, billing);
        }
    }

    cout << "Goodbye." << endl;
    return 0;
}