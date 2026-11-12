#include <SFML/Graphics.hpp>
#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>
#include "Room.h"
#include "Staff.h"
#include "Guest.h"
#include "Booking.h"
#include "Billing.h"

enum Screen {
    SCREEN_LOGIN, SCREEN_DASHBOARD, SCREEN_ROOMS,
    SCREEN_GUESTS, SCREEN_BOOKINGS, SCREEN_BILLING, SCREEN_STAFF
};

enum FieldKind { FIELD_TEXT, FIELD_INT, FIELD_DECIMAL, FIELD_PASSWORD, FIELD_PHONE, FIELD_CNIC };

enum ButtonId {
    BTN_NONE, BTN_LOGIN, BTN_ROOMS, BTN_GUESTS, BTN_BOOKINGS, BTN_BILLING, BTN_STAFF,
    BTN_LOGOUT, BTN_BACK,
    RM_ALL, RM_AVAILABLE, RM_SINGLE, RM_DOUBLE, RM_SUITE, RM_FIND,
    RM_ADD_SINGLE, RM_ADD_DOUBLE, RM_ADD_SUITE, RM_REMOVE,
    RM_CLEANING_LIST, RM_START_CLEANING, RM_FINISH_CLEANING,
    GS_ALL, GS_FIND, GS_SEARCH, GS_REGISTER, GS_REGISTER_VIP, GS_PHONE, GS_REMOVE,
    BK_CREATE, BK_CHECKIN, BK_CHECKOUT, BK_CANCEL, BK_ALL, BK_CONFIRMED,
    BK_CHECKEDIN, BK_CHECKEDOUT, BK_CANCELLED, BK_BY_GUEST, BK_ROOMS,
    BL_ALL, BL_RECEIPT, BL_BY_BOOKING, BL_ADD_SERVICE, BL_DUE, BL_PAY_CASH, BL_PAY_CARD,
    ST_ALL, ST_SALARY, ST_ADD_MANAGER, ST_ADD_RECEPTIONIST, ST_ADD_HOUSEKEEPER, ST_REMOVE
};

struct Button {
    sf::FloatRect area;
    std::string label;
    int id;
};

struct Field {
    std::string label;
    std::string value;
    int kind;
};

struct Line {
    std::string text;
    sf::Color color;
};

struct Column {
    std::string title;
    float width;
};

struct Row {
    std::vector<std::string> cells;
    sf::Color color;
    int id = 0;
};

struct Ui {
    int screen = SCREEN_LOGIN;
    Staff* user = nullptr;
    std::vector<Field> fields;
    int activeField = 0;
    std::vector<Line> lines;
    int scroll = 0;
    std::string message = "";
    bool messageIsError = false;
    std::vector<int> cleaning;
    std::vector<Column> columns;
    std::vector<Row> rows;
    int selectedRow = -1;
    bool tableMode = false;
};

const int VISIBLE_LINES = 23;
const int TABLE_ROWS = 15;
const float ROW_HEIGHT = 32.f;
const float TABLE_TOP = 130.f;

const sf::Color COLOR_BG(236, 240, 245);
const sf::Color COLOR_HEADER(31, 58, 95);
const sf::Color COLOR_BUTTON(41, 98, 168);
const sf::Color COLOR_BUTTON_HOVER(60, 125, 200);
const sf::Color COLOR_SECONDARY(110, 120, 135);
const sf::Color COLOR_TEXT(30, 35, 45);
const sf::Color COLOR_MUTED(110, 120, 135);
const sf::Color COLOR_ERROR(190, 40, 40);
const sf::Color COLOR_GOOD(30, 130, 60);
const sf::Color COLOR_WARN(200, 120, 20);

int toInt(const std::string& text) {
    std::istringstream in(text);
    int value = 0;
    in >> value;
    return value;
}

double toDouble(const std::string& text) {
    std::istringstream in(text);
    double value = 0;
    in >> value;
    return value;
}

double roundMoney(double value) {
    return (int)(value * 100 + 0.5) / 100.0;
}

std::string money(double value) {
    std::ostringstream out;
    out << std::fixed << std::setprecision(2) << value;
    return out.str();
}

void setMessage(Ui& ui, const std::string& text, bool isError) {
    ui.message = text;
    ui.messageIsError = isError;
}

void addLine(Ui& ui, const std::string& text, sf::Color color) {
    Line line;
    line.text = text;
    line.color = color;
    ui.lines.push_back(line);
}

void clearList(Ui& ui) {
    ui.lines.clear();
    ui.rows.clear();
    ui.columns.clear();
    ui.selectedRow = -1;
    ui.tableMode = false;
    ui.scroll = 0;
}

void clampScroll(Ui& ui) {
    int total = ui.tableMode ? static_cast<int>(ui.rows.size()) : static_cast<int>(ui.lines.size());
    int visible = ui.tableMode ? TABLE_ROWS : VISIBLE_LINES;
    int maxScroll = total - visible;
    if (maxScroll < 0) {
        maxScroll = 0;
    }
    if (ui.scroll > maxScroll) {
        ui.scroll = maxScroll;
    }
    if (ui.scroll < 0) {
        ui.scroll = 0;
    }
}

void selectRow(Ui& ui, int index) {
    if (index < 0 || index >= static_cast<int>(ui.rows.size()) || ui.fields.empty()) {
        return;
    }
    if (ui.selectedRow == index) {
        ui.selectedRow = -1;
        ui.fields[0].value = "";
    }
    else {
        ui.selectedRow = index;
        ui.fields[0].value = std::to_string(ui.rows[index].id);
    }
}

void showText(Ui& ui, const std::string& text) {
    clearList(ui);
    std::istringstream in(text);
    std::string line;
    while (std::getline(in, line)) {
        addLine(ui, line, COLOR_TEXT);
    }
}

void addField(Ui& ui, const std::string& label, int kind) {
    Field f;
    f.label = label;
    f.value = "";
    f.kind = kind;
    ui.fields.push_back(f);
}

void clearFields(Ui& ui) {
    for (size_t i = 0; i < ui.fields.size(); i++) {
        ui.fields[i].value = "";
    }
}

void enterScreen(Ui& ui, int screen) {
    ui.screen = screen;
    ui.fields.clear();
    clearList(ui);
    ui.activeField = 0;
    ui.message = "";
    ui.messageIsError = false;
    if (screen == SCREEN_LOGIN) {
        addField(ui, "Staff ID", FIELD_INT);
        addField(ui, "Password", FIELD_PASSWORD);
    }
    else if (screen == SCREEN_ROOMS) {
        addField(ui, "Room number", FIELD_INT);
        addField(ui, "Base price", FIELD_DECIMAL);
    }
    else if (screen == SCREEN_GUESTS) {
        addField(ui, "Guest ID", FIELD_INT);
        addField(ui, "Name", FIELD_TEXT);
        addField(ui, "Phone", FIELD_PHONE);
        addField(ui, "CNIC", FIELD_CNIC);
    }
    else if (screen == SCREEN_BOOKINGS) {
        addField(ui, "Booking ID", FIELD_INT);
        addField(ui, "Guest ID", FIELD_INT);
        addField(ui, "Room number", FIELD_INT);
        addField(ui, "Check-in day (1 to 365)", FIELD_INT);
        addField(ui, "Nights", FIELD_INT);
    }
    else if (screen == SCREEN_BILLING) {
        addField(ui, "Bill ID", FIELD_INT);
        addField(ui, "Booking ID", FIELD_INT);
        addField(ui, "Service name", FIELD_TEXT);
        addField(ui, "Service price", FIELD_DECIMAL);
        addField(ui, "Amount given", FIELD_DECIMAL);
    }
    else if (screen == SCREEN_STAFF) {
        addField(ui, "Staff ID", FIELD_INT);
        addField(ui, "Name", FIELD_TEXT);
        addField(ui, "Phone", FIELD_PHONE);
        addField(ui, "Base salary", FIELD_DECIMAL);
        addField(ui, "Password", FIELD_PASSWORD);
    }
}

