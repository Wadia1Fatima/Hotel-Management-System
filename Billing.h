#ifndef BILLING_H
#define BILLING_H

#include <iostream>
#include <string>
#include <vector>

// An extra service (laundry, food, ...) with a name and a price.
class ServiceCharge {
private:
    std::string serviceName;
    double price;

public:
    // Creates a service charge.
    ServiceCharge(const std::string& serviceName, double price);

    // Getters.
    std::string getServiceName() const;
    double getPrice() const;

    // Returns the service as text.
    std::string toString() const;
};

// A bill for one booking: room charges + services + tax.
class Bill {
private:
    static int nextBillId;              // counter used to give each bill a new id

    int billId;
    int bookingId;
    int guestId;
    int nights;
    double pricePerNight;
    std::vector<ServiceCharge> services; // composition: a Bill has ServiceCharge objects
    bool paid;
    std::string paymentMethod;
    double changeGiven;
    double processingFee;               // extra: fee that was charged for the payment method

public:
    static const double TAX_RATE;       // tax rate (defined in Billing.cpp)

    // Creates a bill and takes the next bill id.
    Bill(int bookingId, int guestId, double pricePerNight, int nights);

    // Money calculations.
    double getRoomCharges() const;
    double getServicesTotal() const;
    double getSubtotal() const;
    double getTax() const;
    double getTotal() const;

    // Adds a service to the bill.
    Bill& operator+=(const ServiceCharge& s);

    // Prints the bill to any output stream.
    friend std::ostream& operator<<(std::ostream& os, const Bill& b);

    // Getters.
    int getBillId() const;
    int getBookingId() const;
    int getGuestId() const;
    int getNights() const;
    double getPricePerNight() const;
    const std::vector<ServiceCharge>& getServices() const;
    bool isPaid() const;
    std::string getPaymentMethod() const;
    double getChangeGiven() const;
    double getProcessingFee() const;

    // Setters.
    void setPaid(bool paid);
    void setPaymentMethod(const std::string& method);
    void setChangeGiven(double change);
    void setProcessingFee(double fee);

    // Returns the bill as text.
    std::string toString() const;

    // Prints toString() on the screen.
    void display() const;
};

// Abstract base class for all payment types.
class Payment {
private:
    double amountTendered;

public:
    // Stores the money the guest gives.
    Payment(double amountTendered);

    // Virtual destructor (needed for a base class).
    virtual ~Payment();

    // Returns the money the guest gives.
    double getAmountTendered() const;

    // Each payment type has its own fee.
    virtual double getProcessingFee(double billTotal) const = 0;

    // Each payment type has its own name.
    virtual std::string getMethod() const = 0;

    // Returns the payment as text (same for all payment types).
    std::string toString() const;
};

// Cash payment: no fee.
class CashPayment : public Payment {
public:
    CashPayment(double amountTendered);
    double getProcessingFee(double billTotal) const override;
    std::string getMethod() const override;
};

// Card payment: fee is 2% of the bill total.
class CardPayment : public Payment {
public:
    CardPayment(double amountTendered);
    double getProcessingFee(double billTotal) const override;
    std::string getMethod() const override;
};

// Manages all bills and payments.
class BillingManager {
private:
    std::vector<Bill> bills;
    std::string lastError;

    // Returns the position of a bill in the vector, or -1 if not found.
    int findIndex(int billId) const;

public:
    // Creates a bill; returns the billId or -1 on error.
    int generateBill(int bookingId, int guestId, double pricePerNight, int nights);

    // Adds a service to an unpaid bill.
    bool addServiceToBill(int billId, const ServiceCharge& s);

    // Find a bill by its id or by its booking id (nullptr if not found).
    // Note: the pointer may become invalid after generateBill() is called again.
    Bill* findBill(int billId);
    Bill* findBillByBooking(int bookingId);

    // Bill total + fee of the payment method (-1 if the bill is not found).
    double getAmountDue(int billId, const Payment& p) const;

    // Pays the bill if the money given is enough.
    bool recordPayment(int billId, const Payment& p);

    // Returns a multi-line receipt (empty string if the bill is not found).
    std::string getReceipt(int billId) const;

    // Returns a copy of all bills.
    std::vector<Bill> getAllBills() const;

    // Returns the last error message.
    std::string getLastError() const;

    // Returns the manager as text.
    std::string toString() const;
};

#endif