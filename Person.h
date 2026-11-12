#ifndef PERSON_H
#define PERSON_H
#include <string>

class Person {
protected:
    int id;
    std::string name;
    std::string phone;
public:
    Person(int id, const std::string& name, const std::string& phone)
        : id(id), name(name), phone(phone) {}
    virtual ~Person() {}
    int getId() const { return id; }
    std::string getName() const { return name; }
    std::string getPhone() const { return phone; }
    void setName(const std::string& n) { name = n; }
    void setPhone(const std::string& p) { phone = p; }
    virtual std::string getRole() const = 0;
    virtual std::string toString() const {
        return "ID: " + std::to_string(id) + ", Name: " + name + ", Phone: " + phone;
    }
};
#endif