void typeChar(Field& f, char32_t c) {
    if (c == 8) {
        if (!f.value.empty()) {
            f.value.pop_back();
        }
        return;
    }
    if (c < 32 || c >= 127) {
        return;
    }
    size_t maxLength = 30;
    if (f.kind == FIELD_INT) {
        maxLength = 9;
    }
    else if (f.kind == FIELD_DECIMAL) {
        maxLength = 12;
    }
    else if (f.kind == FIELD_PASSWORD) {
        maxLength = 20;
    }
    else if (f.kind == FIELD_PHONE) {
        maxLength = 12;
    }
    else if (f.kind == FIELD_CNIC) {
        maxLength = 15;
    }
    if (f.value.size() >= maxLength) {
        return;
    }
    if (f.kind == FIELD_INT && !(c >= '0' && c <= '9')) {
        return;
    }
    if ((f.kind == FIELD_PHONE || f.kind == FIELD_CNIC) && !((c >= '0' && c <= '9') || c == '-')) {
        return;
    }
    if (f.kind == FIELD_DECIMAL) {
        bool isDigit = (c >= '0' && c <= '9');
        bool isNewDot = (c == '.' && f.value.find('.') == std::string::npos);
        if (!isDigit && !isNewDot) {
            return;
        }
    }
    f.value += static_cast<char>(c);
}

bool isCleaning(const Ui& ui, int roomNumber) {
    for (size_t i = 0; i < ui.cleaning.size(); i++) {
        if (ui.cleaning[i] == roomNumber) {
            return true;
        }
    }
    return false;
}

void removeCleaning(Ui& ui, int roomNumber) {
    for (size_t i = 0; i < ui.cleaning.size(); i++) {
        if (ui.cleaning[i] == roomNumber) {
            ui.cleaning.erase(ui.cleaning.begin() + i);
            return;
        }
    }
}

std::string roomText(const Room& r, const Ui& ui) {
    std::string status = "Booked";
    if (r.isAvailable()) {
        status = "Available";
    }
    else if (isCleaning(ui, r.getRoomNumber())) {
        status = "Cleaning";
    }
    std::string guestWord = (r.getCapacity() == 1) ? " guest" : " guests";
    return "Room " + std::to_string(r.getRoomNumber()) + " | " + r.getType() + " | " +
        money(r.getPricePerNight()) + " | " + std::to_string(r.getCapacity()) + guestWord +
        " | " + r.getFacilities() + " | " + status;
}

sf::Color roomColor(const Room& r, const Ui& ui) {
    if (r.isAvailable()) {
        return COLOR_GOOD;
    }
    if (isCleaning(ui, r.getRoomNumber())) {
        return COLOR_WARN;
    }
    return COLOR_ERROR;
}

std::string bookingText(const Booking& b) {
    return "#" + std::to_string(b.getBookingId()) + " | Guest " + std::to_string(b.getGuestId()) +
        " | Room " + std::to_string(b.getRoomNumber()) +
        " | Day " + std::to_string(b.getCheckInDay()) + " to " + std::to_string(b.getCheckOutDay()) +
        " (" + std::to_string(b.getNights()) + " nights) | " + money(b.getPricePerNight()) +
        "/night | " + statusToString(b.getStatus());
}

sf::Color bookingColor(const Booking& b) {
    if (b.getStatus() == BookingStatus::Confirmed) {
        return COLOR_BUTTON;
    }
    if (b.getStatus() == BookingStatus::CheckedIn) {
        return COLOR_WARN;
    }
    if (b.getStatus() == BookingStatus::CheckedOut) {
        return COLOR_GOOD;
    }
    return COLOR_MUTED;
}

std::string billText(const Bill& b) {
    return "Bill #" + std::to_string(b.getBillId()) + " | Booking " + std::to_string(b.getBookingId()) +
        " | Guest " + std::to_string(b.getGuestId()) + " | Total " + money(b.getTotal()) +
        " | " + (b.isPaid() ? "PAID" : "NOT PAID");
}

void addColumn(Ui& ui, const std::string& title, float width) {
    Column c;
    c.title = title;
    c.width = width;
    ui.columns.push_back(c);
}

void startTable(Ui& ui) {
    clearList(ui);
    ui.tableMode = true;
}

Row makeRow(int id, sf::Color color) {
    Row row;
    row.id = id;
    row.color = color;
    return row;
}

sf::Color staffColor(const Staff& s) {
    if (s.getRole() == "Manager") {
        return COLOR_BUTTON;
    }
    if (s.getRole() == "Receptionist") {
        return COLOR_GOOD;
    }
    return COLOR_WARN;
}

void showRooms(Ui& ui, const std::vector<Room*>& list) {
    if (list.empty()) {
        clearList(ui);
        addLine(ui, "No rooms found.", COLOR_MUTED);
        return;
    }
    startTable(ui);
    addColumn(ui, "Room", 70.f);
    addColumn(ui, "Type", 70.f);
    addColumn(ui, "Price/night", 110.f);
    addColumn(ui, "Guests", 65.f);
    addColumn(ui, "Facilities", 280.f);
    addColumn(ui, "Status", 120.f);
    for (size_t i = 0; i < list.size(); i++) {
        const Room& r = *list[i];
        std::string status = "Booked";
        if (r.isAvailable()) {
            status = "Available";
        }
        else if (isCleaning(ui, r.getRoomNumber())) {
            status = "Cleaning";
        }
        Row row = makeRow(r.getRoomNumber(), roomColor(r, ui));
        row.cells.push_back(std::to_string(r.getRoomNumber()));
        row.cells.push_back(r.getType());
        row.cells.push_back(money(r.getPricePerNight()));
        row.cells.push_back(std::to_string(r.getCapacity()));
        row.cells.push_back(r.getFacilities());
        row.cells.push_back(status);
        ui.rows.push_back(row);
    }
}

