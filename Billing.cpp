#include <iomanip>
#include <sstream>
#include "Billing.h"

using namespace std;

// Helper: converts a number to text with 2 decimal places.
static string money(double value) {
    ostringstream out;
    out << fixed << setprecision(2) << value;
    return out.str();
}

// Helper: rounds a money value to 2 decimal places.
static double roundMoney(double value) {
    return (int)(value * 100 + 0.5) / 100.0;
}

// ===================== ServiceCharge =====================

// Creates a service charge.
ServiceCharge::ServiceCharge(const string& serviceName, double price)
    : serviceName(serviceName), price(price) {
}

// Returns the service name.
string ServiceCharge::getServiceName() const {
    return serviceName;
}

// Returns the service price.
double ServiceCharge::getPrice() const {
    return price;
}

// Returns the service as text.
string ServiceCharge::toString() const {
    return serviceName + ": " + money(price);
}

// ===================== Bill =====================

// Static members are defined here.
int Bill::nextBillId = 1;
const double Bill::TAX_RATE = 0.10;

// Creates a bill with the next bill id.
Bill::Bill(int bookingId, int guestId, double pricePerNight, int nights)
    : billId(nextBillId++), bookingId(bookingId), guestId(guestId),
      nights(nights), pricePerNight(pricePerNight), paid(false),
      paymentMethod(""), changeGiven(0.0), processingFee(0.0) {
}

// Room charges = nights * price per night.
double Bill::getRoomCharges() const {
    return nights * pricePerNight;
}

// Adds up the price of all services.
double Bill::getServicesTotal() const {
    double total = 0.0;
    for (size_t i = 0; i < services.size(); i++) {
        total += services[i].getPrice();
    }
    return total;
}

// Subtotal = room charges + services.
double Bill::getSubtotal() const {
    return getRoomCharges() + getServicesTotal();
}

// Tax = subtotal * tax rate.
double Bill::getTax() const {
    return getSubtotal() * TAX_RATE;
}

// Total = subtotal + tax.
double Bill::getTotal() const {
    return getSubtotal() + getTax();
}

// Adds a service to the bill (bill += service).
Bill& Bill::operator+=(const ServiceCharge& s) {
    services.push_back(s);
    return *this;
}

// Writes the bill to an output stream.
ostream& operator<<(ostream& os, const Bill& b) {
    os << b.toString();
    return os;
}

// Getters.
int Bill::getBillId() const { return billId; }
int Bill::getBookingId() const { return bookingId; }
int Bill::getGuestId() const { return guestId; }
int Bill::getNights() const { return nights; }
double Bill::getPricePerNight() const { return pricePerNight; }
const vector<ServiceCharge>& Bill::getServices() const { return services; }
bool Bill::isPaid() const { return paid; }
string Bill::getPaymentMethod() const { return paymentMethod; }
double Bill::getChangeGiven() const { return changeGiven; }
double Bill::getProcessingFee() const { return processingFee; }

// Setters.
void Bill::setPaid(bool paid) { this->paid = paid; }
void Bill::setPaymentMethod(const string& method) { paymentMethod = method; }
void Bill::setChangeGiven(double change) { changeGiven = change; }
void Bill::setProcessingFee(double fee) { processingFee = fee; }

// Returns the bill as one line of text.
string Bill::toString() const {
    ostringstream out;
    out << "Bill #" << billId
        << " | Booking: " << bookingId
        << " | Guest: " << guestId
        << " | Nights: " << nights
        << " | Price/night: " << money(pricePerNight)
        << " | Services: " << services.size()
        << " | Total: " << money(getTotal())
        << " | Paid: " << (paid ? "Yes" : "No");
    return out.str();
}

// Prints the bill text.
void Bill::display() const {
    cout << toString() << endl;
}

// ===================== Payment =====================

// Stores the amount given by the guest.
Payment::Payment(double amountTendered) : amountTendered(amountTendered) {
}

// Virtual destructor (empty).
Payment::~Payment() {
}

// Returns the amount given by the guest.
double Payment::getAmountTendered() const {
    return amountTendered;
}

// Returns the payment as text (method and amount given).
string Payment::toString() const {
    return getMethod() + " payment, amount given: " + money(amountTendered);
}

// ===================== CashPayment =====================

// Creates a cash payment.
CashPayment::CashPayment(double amountTendered) : Payment(amountTendered) {
}

// Cash has no fee.
double CashPayment::getProcessingFee(double) const {
    return 0.0;
}

// Method name is "Cash".
string CashPayment::getMethod() const {
    return "Cash";
}

// ===================== CardPayment =====================

// Creates a card payment.
CardPayment::CardPayment(double amountTendered) : Payment(amountTendered) {
}

// Card fee is 2% of the bill total.
double CardPayment::getProcessingFee(double billTotal) const {
    return billTotal * 0.02;
}

// Method name is "Card".
string CardPayment::getMethod() const {
    return "Card";
}

// ===================== BillingManager =====================