void showGuests(Ui& ui, const std::vector<Guest*>& list) {
    if (list.empty()) {
        clearList(ui);
        addLine(ui, "No guests found.", COLOR_MUTED);
        return;
    }
    startTable(ui);
    addColumn(ui, "ID", 70.f);
    addColumn(ui, "Name", 200.f);
    addColumn(ui, "Phone", 140.f);
    addColumn(ui, "CNIC", 170.f);
    addColumn(ui, "Type", 110.f);
    for (size_t i = 0; i < list.size(); i++) {
        const Guest& g = *list[i];
        Row row = makeRow(g.getId(), g.getDiscountRate() > 0 ? COLOR_WARN : COLOR_SECONDARY);
        row.cells.push_back(std::to_string(g.getId()));
        row.cells.push_back(g.getName());
        row.cells.push_back(g.getPhone());
        row.cells.push_back(g.getCnic());
        row.cells.push_back(g.getGuestType());
        ui.rows.push_back(row);
    }
}

void showStaff(Ui& ui, const std::vector<Staff*>& list) {
    if (list.empty()) {
        clearList(ui);
        addLine(ui, "No staff found.", COLOR_MUTED);
        return;
    }
    startTable(ui);
    addColumn(ui, "ID", 60.f);
    addColumn(ui, "Name", 170.f);
    addColumn(ui, "Phone", 140.f);
    addColumn(ui, "Salary", 130.f);
    addColumn(ui, "Role", 150.f);
    for (size_t i = 0; i < list.size(); i++) {
        const Staff& s = *list[i];
        Row row = makeRow(s.getId(), staffColor(s));
        row.cells.push_back(std::to_string(s.getId()));
        row.cells.push_back(s.getName());
        row.cells.push_back(s.getPhone());
        row.cells.push_back(money(s.calculateSalary()));
        row.cells.push_back(s.getRole());
        ui.rows.push_back(row);
    }
}

void showBookings(Ui& ui, const std::vector<Booking>& list) {
    if (list.empty()) {
        clearList(ui);
        addLine(ui, "No bookings found.", COLOR_MUTED);
        return;
    }
    startTable(ui);
    addColumn(ui, "ID", 50.f);
    addColumn(ui, "Guest", 70.f);
    addColumn(ui, "Room", 65.f);
    addColumn(ui, "Check-in", 90.f);
    addColumn(ui, "Check-out", 100.f);
    addColumn(ui, "Nights", 70.f);
    addColumn(ui, "Per night", 110.f);
    addColumn(ui, "Status", 110.f);
    for (size_t i = 0; i < list.size(); i++) {
        const Booking& b = list[i];
        Row row = makeRow(b.getBookingId(), bookingColor(b));
        row.cells.push_back(std::to_string(b.getBookingId()));
        row.cells.push_back(std::to_string(b.getGuestId()));
        row.cells.push_back(std::to_string(b.getRoomNumber()));
        row.cells.push_back("Day " + std::to_string(b.getCheckInDay()));
        row.cells.push_back("Day " + std::to_string(b.getCheckOutDay()));
        row.cells.push_back(std::to_string(b.getNights()));
        row.cells.push_back(money(b.getPricePerNight()));
        row.cells.push_back(statusToString(b.getStatus()));
        ui.rows.push_back(row);
    }
}

void showBills(Ui& ui, const std::vector<Bill>& list) {
    if (list.empty()) {
        clearList(ui);
        addLine(ui, "No bills yet.", COLOR_MUTED);
        return;
    }
    startTable(ui);
    addColumn(ui, "Bill", 60.f);
    addColumn(ui, "Booking", 90.f);
    addColumn(ui, "Guest", 75.f);
    addColumn(ui, "Total", 130.f);
    addColumn(ui, "Method", 110.f);
    addColumn(ui, "Status", 130.f);
    for (size_t i = 0; i < list.size(); i++) {
        const Bill& b = list[i];
        Row row = makeRow(b.getBillId(), b.isPaid() ? COLOR_GOOD : COLOR_ERROR);
        row.cells.push_back(std::to_string(b.getBillId()));
        row.cells.push_back(std::to_string(b.getBookingId()));
        row.cells.push_back(std::to_string(b.getGuestId()));
        row.cells.push_back(money(b.getTotal()));
        row.cells.push_back(b.getPaymentMethod() == "" ? "-" : b.getPaymentMethod());
        row.cells.push_back(b.isPaid() ? "Paid" : "Not paid");
        ui.rows.push_back(row);
    }
}

Room* makeRoom(int type, int number, double price) {
    if (type == RM_ADD_SINGLE) {
        return new SingleRoom(number, price);
    }
    if (type == RM_ADD_DOUBLE) {
        return new DoubleRoom(number, price);
    }
    return new Suite(number, price);
}

void roomsAction(Ui& ui, int action, RoomManager& rooms) {
    int number = toInt(ui.fields[0].value);
    double price = toDouble(ui.fields[1].value);

    if (action == RM_ALL) {
        showRooms(ui, rooms.getAllRooms());
    }
    else if (action == RM_AVAILABLE) {
        showRooms(ui, rooms.getAvailableRooms());
        setMessage(ui, "Available: " + std::to_string(rooms.getAvailableCount()) +
            " of " + std::to_string(rooms.getTotalRoomCount()), false);
    }
    else if (action == RM_SINGLE) {
        showRooms(ui, rooms.getRoomsByType("Single"));
    }
    else if (action == RM_DOUBLE) {
        showRooms(ui, rooms.getRoomsByType("Double"));
    }
    else if (action == RM_SUITE) {
        showRooms(ui, rooms.getRoomsByType("Suite"));
    }
    else if (action == RM_FIND) {
        Room* r = rooms.findRoom(number);
        if (r == nullptr) {
            setMessage(ui, rooms.getLastError(), true);
        }
        else {
            std::vector<Room*> one;
            one.push_back(r);
            showRooms(ui, one);
            setMessage(ui, "Room found.", false);
        }
    }
    else if (action == RM_CLEANING_LIST) {
        std::vector<Room*> list;
        for (size_t i = 0; i < ui.cleaning.size(); i++) {
            Room* r = rooms.findRoom(ui.cleaning[i]);
            if (r != nullptr) {
                list.push_back(r);
            }
        }
        showRooms(ui, list);
    }
    else if (action == RM_START_CLEANING) {
        Room* r = rooms.findRoom(number);
        if (r == nullptr) {
            setMessage(ui, rooms.getLastError(), true);
        }
        else if (isCleaning(ui, number)) {
            setMessage(ui, "This room is already under cleaning.", true);
        }
        else if (!r->isAvailable()) {
            setMessage(ui, "This room is booked and cannot be cleaned now.", true);
        }
        else {
            r->setAvailable(false);
            ui.cleaning.push_back(number);
            setMessage(ui, "Room is now under cleaning.", false);
            showRooms(ui, rooms.getAllRooms());
        }
    }
    else if (action == RM_FINISH_CLEANING) {
        Room* r = rooms.findRoom(number);
        if (r == nullptr) {
            setMessage(ui, rooms.getLastError(), true);
        }
        else if (!isCleaning(ui, number)) {
            setMessage(ui, "This room is not under cleaning.", true);
        }
        else {
            removeCleaning(ui, number);
            r->setAvailable(true);
            setMessage(ui, "Room cleaned and available again.", false);
            showRooms(ui, rooms.getAllRooms());
        }
    }
    else if (action == RM_ADD_SINGLE || action == RM_ADD_DOUBLE || action == RM_ADD_SUITE) {
        if (number <= 0) {
            setMessage(ui, "Enter a room number.", true);
        }
        else if (price <= 0) {
            setMessage(ui, "Enter a base price greater than 0.", true);
        }
        else {
            Room* r = makeRoom(action, number, price);
            if (rooms.addRoom(r)) {
                setMessage(ui, "Room added.", false);
                showRooms(ui, rooms.getAllRooms());
                clearFields(ui);
            }
            else {
                setMessage(ui, rooms.getLastError(), true);
                delete r;
            }
        }
    }
    else if (action == RM_REMOVE) {
        if (rooms.removeRoom(number)) {
            setMessage(ui, "Room removed.", false);
            showRooms(ui, rooms.getAllRooms());
        }
        else {
            setMessage(ui, rooms.getLastError(), true);
        }
    }
}

bool hasActiveBooking(BookingManager& bookings, int guestId) {
    std::vector<Booking> list = bookings.getBookingsByGuest(guestId);
    for (size_t i = 0; i < list.size(); i++) {
        if (list[i].getStatus() == BookingStatus::Confirmed ||
            list[i].getStatus() == BookingStatus::CheckedIn) {
            return true;
        }
    }
    return false;
}

void guestsAction(Ui& ui, int action, GuestManager& guests, BookingManager& bookings) {
    int guestId = toInt(ui.fields[0].value);
    std::string name = ui.fields[1].value;
    std::string phone = ui.fields[2].value;
    std::string cnic = ui.fields[3].value;

    if (action == GS_ALL) {
        showGuests(ui, guests.getAllGuests());
    }
    else if (action == GS_FIND) {
        Guest* g = guests.findGuest(guestId);
        if (g == nullptr) {
            setMessage(ui, guests.getLastError(), true);
        }
        else {
            std::vector<Guest*> one;
            one.push_back(g);
            showGuests(ui, one);
            setMessage(ui, "Guest found.", false);
        }
    }
    else if (action == GS_SEARCH) {
        showGuests(ui, guests.searchByName(name));
    }
    else if (action == GS_REGISTER || action == GS_REGISTER_VIP) {
        if (guestId != 0) {
            setMessage(ui, "Leave Guest ID empty. A new ID is given automatically.", true);
            return;
        }
        if (name == "" || phone == "" || cnic == "") {
            setMessage(ui, "Fill in name, phone and CNIC.", true);
            return;
        }
        std::vector<Guest*> allGuests = guests.getAllGuests();
        for (size_t i = 0; i < allGuests.size(); i++) {
            if (allGuests[i]->getCnic() == cnic) {
                setMessage(ui, "A guest with this CNIC already exists.", true);
                return;
            }
        }
        int newId = GuestManager::generateGuestId();
        Guest* g = nullptr;
        if (action == GS_REGISTER_VIP) {
            g = new VIPGuest(newId, name, phone, cnic);
        }
        else {
            g = new Guest(newId, name, phone, cnic);
        }
        if (guests.addGuest(g)) {
            setMessage(ui, "Guest registered. Guest ID: " + std::to_string(newId), false);
            showGuests(ui, guests.getAllGuests());
            clearFields(ui);
        }
        else {
            setMessage(ui, guests.getLastError(), true);
            delete g;
        }
    }
    else if (action == GS_PHONE) {
        if (guests.updateGuestPhone(guestId, phone)) {
            setMessage(ui, "Phone updated.", false);
            showGuests(ui, guests.getAllGuests());
        }
        else {
            setMessage(ui, guests.getLastError(), true);
        }
    }
    else if (action == GS_REMOVE) {
        if (hasActiveBooking(bookings, guestId)) {
            setMessage(ui, "This guest has an active booking and cannot be removed.", true);
        }
        else if (guests.removeGuest(guestId)) {
            setMessage(ui, "Guest removed.", false);
            showGuests(ui, guests.getAllGuests());
        }
        else {
            setMessage(ui, guests.getLastError(), true);
        }
    }
}

void checkOutAndBill(Ui& ui, int bookingId, BookingManager& bookings, RoomManager& rooms,
    GuestManager& guests, BillingManager& billing) {
    if (!bookings.checkOut(bookingId)) {
        setMessage(ui, bookings.getLastError(), true);
        return;
    }
    Booking* b = bookings.findBooking(bookingId);
    Room* room = rooms.findRoom(b->getRoomNumber());
    if (room != nullptr) {
        room->setAvailable(false);
        ui.cleaning.push_back(b->getRoomNumber());
    }
    double price = b->getPricePerNight();
    std::string note = "";
    Guest* g = guests.findGuest(b->getGuestId());
    if (g != nullptr && g->getDiscountRate() > 0) {
        price = roundMoney(price * (1 - g->getDiscountRate()));
        note = " (" + g->getGuestType() + " discount applied)";
    }
    int billId = billing.generateBill(b->getBookingId(), b->getGuestId(), price, b->getNights());
    if (billId == -1) {
        setMessage(ui, billing.getLastError(), true);
        return;
    }
    showText(ui, billing.getReceipt(billId));
    setMessage(ui, "Checked out. Bill ID: " + std::to_string(billId) + note + ". Room is under cleaning.", false);
}

void bookingsAction(Ui& ui, int action, BookingManager& bookings, RoomManager& rooms,
    GuestManager& guests, BillingManager& billing) {
    int bookingId = toInt(ui.fields[0].value);
    int guestId = toInt(ui.fields[1].value);
    int roomNumber = toInt(ui.fields[2].value);
    int day = toInt(ui.fields[3].value);
    int nights = toInt(ui.fields[4].value);

    if (action == BK_CREATE) {
        int newId = bookings.createBooking(guestId, roomNumber, day, nights);
        if (newId == -1) {
            setMessage(ui, bookings.getLastError(), true);
        }
        else {
            Booking* b = bookings.findBooking(newId);
            setMessage(ui, "Booking created. ID: " + std::to_string(newId) +
                " | Estimated cost: " + money(b->getEstimatedCost()), false);
            showBookings(ui, bookings.getAllBookings());
        }
    }
    else if (action == BK_CHECKIN) {
        if (bookings.checkIn(bookingId)) {
            setMessage(ui, "Guest checked in.", false);
            showBookings(ui, bookings.getAllBookings());
        }
        else {
            setMessage(ui, bookings.getLastError(), true);
        }
    }
    else if (action == BK_CHECKOUT) {
        checkOutAndBill(ui, bookingId, bookings, rooms, guests, billing);
    }
    else if (action == BK_CANCEL) {
        if (bookings.cancelBooking(bookingId)) {
            setMessage(ui, "Booking cancelled. The room is free again.", false);
            showBookings(ui, bookings.getAllBookings());
        }
        else {
            setMessage(ui, bookings.getLastError(), true);
        }
    }
    else if (action == BK_ALL) {
        showBookings(ui, bookings.getAllBookings());
    }
    else if (action == BK_CONFIRMED) {
        showBookings(ui, bookings.getBookingsByStatus(BookingStatus::Confirmed));
    }
    else if (action == BK_CHECKEDIN) {
        showBookings(ui, bookings.getBookingsByStatus(BookingStatus::CheckedIn));
    }
    else if (action == BK_CHECKEDOUT) {
        showBookings(ui, bookings.getBookingsByStatus(BookingStatus::CheckedOut));
    }
    else if (action == BK_CANCELLED) {
        showBookings(ui, bookings.getBookingsByStatus(BookingStatus::Cancelled));
    }
    else if (action == BK_BY_GUEST) {
        showBookings(ui, bookings.getBookingsByGuest(guestId));
    }
    else if (action == BK_ROOMS) {
        showRooms(ui, rooms.getAvailableRooms());
        setMessage(ui, "These rooms can be booked.", false);
    }
}