// Finds the position of a bill in the vector (-1 if not found).
int BillingManager::findIndex(int billId) const {
    for (size_t i = 0; i < bills.size(); i++) {
        if (bills[i].getBillId() == billId) {
            return (int)i;
        }
    }
    return -1;
}

// Creates a new bill and returns its id (or -1 on error).
int BillingManager::generateBill(int bookingId, int guestId, double pricePerNight, int nights) {
    if (nights <= 0) {
        lastError = "Nights must be greater than 0.";
        return -1;
    }
    if (pricePerNight <= 0) {
        lastError = "Price per night must be greater than 0.";
        return -1;
    }
    if (findBillByBooking(bookingId) != nullptr) {
        lastError = "A bill already exists for this booking.";
        return -1;
    }
    Bill newBill(bookingId, guestId, pricePerNight, nights);
    bills.push_back(newBill);
    lastError = "";   // clear the "no bill" message from the search above
    return newBill.getBillId();
}

// Adds a service to a bill that is not paid yet.
bool BillingManager::addServiceToBill(int billId, const ServiceCharge& s) {
    int index = findIndex(billId);
    if (index == -1) {
        lastError = "Bill not found.";
        return false;
    }
    if (bills[index].isPaid()) {
        lastError = "Bill is already paid.";
        return false;
    }
    if (s.getPrice() < 0) {
        lastError = "Service price cannot be negative.";
        return false;
    }
    bills[index] += s;
    lastError = "";
    return true;
}

// Finds a bill by bill id.
Bill* BillingManager::findBill(int billId) {
    int index = findIndex(billId);
    if (index == -1) {
        lastError = "Bill not found.";
        return nullptr;
    }
    lastError = "";
    return &bills[index];
}

// Finds a bill by booking id.
Bill* BillingManager::findBillByBooking(int bookingId) {
    for (size_t i = 0; i < bills.size(); i++) {
        if (bills[i].getBookingId() == bookingId) {
            lastError = "";
            return &bills[i];
        }
    }
    lastError = "No bill for this booking.";
    return nullptr;
}

// Amount due = bill total + fee (the fee depends on the payment type).
double BillingManager::getAmountDue(int billId, const Payment& p) const {
    int index = findIndex(billId);
    if (index == -1) {
        return -1;
    }
    double total = bills[index].getTotal();
    return roundMoney(total + p.getProcessingFee(total));   // polymorphic call
}

// Pays a bill if the money given is enough.
bool BillingManager::recordPayment(int billId, const Payment& p) {
    int index = findIndex(billId);
    if (index == -1) {
        lastError = "Bill not found.";
        return false;
    }
    if (bills[index].isPaid()) {
        lastError = "Bill is already paid.";
        return false;
    }
    double total = bills[index].getTotal();
    double fee = p.getProcessingFee(total);     // polymorphic call
    double due = roundMoney(total + fee);
    double given = roundMoney(p.getAmountTendered());
    if (given < due) {
        lastError = "Amount given is less than the amount due.";
        return false;
    }
    bills[index].setPaid(true);
    bills[index].setPaymentMethod(p.getMethod());   // polymorphic call
    bills[index].setProcessingFee(fee);
    bills[index].setChangeGiven(roundMoney(given - due));
    lastError = "";
    return true;
}

// Builds a multi-line receipt.
string BillingManager::getReceipt(int billId) const {
    int index = findIndex(billId);
    if (index == -1) {
        return "";
    }
    const Bill& b = bills[index];
    ostringstream out;
    out << "========== RECEIPT ==========\n";
    out << "Bill ID    : " << b.getBillId() << "\n";
    out << "Booking ID : " << b.getBookingId() << "\n";
    out << "Guest ID   : " << b.getGuestId() << "\n";
    out << "-----------------------------\n";
    out << "Room: " << b.getNights() << " nights x " << money(b.getPricePerNight())
        << " = " << money(b.getRoomCharges()) << "\n";
    const vector<ServiceCharge>& list = b.getServices();
    for (size_t i = 0; i < list.size(); i++) {
        out << "Service - " << list[i].toString() << "\n";
    }
    out << "-----------------------------\n";
    out << "Subtotal   : " << money(b.getSubtotal()) << "\n";
    out << "Tax (" << (int)(Bill::TAX_RATE * 100) << "%)   : " << money(b.getTax()) << "\n";
    out << "Total      : " << money(b.getTotal()) << "\n";
    if (b.isPaid()) {
        out << "Method     : " << b.getPaymentMethod() << "\n";
        out << "Fee        : " << money(b.getProcessingFee()) << "\n";
        out << "Change     : " << money(b.getChangeGiven()) << "\n";
        out << "Status     : PAID\n";
    } else {
        out << "Status     : NOT PAID\n";
    }
    out << "=============================";
    return out.str();
}

// Returns a copy of all bills.
vector<Bill> BillingManager::getAllBills() const {
    return bills;
}

// Returns the last error message.
string BillingManager::getLastError() const {
    return lastError;
}

// Returns the manager as text (number of bills).
string BillingManager::toString() const {
    ostringstream out;
    out << "BillingManager: " << bills.size() << " bills";
    return out.str();
}