void payBill(Ui& ui, BillingManager& billing, int billId, const Payment& payment) {
    if (billing.recordPayment(billId, payment)) {
        Bill* b = billing.findBill(billId);
        setMessage(ui, "Payment accepted. Change: " + money(b->getChangeGiven()), false);
        showText(ui, billing.getReceipt(billId));
    }
    else {
        setMessage(ui, billing.getLastError(), true);
    }
}

void billingAction(Ui& ui, int action, BillingManager& billing) {
    int billId = toInt(ui.fields[0].value);
    int bookingId = toInt(ui.fields[1].value);
    std::string serviceName = ui.fields[2].value;
    double servicePrice = toDouble(ui.fields[3].value);
    double given = toDouble(ui.fields[4].value);

    if (action == BL_ALL) {
        showBills(ui, billing.getAllBills());
    }
    else if (action == BL_RECEIPT) {
        std::string receipt = billing.getReceipt(billId);
        if (receipt == "") {
            setMessage(ui, "Bill not found.", true);
        }
        else {
            showText(ui, receipt);
            setMessage(ui, "Receipt shown.", false);
        }
    }
    else if (action == BL_BY_BOOKING) {
        Bill* b = billing.findBillByBooking(bookingId);
        if (b == nullptr) {
            setMessage(ui, billing.getLastError(), true);
        }
        else {
            std::vector<Bill> one;
            one.push_back(*b);
            showBills(ui, one);
            setMessage(ui, "Bill found. Bill ID: " + std::to_string(b->getBillId()), false);
        }
    }
    else if (action == BL_ADD_SERVICE) {
        if (serviceName == "") {
            setMessage(ui, "Enter a service name.", true);
        }
        else if (billing.addServiceToBill(billId, ServiceCharge(serviceName, servicePrice))) {
            setMessage(ui, "Service added.", false);
            showText(ui, billing.getReceipt(billId));
        }
        else {
            setMessage(ui, billing.getLastError(), true);
        }
    }
    else if (action == BL_DUE) {
        CashPayment cash(0);
        CardPayment card(0);
        double dueCash = billing.getAmountDue(billId, cash);
        double dueCard = billing.getAmountDue(billId, card);
        if (dueCash < 0) {
            setMessage(ui, "Bill not found.", true);
        }
        else {
            setMessage(ui, "Due with cash: " + money(dueCash) + "   |   Due with card: " + money(dueCard), false);
        }
    }
    else if (action == BL_PAY_CASH) {
        CashPayment payment(given);
        payBill(ui, billing, billId, payment);
    }
    else if (action == BL_PAY_CARD) {
        CardPayment payment(given);
        payBill(ui, billing, billId, payment);
    }
}

void staffAction(Ui& ui, int action, StaffManager& staff) {
    int staffId = toInt(ui.fields[0].value);
    std::string name = ui.fields[1].value;
    std::string phone = ui.fields[2].value;
    double salary = toDouble(ui.fields[3].value);
    std::string password = ui.fields[4].value;

    if (action == ST_ALL) {
        showStaff(ui, staff.getAllStaff());
    }
    else if (action == ST_SALARY) {
        setMessage(ui, "Total salary expense: " + money(staff.getTotalSalaryExpense()), false);
    }
    else if (action == ST_ADD_MANAGER || action == ST_ADD_RECEPTIONIST || action == ST_ADD_HOUSEKEEPER) {
        if (staffId <= 0 || name == "" || phone == "" || salary <= 0 || password == "") {
            setMessage(ui, "Fill in ID, name, phone, salary and password.", true);
            return;
        }
        Staff* s = nullptr;
        if (action == ST_ADD_MANAGER) {
            s = new Manager(staffId, name, phone, salary, password);
        }
        else if (action == ST_ADD_RECEPTIONIST) {
            s = new Receptionist(staffId, name, phone, salary, password);
        }
        else {
            s = new Housekeeper(staffId, name, phone, salary, password);
        }
        if (staff.addStaff(s)) {
            setMessage(ui, "Staff added.", false);
            showStaff(ui, staff.getAllStaff());
            clearFields(ui);
        }
        else {
            setMessage(ui, staff.getLastError(), true);
            delete s;
        }
    }
    else if (action == ST_REMOVE) {
        if (staffId == ui.user->getId()) {
            setMessage(ui, "You cannot remove your own account.", true);
        }
        else if (staff.removeStaff(staffId)) {
            setMessage(ui, "Staff removed.", false);
            showStaff(ui, staff.getAllStaff());
        }
        else {
            setMessage(ui, staff.getLastError(), true);
        }
    }
}

void handleButton(Ui& ui, int action, RoomManager& rooms, StaffManager& staff, GuestManager& guests,
    BookingManager& bookings, BillingManager& billing) {
    if (action == BTN_LOGIN) {
        std::string idText = ui.fields[0].value;
        std::string password = ui.fields[1].value;
        if (idText == "" || password == "") {
            setMessage(ui, "Enter your staff ID and password.", true);
            return;
        }
        Staff* s = staff.login(toInt(idText), password);
        if (s == nullptr) {
            setMessage(ui, staff.getLastError(), true);
            ui.fields[1].value = "";
        }
        else {
            ui.user = s;
            enterScreen(ui, SCREEN_DASHBOARD);
        }
    }
    else if (action == BTN_ROOMS) {
        enterScreen(ui, SCREEN_ROOMS);
        roomsAction(ui, RM_ALL, rooms);
    }
    else if (action == BTN_GUESTS) {
        enterScreen(ui, SCREEN_GUESTS);
        guestsAction(ui, GS_ALL, guests, bookings);
    }
    else if (action == BTN_BOOKINGS) {
        enterScreen(ui, SCREEN_BOOKINGS);
        bookingsAction(ui, BK_ALL, bookings, rooms, guests, billing);
    }
    else if (action == BTN_BILLING) {
        enterScreen(ui, SCREEN_BILLING);
        billingAction(ui, BL_ALL, billing);
    }
    else if (action == BTN_STAFF) {
        enterScreen(ui, SCREEN_STAFF);
        staffAction(ui, ST_ALL, staff);
    }
    else if (action == BTN_BACK) {
        enterScreen(ui, SCREEN_DASHBOARD);
    }
    else if (action == BTN_LOGOUT) {
        ui.user = nullptr;
        enterScreen(ui, SCREEN_LOGIN);
    }
    else if (ui.screen == SCREEN_ROOMS) {
        roomsAction(ui, action, rooms);
    }
    else if (ui.screen == SCREEN_GUESTS) {
        guestsAction(ui, action, guests, bookings);
    }
    else if (ui.screen == SCREEN_BOOKINGS) {
        bookingsAction(ui, action, bookings, rooms, guests, billing);
    }
    else if (ui.screen == SCREEN_BILLING) {
        billingAction(ui, action, billing);
    }
    else if (ui.screen == SCREEN_STAFF) {
        staffAction(ui, action, staff);
    }
}

Button makeButton(float x, float y, float w, float h, const std::string& label, int id) {
    Button b;
    b.area = sf::FloatRect({ x, y }, { w, h });
    b.label = label;
    b.id = id;
    return b;
}

void addColumnButton(std::vector<Button>& list, const std::string& label, int id) {
    float y = 90.f + 36.f * static_cast<float>(list.size());
    list.push_back(makeButton(310.f, y, 200.f, 32.f, label, id));
}

std::vector<Button> makeButtons(int screen, const Staff* user) {
    std::vector<Button> list;
    if (screen == SCREEN_LOGIN) {
        list.push_back(makeButton(500.f, 380.f, 300.f, 50.f, "Login", BTN_LOGIN));
        return list;
    }
    if (screen == SCREEN_DASHBOARD) {
        float y = 200.f;
        list.push_back(makeButton(500.f, y, 300.f, 55.f, "Rooms", BTN_ROOMS));
        y += 70.f;
        if (user->canHandleBookings()) {
            list.push_back(makeButton(500.f, y, 300.f, 55.f, "Guests", BTN_GUESTS));
            y += 70.f;
            list.push_back(makeButton(500.f, y, 300.f, 55.f, "Bookings", BTN_BOOKINGS));
            y += 70.f;
            list.push_back(makeButton(500.f, y, 300.f, 55.f, "Billing", BTN_BILLING));
            y += 70.f;
        }
        if (user->canManageStaff()) {
            list.push_back(makeButton(500.f, y, 300.f, 55.f, "Staff", BTN_STAFF));
        }
        list.push_back(makeButton(500.f, 565.f, 300.f, 50.f, "Logout", BTN_LOGOUT));
        return list;
    }
    if (screen == SCREEN_ROOMS) {
        addColumnButton(list, "Show all rooms", RM_ALL);
        addColumnButton(list, "Available rooms", RM_AVAILABLE);
        addColumnButton(list, "Single rooms", RM_SINGLE);
        addColumnButton(list, "Double rooms", RM_DOUBLE);
        addColumnButton(list, "Suites", RM_SUITE);
        addColumnButton(list, "Find room", RM_FIND);
        if (user->canManageRooms()) {
            addColumnButton(list, "Cleaning rooms", RM_CLEANING_LIST);
            addColumnButton(list, "Start cleaning", RM_START_CLEANING);
            addColumnButton(list, "Mark cleaned", RM_FINISH_CLEANING);
        }
        if (user->canManageStaff()) {
            addColumnButton(list, "Add single room", RM_ADD_SINGLE);
            addColumnButton(list, "Add double room", RM_ADD_DOUBLE);
            addColumnButton(list, "Add suite", RM_ADD_SUITE);
            addColumnButton(list, "Remove room", RM_REMOVE);
        }
    }
    else if (screen == SCREEN_GUESTS) {
        addColumnButton(list, "Show all guests", GS_ALL);
        addColumnButton(list, "Find by ID", GS_FIND);
        addColumnButton(list, "Search by name", GS_SEARCH);
        addColumnButton(list, "Register guest", GS_REGISTER);
        addColumnButton(list, "Register VIP guest", GS_REGISTER_VIP);
        addColumnButton(list, "Update phone", GS_PHONE);
        if (user->canManageStaff()) {
            addColumnButton(list, "Remove guest", GS_REMOVE);
        }
    }
    else if (screen == SCREEN_BOOKINGS) {
        addColumnButton(list, "Create booking", BK_CREATE);
        addColumnButton(list, "Check in", BK_CHECKIN);
        addColumnButton(list, "Check out + bill", BK_CHECKOUT);
        addColumnButton(list, "Cancel booking", BK_CANCEL);
        addColumnButton(list, "All bookings", BK_ALL);
        addColumnButton(list, "Confirmed", BK_CONFIRMED);
        addColumnButton(list, "Checked in", BK_CHECKEDIN);
        addColumnButton(list, "Checked out", BK_CHECKEDOUT);
        addColumnButton(list, "Cancelled", BK_CANCELLED);
        addColumnButton(list, "Bookings of guest", BK_BY_GUEST);
        addColumnButton(list, "Available rooms", BK_ROOMS);
    }
    else if (screen == SCREEN_BILLING) {
        addColumnButton(list, "All bills", BL_ALL);
        addColumnButton(list, "Show receipt", BL_RECEIPT);
        addColumnButton(list, "Find by booking ID", BL_BY_BOOKING);
        addColumnButton(list, "Add service", BL_ADD_SERVICE);
        addColumnButton(list, "Amount due", BL_DUE);
        addColumnButton(list, "Pay with cash", BL_PAY_CASH);
        addColumnButton(list, "Pay with card", BL_PAY_CARD);
    }
    else if (screen == SCREEN_STAFF) {
        addColumnButton(list, "Show all staff", ST_ALL);
        addColumnButton(list, "Total salary", ST_SALARY);
        addColumnButton(list, "Add manager", ST_ADD_MANAGER);
        addColumnButton(list, "Add receptionist", ST_ADD_RECEPTIONIST);
        addColumnButton(list, "Add housekeeper", ST_ADD_HOUSEKEEPER);
        addColumnButton(list, "Remove staff", ST_REMOVE);
    }
    list.push_back(makeButton(15.f, 590.f, 150.f, 42.f, "Back", BTN_BACK));
    return list;
}

sf::FloatRect fieldRect(int screen, int index) {
    float i = static_cast<float>(index);
    if (screen == SCREEN_LOGIN) {
        return sf::FloatRect({ 500.f, 205.f + 95.f * i }, { 300.f, 45.f });
    }
    return sf::FloatRect({ 15.f, 112.f + 68.f * i }, { 280.f, 40.f });
}

std::string screenTitle(int screen) {
    if (screen == SCREEN_ROOMS) return "Rooms";
    if (screen == SCREEN_GUESTS) return "Guests";
    if (screen == SCREEN_BOOKINGS) return "Bookings";
    if (screen == SCREEN_BILLING) return "Billing";
    if (screen == SCREEN_STAFF) return "Staff";
    return "";
}

void drawText(sf::RenderWindow& window, const sf::Font& font, const std::string& str,
    unsigned int size, float x, float y, sf::Color color) {
    sf::Text text(font, str, size);
    text.setFillColor(color);
    text.setPosition({ x, y });
    window.draw(text);
}

void drawButton(sf::RenderWindow& window, const sf::Font& font, const Button& b, bool hover) {
    bool secondary = (b.id == BTN_LOGOUT || b.id == BTN_BACK);
    sf::RectangleShape box(b.area.size);
    box.setPosition(b.area.position);
    if (secondary) {
        box.setFillColor(hover ? COLOR_TEXT : COLOR_SECONDARY);
    }
    else {
        box.setFillColor(hover ? COLOR_BUTTON_HOVER : COLOR_BUTTON);
    }
    window.draw(box);

    sf::Text text(font, b.label, 18);
    text.setFillColor(sf::Color::White);
    sf::FloatRect bounds = text.getLocalBounds();
    text.setPosition({ b.area.position.x + (b.area.size.x - bounds.size.x) / 2.f - bounds.position.x,
                      b.area.position.y + (b.area.size.y - bounds.size.y) / 2.f - bounds.position.y });
    window.draw(text);
}

void drawField(sf::RenderWindow& window, const sf::Font& font, const Field& f,
    const sf::FloatRect& area, bool active) {
    drawText(window, font, f.label, 16, area.position.x, area.position.y - 24.f, COLOR_MUTED);
    sf::RectangleShape box(area.size);
    box.setPosition(area.position);
    box.setFillColor(sf::Color::White);
    box.setOutlineThickness(2.f);
    box.setOutlineColor(active ? COLOR_BUTTON : sf::Color(180, 188, 200));
    window.draw(box);
    std::string shown = f.value;
    if (f.kind == FIELD_PASSWORD) {
        shown = std::string(f.value.size(), '*');
    }
    drawText(window, font, shown + (active ? "|" : ""), 20, area.position.x + 10.f, area.position.y + 7.f, COLOR_TEXT);
}

void drawHeader(sf::RenderWindow& window, const sf::Font& font, const Ui& ui) {
    sf::RectangleShape bar({ 1300.f, 70.f });
    bar.setFillColor(COLOR_HEADER);
    window.draw(bar);
    std::string title = "Hotel Management System";
    if (ui.screen != SCREEN_LOGIN && ui.screen != SCREEN_DASHBOARD) {
        title += "  -  " + screenTitle(ui.screen);
    }
    drawText(window, font, title, 28, 30.f, 17.f, sf::Color::White);
    if (ui.user != nullptr) {
        drawText(window, font, ui.user->getName() + " (" + ui.user->getRole() + ")", 20, 1000.f, 22.f, sf::Color::White);
    }
}

void drawList(sf::RenderWindow& window, const sf::Font& mono, const Ui& ui) {
    sf::RectangleShape panel({ 760.f, 540.f });
    panel.setPosition({ 530.f, 90.f });
    panel.setFillColor(sf::Color::White);
    window.draw(panel);
    for (int i = 0; i < VISIBLE_LINES; i++) {
        int index = ui.scroll + i;
        if (index >= static_cast<int>(ui.lines.size())) {
            break;
        }
        drawText(window, mono, ui.lines[index].text, 14, 540.f, 98.f + 22.f * static_cast<float>(i), ui.lines[index].color);
    }
    if (static_cast<int>(ui.lines.size()) > VISIBLE_LINES) {
        drawText(window, mono, "Scroll with the mouse wheel to see more lines.", 13, 540.f, 608.f, COLOR_MUTED);
    }
}

std::string fitText(const sf::Font& font, const std::string& str, unsigned int size, float maxWidth) {
    sf::Text probe(font, str, size);
    if (probe.getLocalBounds().size.x <= maxWidth) {
        return str;
    }
    std::string shown = str;
    while (shown.size() > 1) {
        shown.pop_back();
        probe.setString(shown + "..");
        if (probe.getLocalBounds().size.x <= maxWidth) {
            break;
        }
    }
    return shown + "..";
}

void drawTable(sf::RenderWindow& window, const sf::Font& font, const Ui& ui) {
    sf::RectangleShape panel({ 760.f, 540.f });
    panel.setPosition({ 530.f, 90.f });
    panel.setFillColor(sf::Color::White);
    window.draw(panel);

    sf::RectangleShape headerBar({ 760.f, 36.f });
    headerBar.setPosition({ 530.f, 90.f });
    headerBar.setFillColor(COLOR_HEADER);
    window.draw(headerBar);

    float x = 540.f;
    for (size_t c = 0; c < ui.columns.size(); c++) {
        drawText(window, font, ui.columns[c].title, 16, x + 6.f, 98.f, sf::Color::White);
        x += ui.columns[c].width;
    }

    for (int i = 0; i < TABLE_ROWS; i++) {
        int index = ui.scroll + i;
        if (index >= static_cast<int>(ui.rows.size())) {
            break;
        }
        const Row& row = ui.rows[index];
        float y = TABLE_TOP + ROW_HEIGHT * static_cast<float>(i);

        sf::RectangleShape stripe({ 760.f, ROW_HEIGHT });
        stripe.setPosition({ 530.f, y });
        if (index == ui.selectedRow) {
            stripe.setFillColor(sf::Color(200, 220, 245));
        }
        else if (index % 2 == 1) {
            stripe.setFillColor(sf::Color(244, 246, 250));
        }
        else {
            stripe.setFillColor(sf::Color::White);
        }
        window.draw(stripe);

        float cx = 540.f;
        for (size_t c = 0; c < row.cells.size() && c < ui.columns.size(); c++) {
            float w = ui.columns[c].width;
            bool isBadge = (c + 1 == ui.columns.size());
            if (isBadge) {
                sf::RectangleShape badge({ w - 10.f, 24.f });
                badge.setPosition({ cx, y + 4.f });
                badge.setFillColor(row.color);
                window.draw(badge);
                drawText(window, font, fitText(font, row.cells[c], 14, w - 26.f), 14, cx + 8.f, y + 8.f, sf::Color::White);
            }
            else {
                drawText(window, font, fitText(font, row.cells[c], 15, w - 12.f), 15, cx + 6.f, y + 7.f, COLOR_TEXT);
            }
            cx += w;
        }
    }
    drawText(window, font, "Click a row to select it. Mouse wheel scrolls.", 13, 540.f, 612.f, COLOR_MUTED);
}

void finishCleaning(Ui& ui, RoomManager& rooms, int roomNumber) {
    Room* r = rooms.findRoom(roomNumber);
    if (r != nullptr) {
        removeCleaning(ui, roomNumber);
        r->setAvailable(true);
    }
}

int demoStay(Ui& ui, BookingManager& bookings, RoomManager& rooms, GuestManager& guests,
    BillingManager& billing, int guestId, int roomNumber, int day, int nights) {
    int bookingId = bookings.createBooking(guestId, roomNumber, day, nights);
    if (bookingId == -1) {
        return -1;
    }
    bookings.checkIn(bookingId);
    checkOutAndBill(ui, bookingId, bookings, rooms, guests, billing);
    Bill* b = billing.findBillByBooking(bookingId);
    if (b == nullptr) {
        return -1;
    }
    return b->getBillId();
}

void loadDemoData(Ui& ui, RoomManager& rooms, StaffManager& staff, GuestManager& guests,
    BookingManager& bookings, BillingManager& billing) {
    rooms.loadDefaultRooms();
    rooms.addRoom(new SingleRoom(105, 4500));
    rooms.addRoom(new DoubleRoom(205, 6000));
    rooms.addRoom(new Suite(303, 8000));

    staff.addStaff(new Manager(1, "Ali", "0300-1111111", 100000, "admin123"));
    staff.addStaff(new Receptionist(2, "Sara", "0300-2222222", 40000, "rec123"));
    staff.addStaff(new Housekeeper(3, "Bilal", "0300-4444444", 30000, "house123"));
    staff.addStaff(new Receptionist(4, "Hina", "0300-3333333", 42000, "hina123"));
    staff.addStaff(new Housekeeper(5, "Imran", "0300-5555555", 32000, "imran123"));
    staff.addStaff(new Housekeeper(6, "Nadia", "0300-6666666", 31000, "nadia123"));

    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Ahmed Khan", "0301-5555555", "35202-1234567-1"));
    guests.addGuest(new VIPGuest(GuestManager::generateGuestId(), "Zara Malik", "0302-6666666", "35202-7654321-2"));
    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Bilal Ahmed", "0303-1234567", "35201-1111111-1"));
    guests.addGuest(new VIPGuest(GuestManager::generateGuestId(), "Ayesha Siddiqui", "0304-2345678", "35201-2222222-2"));
    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Usman Tariq", "0305-3456789", "35201-3333333-3"));
    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Maryam Fatima", "0306-4567890", "35201-4444444-4"));
    guests.addGuest(new VIPGuest(GuestManager::generateGuestId(), "Hamza Sheikh", "0307-5678901", "35201-5555555-5"));
    guests.addGuest(new Guest(GuestManager::generateGuestId(), "Noor Ali", "0308-6789012", "35201-6666666-6"));

    int bill1 = demoStay(ui, bookings, rooms, guests, billing, 1001, 101, 5, 3);
    billing.addServiceToBill(bill1, ServiceCharge("Laundry", 500));
    billing.addServiceToBill(bill1, ServiceCharge("Food", 1200));
    CashPayment cash1(20000);
    billing.recordPayment(bill1, cash1);
    finishCleaning(ui, rooms, 101);

    int bill2 = demoStay(ui, bookings, rooms, guests, billing, 1002, 301, 10, 4);
    billing.addServiceToBill(bill2, ServiceCharge("Spa", 3000));
    CardPayment card2(54000);
    billing.recordPayment(bill2, card2);

    demoStay(ui, bookings, rooms, guests, billing, 1003, 201, 20, 2);

    int stay = bookings.createBooking(1004, 302, 30, 3);
    bookings.checkIn(stay);

    bookings.createBooking(1005, 102, 40, 2);
    bookings.createBooking(1006, 202, 45, 5);
    bookings.createBooking(1007, 303, 50, 2);

    int cancelled1 = bookings.createBooking(1008, 103, 60, 1);
    bookings.cancelBooking(cancelled1);
    int cancelled2 = bookings.createBooking(1001, 104, 70, 2);
    bookings.cancelBooking(cancelled2);
}

int main() {
    sf::RenderWindow window(sf::VideoMode({ 1300u, 680u }), "Hotel Management System");
    window.setFramerateLimit(60);

    sf::Font font;
    if (!font.openFromFile("arial.ttf")) {
        std::cout << "Cannot open arial.ttf. Copy it into the project folder." << std::endl;
        return 1;
    }
    sf::Font mono;
    if (!mono.openFromFile("consola.ttf")) {
        mono = font;
    }

    RoomManager rooms;
    StaffManager staff;
    GuestManager guests;
    BookingManager bookings(rooms, guests);
    BillingManager billing;

    Ui ui;
    loadDemoData(ui, rooms, staff, guests, bookings, billing);
    enterScreen(ui, SCREEN_LOGIN);

    while (window.isOpen()) {
        sf::Vector2f mouse(sf::Mouse::getPosition(window));
        std::vector<Button> buttons = makeButtons(ui.screen, ui.user);
        int pressed = BTN_NONE;

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            else if (const auto* click = event->getIf<sf::Event::MouseButtonPressed>()) {
                if (click->button == sf::Mouse::Button::Left) {
                    sf::Vector2f p(click->position);
                    for (size_t i = 0; i < ui.fields.size(); i++) {
                        if (fieldRect(ui.screen, static_cast<int>(i)).contains(p)) {
                            ui.activeField = static_cast<int>(i);
                        }
                    }
                    for (size_t i = 0; i < buttons.size(); i++) {
                        if (buttons[i].area.contains(p)) {
                            pressed = buttons[i].id;
                        }
                    }
                    if (ui.tableMode && p.x >= 530.f && p.x <= 1290.f &&
                        p.y >= TABLE_TOP && p.y < TABLE_TOP + ROW_HEIGHT * TABLE_ROWS) {
                        int index = ui.scroll + static_cast<int>((p.y - TABLE_TOP) / ROW_HEIGHT);
                        selectRow(ui, index);
                    }
                }
            }
            else if (const auto* typed = event->getIf<sf::Event::TextEntered>()) {
                if (!ui.fields.empty()) {
                    typeChar(ui.fields[ui.activeField], typed->unicode);
                }
            }
            else if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Tab && !ui.fields.empty()) {
                    ui.activeField = (ui.activeField + 1) % static_cast<int>(ui.fields.size());
                }
                else if (key->code == sf::Keyboard::Key::Enter && ui.screen == SCREEN_LOGIN) {
                    pressed = BTN_LOGIN;
                }
            }
            else if (const auto* wheel = event->getIf<sf::Event::MouseWheelScrolled>()) {
                ui.scroll -= static_cast<int>(wheel->delta * 3.f);
                clampScroll(ui);
            }
        }

        if (pressed != BTN_NONE) {
            handleButton(ui, pressed, rooms, staff, guests, bookings, billing);
        }

        window.clear(COLOR_BG);
        drawHeader(window, font, ui);

        if (ui.screen == SCREEN_LOGIN) {
            sf::RectangleShape panel({ 500.f, 430.f });
            panel.setPosition({ 400.f, 100.f });
            panel.setFillColor(sf::Color::White);
            window.draw(panel);
            drawText(window, font, "Staff Login", 30, 500.f, 125.f, COLOR_TEXT);
            drawText(window, font, ui.message, 18, 500.f, 445.f, COLOR_ERROR);
            drawText(window, font, "Tab: switch field     Enter: login", 16, 500.f, 490.f, COLOR_MUTED);
        }
        else if (ui.screen == SCREEN_DASHBOARD) {
            drawText(window, font, "Welcome, " + ui.user->getName(), 30, 500.f, 100.f, COLOR_TEXT);
            drawText(window, font, "Role: " + ui.user->getRole(), 20, 500.f, 145.f, COLOR_MUTED);
        }
        else {
            if (ui.tableMode) {
                drawTable(window, font, ui);
            }
            else {
                drawList(window, mono, ui);
            }
            drawText(window, font, ui.message, 18, 15.f, 645.f, ui.messageIsError ? COLOR_ERROR : COLOR_GOOD);
        }

        for (size_t i = 0; i < ui.fields.size(); i++) {
            int index = static_cast<int>(i);
            drawField(window, font, ui.fields[i], fieldRect(ui.screen, index), ui.activeField == index);
        }

        for (size_t i = 0; i < buttons.size(); i++) {
            drawButton(window, font, buttons[i], buttons[i].area.contains(mouse));
        }

        window.display();
    }

    return 0;
}