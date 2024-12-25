#include <iostream>
#include <ctime>
#include <iomanip>

#include "sqlite3.h"

using namespace std;

string admin_key = "admin";
// string "Enter your choice: " = "Please select an option: ";


struct Node {
    int reservationId, userId, carId;
    string status, reported_by;
    float total_amount_paid, extra_charge;
    string end_date;
    Node * next;

    Node(int reserve_id, int usr_id, int car_id, float total_charge,
     float penalty, string status_of_rsv, string return_date, string reporter="") {
        reservationId = reserve_id;
        userId = usr_id;
        carId = car_id;
        total_amount_paid = total_charge;
        extra_charge = penalty;
        status = status_of_rsv;
        end_date = return_date;
        reported_by = reporter;
        next = nullptr;
    }
};

/*
This class need an update on the display method and specially at 
addElement method to check if the reservation id already exists in 
the linked list if it does exist the method should update this
reservation with the new one else this will make a duplicate data
but may be since this car rental system it may not be necessary.
*/

class DailyReport{
    Node * head = nullptr;

public:

    void addReport(int reserve_id, int usr_id, int car_id, float total_charge,
                float penalty, string status_of_rsv, string end_date, string reporter="") {

        Node* newNode = new Node(reserve_id, usr_id, car_id, total_charge,
                                 penalty, status_of_rsv, end_date, reporter);
        
        if (head == nullptr) {
            head = newNode;
            return;
        }

        newNode->next = head;
        head = newNode;
    }

    void displayAll() {
        string status;
        cout << "=====================Reports=====================" << endl;

        if (head == nullptr) {
            cout << "No reports available.\n";
            return;
        }
        while (true){
        cout << "1. All Reports." << endl;
        cout << "2. New Reservations." << endl;
        cout << "3. Comleted Reservations." << endl;
        cout << "4. Cancelled Reservations." << endl;
        cout << "0. Back." << endl;
        cout << "Enter your choice: ";
        int choice;
        cin >> choice;

        switch (choice)
        {
            case 0:
                return;
            case 1:
                status = "All";
                break;
            case 2:
                status = "Pending";
                break;
            case 3:
                status = "Complete";
                break;
            case 4:
                status = "Canceled";
                break;
            
            default:
                cerr << "Invalid choice please try again!!" << endl;
                break;
        }


        Node* temp = head;
        bool found = false;

        // Table header
        cout << left << setw(12) << "Res. ID" << setw(10) << "User ID"
             << setw(10) << "Car ID" << setw(15) << "Total Paid"
             << setw(15) << "Extra Charge" << setw(15) << "Status"
             << setw(15) << "End Date" << setw(15) << "Reporter" << endl;

        cout << string(100, '-') << endl;

        // Search for the given status
        while (temp != nullptr) {
            if (status != "All"){
                if (temp->status != status){
                    temp = temp->next;
                    continue;
                    }
            }
                found = true;
                cout << left << setw(12) << temp->reservationId
                     << setw(10) << temp->userId
                     << setw(10) << temp->carId
                     << setw(15) << temp->total_amount_paid
                     << setw(15) << temp->extra_charge
                     << setw(15) << temp->status
                     << setw(15) << temp->end_date
                     << setw(15) << temp->reported_by << endl;
            
            temp = temp->next;
        }

        if (!found) {
            cout << "No reports found with status: " << status << endl;
        }

        cout << string(100, '-') << endl;
        cout << "======================End of Reports=====================" << endl;
    }
    }


/* 
    This function returns "true" or "1" if the current date of the machine is
    greater than or equal to the end date from the database.
    It helps to identify the overdue reservations
   
*/ 
  bool compareDates(string end_date) {
        time_t now = time(0); // Current time

        tm* currentDate = localtime(&now);

        try {
            int year = stoi(end_date.substr(0, 4));
            int month = stoi(end_date.substr(5, 2));
            int day = stoi(end_date.substr(8, 2));

            tm endDate = {};
            endDate.tm_year = year - 1900;
            endDate.tm_mon = month - 1;
            endDate.tm_mday = day;

            time_t endTime = mktime(&endDate);

            if (endTime == -1) {
                throw runtime_error("Invalid date conversion.");
            }

            return now >= endTime;
        }
        catch (exception& e) {
            cerr << "Error: " << e.what() << endl;
            return false; 
        }
    }


} report;



struct User{
    string first_name, last_name, email, password, phone, address;
    string user_role;

    User(){
        user_role = "Customer";
    }
};

class Users{

    /* 
        This function is used for adding date to the current date and
        finding the difference between two dates and return the current date
    */
string dateManipulation(string end_date="", int days=0){
        string result = "";
        time_t now = time(0);

        tm* currentDate = localtime(&now);

        if (days != 0){
            currentDate->tm_mday += days;

            mktime(currentDate); 
            result = to_string(currentDate->tm_year + 1900) + '-' +
                (currentDate->tm_mon + 1 < 10 ? "0" : "") + to_string(currentDate->tm_mon + 1) + '-' +
                (currentDate->tm_mday < 10 ? "0" : "") + to_string(currentDate->tm_mday);
            return result;
        }
        else if (!end_date.empty()){
        try {
            int year = stoi(end_date.substr(0, 4));
            int month = stoi(end_date.substr(5, 2));
            int day = stoi(end_date.substr(8, 2));

            
            tm endDate = {};
            endDate.tm_year = year - 1900;
            endDate.tm_mon = month - 1;
            endDate.tm_mday = day;

            time_t endTime = mktime(&endDate);

            cout << "endTime = " << endTime << endl;

            double diffInSeconds = difftime(now, endTime);
            int diffInDays = (diffInSeconds / 86400) + 1; // 86,400 is number of seconds in one day 
            result = to_string(diffInDays);
        }
        catch (exception& e){
            cout << "Error: " << e.what() << endl;
            result = "Error";
        }
        }

        else {
            result = to_string(currentDate->tm_year + 1900) + '-' +
                (currentDate->tm_mon + 1 < 10 ? "0" : "") + to_string(currentDate->tm_mon + 1) + '-' +
                (currentDate->tm_mday < 10 ? "0" : "") + to_string(currentDate->tm_mday);
        }

        return result;

    }

void viewAllUsers(sqlite3 *DB, bool isManager = false) {
    int choice;
    char continu = 'y';
    string roleFilter = "Customer";

    while (continu == 'y' || continu == 'Y'){
        continu = 'n';
    if (isManager) {
        cout << "\nChoose the type of users you want to view:" << endl;
        cout << "1. All Users" << endl;
        cout << "2. Admins" << endl;
        cout << "3. Inventory Managers" << endl;
        cout << "4. Service Agents" << endl;
        cout << "5. Customers" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1:
                roleFilter = "";  // No filter for all users
                break;
            case 2:
                roleFilter = "Admin";
                break;
            case 3:
                roleFilter = "Inventory Manager";
                break;
            case 4:
                roleFilter = "Service Agent";
                break;
            case 5:
                roleFilter = "Customer";
                break;
            default:
                cout << "Invalid choice. Returning to menu." << endl;
                return;
        }
    }

    string query = "SELECT user_id, first_name, last_name, email, phone, address, role FROM users";
    if (!roleFilter.empty()) {
        query += " WHERE role = '" + roleFilter + "'";
    }

    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    cout << "\n==========================================" << endl;
    cout << "                User List                 " << endl;
    cout << "==========================================" << endl;

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        int userId = sqlite3_column_int(stmt, 0);
        string firstName = (char *)sqlite3_column_text(stmt, 1);
        string lastName = (char *)sqlite3_column_text(stmt, 2);
        string email = (char *)sqlite3_column_text(stmt, 3);
        string phone = (char *)sqlite3_column_text(stmt, 4);
        string address = (char *)sqlite3_column_text(stmt, 5);
        string role = (char *)sqlite3_column_text(stmt, 6);

        cout << "User ID: " << userId << endl;
        cout << "Name: " << firstName << " " << lastName << endl;
        cout << "Email: " << email << endl;
        cout << "Phone: " << phone << endl;
        cout << "Address: " << address << endl;
        cout << "Role: " << role << endl;
        cout << "------------------------------------------" << endl;
    }

    if (rc != SQLITE_DONE) {
        cerr << "Error fetching data: " << sqlite3_errmsg(DB) << endl;
    }

    sqlite3_finalize(stmt);
    cout << "==========================================" << endl;
    cout << "End of user list.\n" << endl;

    if (isManager){ // this is only for admins
        cout << "Would you like to go again?(y/n): ";
        cin >> continu;
    }

    }
}

void updateUser(sqlite3 *DB) {
    int userId;
    cout << "\nEnter the User ID of the user you want to update: ";
    cin >> userId;
    cin.ignore(); // To consume the newline character left by cin

    // Fetch the user details to confirm existence
    string query = "SELECT first_name, last_name, email, phone, address, role FROM users WHERE user_id = " + to_string(userId) + ";";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "User with ID " << userId << " not found." << endl;
        sqlite3_finalize(stmt);
        return;
    }

    // Display current user details
    string firstName = (char *)sqlite3_column_text(stmt, 0);
    string lastName = (char *)sqlite3_column_text(stmt, 1);
    string email = (char *)sqlite3_column_text(stmt, 2);
    string phone = (char *)sqlite3_column_text(stmt, 3);
    string address = (char *)sqlite3_column_text(stmt, 4);
    string role = (char *)sqlite3_column_text(stmt, 5);

    cout << "\nCurrent details for User ID " << userId << ":\n";
    cout << "1. First Name: " << firstName << endl;
    cout << "2. Last Name: " << lastName << endl;
    cout << "3. Email: " << email << endl;
    cout << "4. Phone: " << phone << endl;
    cout << "5. Address: " << address << endl;
    cout << "6. Role: " << role << endl;
    cout << "===================================" << endl;

    sqlite3_finalize(stmt);

    // Get new values for all fields
    string newFirstName, newLastName, newEmail, newPhone, newAddress, newRole;
    cout << "Enter new First Name (current: " << firstName << "): ";
    getline(cin, newFirstName);
    cout << "Enter new Last Name (current: " << lastName << "): ";
    getline(cin, newLastName);
    cout << "Enter new Email (current: " << email << "): ";
    getline(cin, newEmail);
    cout << "Enter new Phone (current: " << phone << "): ";
    getline(cin, newPhone);
    cout << "Enter new Address (current: " << address << "): ";
    getline(cin, newAddress);
    cout << "Enter new Role (current: " << role << "): ";
    getline(cin, newRole);

    // Construct the update query
    string updateQuery = "UPDATE users SET first_name = '" + newFirstName +
                         "', last_name = '" + newLastName +
                         "', email = '" + newEmail +
                         "', phone = '" + newPhone +
                         "', address = '" + newAddress +
                         "', role = '" + newRole +
                         "', updated_at = CURRENT_TIMESTAMP WHERE user_id = " + to_string(userId) + ";";

    // Execute the update
    rc = sqlite3_exec(DB, updateQuery.c_str(), 0, 0, nullptr);
    if (rc == SQLITE_OK) {
        cout << "User updated successfully!" << endl;
    } else {
        cerr << "Error updating user: " << sqlite3_errmsg(DB) << endl;
    }
}

// This function can`t delete Customer or Admin
void fireEmployee(sqlite3 *DB) {
    int userId;
    cout << "\nEnter the User ID of the employee you want to fire: ";
    cin >> userId;

    // Check if the user exists and is an employee (not Admin or Customer)
    string query = "SELECT role FROM users WHERE user_id = " + to_string(userId) + ";";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    if (sqlite3_step(stmt) != SQLITE_ROW) {
        cout << "User with ID " << userId << " not found." << endl;
        sqlite3_finalize(stmt);
        return;
    }

    string role = (char *)sqlite3_column_text(stmt, 0);
    if (role == "Admin" || role == "Customer") {
        cout << "You cannot fire an Admin or Customer." << endl;
        sqlite3_finalize(stmt);
        return;
    }

    sqlite3_finalize(stmt);

    // Confirm firing the employee
    cout << "Are you sure you want to fire this " << role << "? (y/n): ";
    char confirmation;
    cin >> confirmation;

    if (confirmation == 'y' || confirmation == 'Y') {
        // Delete the employee from the database
        string deleteQuery = "DELETE FROM users WHERE user_id = " + to_string(userId) + ";";
        rc = sqlite3_exec(DB, deleteQuery.c_str(), nullptr, nullptr, nullptr);
        if (rc == SQLITE_OK) {
            cout << "Employee with ID " << userId << " has been fired successfully." << endl;
        } else {
            cerr << "Error firing employee: " << sqlite3_errmsg(DB) << endl;
        }
    } else {
        cout << "Employee firing canceled." << endl;
    }
}


bool insertPayment(sqlite3 *DB, int reservation_id, float amount, string &payment_method, string &status) {
    string insertQuery = "INSERT INTO payments (reservation_id, amount, payment_method, status) VALUES (" +
                         to_string(reservation_id) + ", " + to_string(amount) + ", '" + payment_method + "', '" + status + "');";
    
    int rc = sqlite3_exec(DB, insertQuery.c_str(), 0, 0, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error inserting payment data: " << sqlite3_errmsg(DB) << endl;
        return false;
    }
    cout << "Payment record inserted successfully!" << endl;
    return true;
}

void addNewCar(sqlite3 *DB) {
    string produced_by, model, license_plate, status = "Available", description;
    float daily_rental_price;
    cin.ignore();
    cout << "Enter Car Manufacturer (Produced By): ";
    getline(cin, produced_by);
    cout << "Enter Car Model: ";
    getline(cin, model);
    cout << "Enter License Plate: ";
    getline(cin, license_plate);
    cout << "Enter Daily Rental Price: ";
    cin >> daily_rental_price;
    cin.ignore();
    cout << "Enter concise description of the car (e.g., engine type): ";
    getline(cin, description);

    string query = "INSERT INTO cars (produced_by, model, license_plate, daily_rental_price, status, description) VALUES ('" +
                   produced_by + "', '" + model + "', '" + license_plate + "', " + to_string(daily_rental_price) + ", '" + status + "', '" + description + "');";

    char *errMsg = 0;
    int rc = sqlite3_exec(DB, query.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error adding new car: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Car added successfully!" << endl;
    }
}

void updateCar(sqlite3 *DB) {
    int car_id;
    string produced_by, model, license_plate, status, description;
    float daily_rental_price;

    cout << "Enter Car ID to Update: ";
    cin >> car_id;
    cin.ignore();

    string checkQuery = "SELECT * FROM cars WHERE car_id = " + to_string(car_id) + ";";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, checkQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Error checking car existence: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "Car with ID " << car_id << " does not exist." << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    cout << "Enter New Manufacturer (Produced By): ";
    getline(cin, produced_by);
    cout << "Enter New Model: ";
    getline(cin, model);
    cout << "Enter New License Plate: ";
    getline(cin, license_plate);
    cout << "Enter New Daily Rental Price: ";
    cin >> daily_rental_price;
    cin.ignore();
    cout << "Enter New Status (Available/Rented/Under Maintainance): ";
    getline(cin, status);
    cout << "Enter the description: ";
    getline(cin, description);

    string query = "UPDATE cars SET produced_by = '" + produced_by +
                   "', model = '" + model +
                   "', license_plate = '" + license_plate +
                   "', daily_rental_price = " + to_string(daily_rental_price) +
                   ", status = '" + status +
                   "', description = '" + description +
                   "', updated_at = CURRENT_TIMESTAMP WHERE car_id = " + to_string(car_id) + ";";

    char *errMsg = 0;
    rc = sqlite3_exec(DB, query.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error updating car details: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Car details updated successfully!" << endl;
    }
}

void removeCar(sqlite3 *DB) {
    int car_id;
    cout << "Enter Car ID to Remove: ";
    cin >> car_id;

    string checkQuery = "SELECT * FROM cars WHERE car_id = " + to_string(car_id) + ";";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, checkQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Error checking car existence: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "Car with ID " << car_id << " does not exist." << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    char confirm;
    cout << "Are you sure you want to delete this car? (y/n): ";
    cin >> confirm;
    if (confirm != 'y' && confirm != 'Y') {
        cout << "Deletion Canceled." << endl;
        return;
    }

    string query = "DELETE FROM cars WHERE car_id = " + to_string(car_id) + ";";

    char *errMsg = 0;
    rc = sqlite3_exec(DB, query.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error removing car: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Car removed successfully!" << endl;
    }
}

void makeReport(sqlite3 *DB, int Car_id, int employee_id){
    string first_name, last_name;
    
    string query = "SELECT first_name, last_name FROM users WHERE user_id = " + to_string(employee_id);
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, query.c_str(),-1, &stmt, nullptr);
    if (rc!= SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    
    rc = sqlite3_step(stmt);
    if (rc!= SQLITE_ROW) {
        cout << "Your ID is not valid" << endl;
        sqlite3_finalize(stmt);
        return;
    }

    first_name = (char *)sqlite3_column_text(stmt, 0);
    last_name = (char *)sqlite3_column_text(stmt,1);

    sqlite3_finalize(stmt);

    query = "SELECT * FROM reservations WHERE car_id = " + to_string(Car_id);

    rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr);
    if (rc!= SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW){
        cout << "No reservations found for this car." << endl;
        sqlite3_finalize(stmt);
        return;
    }
    
    int reserve_id = sqlite3_column_int(stmt, 0);
    int user_id = sqlite3_column_int(stmt, 1);
    float total_amount = sqlite3_column_double(stmt, 5);
    float extra_amount = sqlite3_column_double(stmt, 6);
    string status = (char *)sqlite3_column_text(stmt, 7);
    string end_date = (char *)sqlite3_column_text(stmt, 4);
    string name = first_name + " " + last_name;

    report.addReport(reserve_id, user_id, Car_id, total_amount,
                     extra_amount, status, end_date, name);

}

// The improvement for this function is done
void makeReservation(sqlite3* DB, int employee_id, int user_id = 0) {
    int car_id;
    int days;
    cout << "Making reservation..." << endl;
    if (user_id == 0){
        cout << "Enter the user ID: ";
        cin >> user_id;
    }

    // Check for any pending reservations for this user
    string pendingQuery = "SELECT reservation_id, end_date FROM reservations WHERE user_id = " + to_string(user_id) + " AND status = 'Pending';";
    sqlite3_stmt* pendingStmt;

    int rc = sqlite3_prepare_v2(DB, pendingQuery.c_str(), -1, &pendingStmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement for checking pending reservations: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    bool hasOverdueReservation = false;

    while (sqlite3_step(pendingStmt) == SQLITE_ROW) {
        int reservation_id = sqlite3_column_int(pendingStmt, 0);
        string end_date = (char*)sqlite3_column_text(pendingStmt, 1);

        // Check if the reservation is overdue using report.compareDates(end_date)
        if (report.compareDates(end_date)) {
            cout << "Warning: User has an overdue reservation (Reservation ID: " << reservation_id << ").\n";
            hasOverdueReservation = true;
        }
    }

    sqlite3_finalize(pendingStmt);

    if (hasOverdueReservation) {
        cout << "Cannot proceed with the reservation until overdue reservations are resolved.\n";
        return;
    }

    // Proceed with reservation if no overdue reservations
    cout << "Enter the car ID: ";
    cin >> car_id;

    string carQuery = "SELECT daily_rental_price, status FROM cars WHERE car_id = " + to_string(car_id) + ";";
    sqlite3_stmt* stmt;
    rc = sqlite3_prepare_v2(DB, carQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cout << "Car not found or unavailable." << endl;
        sqlite3_finalize(stmt);
        return;
    }


    float daily_price = sqlite3_column_double(stmt, 0);
    string status = (char*)sqlite3_column_text(stmt, 1);
    sqlite3_finalize(stmt);

    if (status != "Available") {
        cout << "Car is not available for reservation." << endl;
        return;
    }

    cout << "For how long do you need the car? (Enter number of days): ";
    cin >> days;
    
    float total_price = daily_price * days; 
    string end_date = dateManipulation("", days);

    string insertQuery = "INSERT INTO reservations (user_id, car_id, end_date, total_price) VALUES (" 
        + to_string(user_id) + ", " + to_string(car_id) + ", '" + end_date + "', " + to_string(total_price) + ");";

    rc = sqlite3_exec(DB, insertQuery.c_str(), 0, 0, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Error inserting reservation: " << sqlite3_errmsg(DB) << endl;
        return;
    } else {
        cout << "Reservation made successfully!" << endl;
        makeReport(DB, car_id, employee_id); // Generate report
    }

    string updateCarQuery = "UPDATE cars SET status = 'Rented' WHERE car_id = " + to_string(car_id) + ";";
    rc = sqlite3_exec(DB, updateCarQuery.c_str(), 0, 0, nullptr);

    if (rc != SQLITE_OK) {
        cerr << "Error updating car status: " << sqlite3_errmsg(DB) << endl;
    } else {
        cout << "Car status updated successfully!" << endl;
    }
}

// done maybe it need some additional improvement to generate reports
void cancelReservation(sqlite3 *DB, int employee_id) {
    int reserve_id, id;
    float penality = 0.25, daily_rent = 0.0, total_price;
    string payment_method, payment_status = "Success";

    cout << "Enter the user ID: ";
    cin >> id;
    cout << "Enter the Reservation ID: ";
    cin >> reserve_id;

    if (reserve_id <= 0) {
        cerr << "Invalid Reservation ID entered." << endl;
        return;
    }

    string query = "SELECT * FROM reservations WHERE reservation_id = " + to_string(reserve_id) + ";";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Error preparing query: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "No reservation found with this ID: " << reserve_id << endl;
        sqlite3_finalize(stmt);
        return;
    }

    int user_id = sqlite3_column_int(stmt, 1);
    int car_id = sqlite3_column_int(stmt, 2);
    string end_date = (char *)sqlite3_column_text(stmt, 4);
    total_price = sqlite3_column_double(stmt, 5);
    string status = (char *)sqlite3_column_text(stmt, 7);

    if (user_id != id) {
        cerr << "Reservation does not belong to this user." << endl;
        sqlite3_finalize(stmt);
        return;
    }

    else if (status != "Pending") {
        cout << "Reservation is already " + status + "ed "<< endl;
        return;
    }
    
    sqlite3_finalize(stmt);

    // Calculate penalty or refund based on reservation timing
    int days = stoi(dateManipulation(end_date, 0)); // Positive: Late; Negative: Early
    if (days > 0) {
        penality *= days;
        total_price += penality;
        status = "Completed";
        cout << "Penalty for " << days << " late days applied." << endl;
    } else if (days < 0) {
        string rentalQuery = "SELECT daily_rent FROM cars WHERE car_id = " + to_string(car_id) + ";";
        rc = sqlite3_prepare_v2(DB, rentalQuery.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "Error fetching rental price: " << sqlite3_errmsg(DB) << endl;
            return;
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            cerr << "No rental price found for car ID: " << car_id << endl;
            sqlite3_finalize(stmt);
            return;
        }
        daily_rent = sqlite3_column_double(stmt, 0);
        total_price += days * daily_rent; // days is negative for early return
        status = "Cancelled";
        sqlite3_finalize(stmt);
    } else {
        status = "Completed";
    }

    cout << "Total amount to be paid: " << total_price << endl;
    cout << "Enter payment method (Credit Card, Debit Card, PayPal, Cash, TeleBirr, CBE): ";
    cin.ignore();
    getline(cin, payment_method);

    // Insert payment record
    if (!insertPayment(DB, reserve_id, total_price, payment_method, payment_status)) {
        cerr << "Payment insertion failed." << endl;
        return;
    }

    // Update reservation
    string updateQuery = "UPDATE reservations SET extra_charge = " + to_string(penality) +
                         ", total_price = " + to_string(total_price) +
                         ", status = '" + status + "' WHERE reservation_id = " + to_string(reserve_id) + ";";
    rc = sqlite3_exec(DB, updateQuery.c_str(), 0, 0, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error updating reservation: " << sqlite3_errmsg(DB) << endl;
        return;
    }
    makeReport(DB, car_id, employee_id); // Generate report
    // Update car status to "Available"
    string updateCarQuery = "UPDATE cars SET status = 'Available' WHERE car_id = " + to_string(car_id) + ";";
    rc = sqlite3_exec(DB, updateCarQuery.c_str(), 0, 0, nullptr);
    
    if (rc != SQLITE_OK) {
        cerr << "Error updating car status: " << sqlite3_errmsg(DB) << endl;
        return;
    }


    cout << "=================== Reservation cancelled successfully =====================" << endl;
}
void viewReservationDetails(sqlite3 *DB) {
    cout << "=================== Reservation Details ====================" << endl;
    cout << "Search by:" << endl;
    cout << "1. User ID" << endl;
    cout << "2. Status" << endl;
    cout << "Choose an option (1 or 2): ";
    int choice;
    cin >> choice;

    string query;
    if (choice == 1) {
        int user_id;
        cout << "Enter User ID: ";
        cin >> user_id;
        query = "SELECT * FROM reservations WHERE user_id = " + to_string(user_id) + ";";
    } else if (choice == 2) {
        string status;
        cout << "Enter Status (Pending, Completed, or Canceled): ";
        cin.ignore();
        getline(cin, status);
        query = "SELECT * FROM reservations WHERE status = '" + status + "';";
    } else {
        cout << "Invalid option selected." << endl;
        return;
    }

    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
        bool hasResults = false;

        while (sqlite3_step(stmt) == SQLITE_ROW) {
            hasResults = true;

            // Fetching all reservation table columns
            int reservation_id = sqlite3_column_int(stmt, 0);
            int user_id = sqlite3_column_int(stmt, 1);
            int car_id = sqlite3_column_int(stmt, 2);
            string end_date = (char *)sqlite3_column_text(stmt, 4);
            double total_price = sqlite3_column_double(stmt, 5);
            double extra_charge = sqlite3_column_double(stmt, 6);
            string status = (char *)sqlite3_column_text(stmt, 7);

            // Fetch car details
            string carQuery = "SELECT * FROM cars WHERE car_id = " + to_string(car_id) + ";";
            sqlite3_stmt *stmt_car;
            sqlite3_prepare_v2(DB, carQuery.c_str(), -1, &stmt_car, nullptr);
            sqlite3_step(stmt_car);

            string producer = (char *)sqlite3_column_text(stmt_car, 1);
            string model = (char *)sqlite3_column_text(stmt_car, 2);
            string license_plate = (char *)sqlite3_column_text(stmt_car, 3);

            sqlite3_finalize(stmt_car);

            // Displaying all data
            cout << "----------------------------------------------------" << endl;
            cout << "Reservation ID: " << reservation_id << endl;
            cout << "User ID: " << user_id << endl;
            cout << "Car Details: " << endl;
            cout << "\tProducer: " << producer << endl;
            cout << "\tModel: " << model << endl;
            cout << "\tLicense Plate: " << license_plate << endl;
            cout << "End Date: " << end_date << endl;
            cout << "Total Price: $" << total_price << endl;
            cout << "Extra Charge: $" << extra_charge << endl;
            cout << "Status: " << status << endl;
            cout << "----------------------------------------------------" << endl;
        }
        cout << "=========================END======================" << endl;
        if (!hasResults) {
            cout << "\nNo reservations found for the given criteria.\n";
        }

        sqlite3_finalize(stmt);
    } else {
        cerr << "Error preparing query: " << sqlite3_errmsg(DB) << endl;
    }
}

void viewAllReservation(sqlite3 *DB) {
    int choice;

    while (true) {
        cout << endl << "================ Reservation Menu ==============" << endl;
        cout << "1. View All Reservations" << endl;
        cout << "2. View Overdue Reservations" << endl;
        cout << "0. Back" << endl;
        cout << "===============================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 0) {
            cout << "Returning to the previous menu.\n";
            return;
        }

        string query = "SELECT * FROM reservations";

        sqlite3_stmt *stmt;
        if (sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, nullptr) == SQLITE_OK) {
            bool hasResults = false;

            while (sqlite3_step(stmt) == SQLITE_ROW) {
                int reservation_id = sqlite3_column_int(stmt, 0);
                int user_id = sqlite3_column_int(stmt, 1);
                int car_id = sqlite3_column_int(stmt, 2);
                string start_date = (char *)sqlite3_column_text(stmt, 3);
                string end_date = (char *)sqlite3_column_text(stmt, 4);
                double total_price = sqlite3_column_double(stmt, 5);
                double extra_charge = sqlite3_column_double(stmt, 6);
                string status = (char *)sqlite3_column_text(stmt, 7);

                // Skip if option 2 (Overdue Reservations) is selected and the end date is not overdue
                if (choice == 2) {
                    if (status != "Pending")
                        continue;
                    else if (!report.compareDates(end_date))
                        continue;
                }

                string carQuery = "SELECT * FROM cars WHERE car_id = " + to_string(car_id) + ";";
                sqlite3_stmt *stmt_car;

                if (sqlite3_prepare_v2(DB, carQuery.c_str(), -1, &stmt_car, nullptr) == SQLITE_OK) {
                    if (sqlite3_step(stmt_car) == SQLITE_ROW) {
                        string producer = (char *)sqlite3_column_text(stmt_car, 1);
                        string model = (char *)sqlite3_column_text(stmt_car, 2);
                        string license_plate = (char *)sqlite3_column_text(stmt_car, 3);

                        // Display reservation details
                        cout << "----------------------------------------------------" << endl;
                        cout << "Reservation ID: " << reservation_id << endl;
                        cout << "User ID: " << user_id << endl;
                        cout << "Producer: " << producer << endl;
                        cout << "Model: " << model << endl;
                        cout << "License Plate: " << license_plate << endl;
                        cout << "Car ID: " << car_id << endl;
                        cout << "Start Date: " << start_date << endl;
                        cout << "End Date: " << end_date << endl;
                        cout << "Total Price: $" << total_price << endl;
                        cout << "Extra Charge: $" << extra_charge << endl;
                        cout << "Status: " << status << endl;
                        cout << "----------------------------------------------------" << endl;

                        hasResults = true;
                    }
                    sqlite3_finalize(stmt_car);
                } else {
                    cerr << "Error fetching car details: " << sqlite3_errmsg(DB) << endl;
                }
            }

            if (!hasResults) {
                cout << "\nNo records found.\n";
            }

            sqlite3_finalize(stmt);
        } else {
            cerr << "Error executing query: " << sqlite3_errmsg(DB) << endl;
        }
    }
}

// done
void agentMenu(sqlite3 *DB, int employee_id){
    int choice = 0;
    while(true){
        cout << endl << "================Agent Menu==============" << endl;
        cout << "1. Make a Reservation" << endl;
        cout << "2. Cancel a Reservation" << endl;
        cout << "3. Search Reservation" << endl;
        cout << "4. Register New Customer" << endl;
        cout << "5. View All Customers." << endl;
        cout << "6. View Reservations." << endl;
        cout << "0. Log Out" << endl;
        cout << "=============================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch(choice){
            case 1:
                makeReservation(DB, employee_id);
                break;
            case 2:
                cancelReservation(DB, employee_id);
                break;
            case 3:
                viewReservationDetails(DB);
                break;
            case 4:
                cout << "Registering a new user..." << endl;
                register_user(DB, employee_id);
                break;
            case 5:
                cout << "Loading users..." << endl;
                viewAllUsers(DB);
                break;
            case 6:
                cout << "Loading users..." << endl;
                viewAllReservation(DB);
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice! Please try again." << endl;
            }
        }
    }

void inventoryManagerMenu(sqlite3* DB) {
    int choice = 0;
    A:
    cout << "============================================" << endl;
    cout << "           Inventory Manager Menu           " << endl;
    cout << "============================================" << endl;
    cout << "1. Add a New Car" << endl;
    cout << "2. Update Car Details" << endl;
    cout << "3. Remove a Car" << endl;
    cout << "4. View Car" << endl;
    cout << "5. View Reservations " << endl;
    cout << "0. Log Out" << endl;
    cout << "=============================================" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

    do {
        switch (choice) {
            case 1:
                cout << "Adding a new car..." << endl;
                addNewCar(DB);
                goto A;
                break;
            case 2:
                cout << "Updating car details..." << endl;
                updateCar(DB);
                goto A;
                break;
            case 3:
                cout << "Removing a car..." << endl;
                removeCar(DB);
                goto A;
                break;
            case 4:
                cout << "Viewing cars..." << endl;
                viewCars(DB, true);
                goto A;
                break;
            case 5:
                cout << "Redirecting to reservations..." << endl;
                viewAllReservation(DB);
                goto A;
                break;
            case 0:
                cout << "Returning to Main Menu..." << endl;
                break;
            default:
                cout << "Invalid choice. Try again!" << endl;
                goto A;
        }
    } while (choice != 0);
}

//need to implement this function
void adminMenu(sqlite3 *DB, int employee_id) {
    int choice = 0;
    string key = admin_key;

    while (true) {
        cout << endl
             << "=================================" << endl;
        cout << "            Admin Menu           " << endl;
        cout << "=================================" << endl;
        cout << "1. Add New User" << endl;
        cout << "2. View Reports" << endl;
        cout << "3. View Users" << endl;
        cout << "4. Update User Data" << endl;
        cout << "5. Fire Employee" << endl;
        cout << "0. Log Out" << endl;
        cout << "=================================" << endl;
        cout << "Enter your choice: ";
        cin >> choice;


        switch (choice) {
            case 1:
                cout << "Redirecting to add a new user..." << endl;
                register_user(DB, employee_id, key);
                break;

            case 2:
                cout << "Displaying all reports..." << endl;
                report.displayAll(); 
                break;

            case 3:
                cout << "Fetching users..." << endl;
                viewAllUsers(DB, true); // Function to query and display all employees
                break;

            case 4:
                cout << "Updating employee data..." << endl;
                updateUser(DB); // Function to update employee details
                break;

            case 5:
                cout << "Firing an employee..." << endl;
                fireEmployee(DB); // Function to remove an employee
                break;

            case 0:
                cout << "Logging out. Goodbye!" << endl;
                return;

            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }
}


    public:
void register_user(sqlite3* DB, int employee_id, string key = "") {
    User newUser;
    int choice;

    if (key == admin_key) {
        cout << "1. Admin." << endl;
        cout << "2. Inventory Manager." << endl;
        cout << "3. Service Agent." << endl;
        cout << "Who are you trying to register: ";
        cin >> choice;

        if (choice == 1)
            newUser.user_role = "Admin";
        else if (choice == 2)
            newUser.user_role = "Inventory Manager";
        else if (choice == 3)
            newUser.user_role = "Service Agent";
    } else {
        newUser.user_role = "Customer"; // Default role
    }

    cout << endl << "Register New User" << endl;
    cout << "--------------------" << endl;
    cout << "Enter first name: ";
    cin.ignore();
    getline(cin, newUser.first_name);
    cout << "Enter last name: ";
    getline(cin, newUser.last_name);
    cout << "Enter email: ";
    getline(cin, newUser.email);
    if (newUser.user_role != "Customer") {
        cout << "Enter password: ";
        getline(cin, newUser.password);
    }
    cout << "Enter phone number: ";
    getline(cin, newUser.phone);
    cout << "Enter address: ";
    getline(cin, newUser.address);

    string sql = "INSERT INTO Users(first_name, last_name, email, password, phone, address, role) VALUES('" +
                 newUser.first_name + "', '" + newUser.last_name + "', '" + newUser.email + "', '" +
                 newUser.password + "', '" + newUser.phone + "', '" + newUser.address + "', '" + 
                 newUser.user_role + "');";

    int rc = sqlite3_exec(DB, sql.c_str(), nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        cerr << "Error inserting data: " << sqlite3_errmsg(DB) << endl;
    } else {
        cout << "User registered successfully!" << endl;

        if (newUser.user_role == "Customer") {
            // Retrieve the user ID of the newly registered user
            string userIdQuery = "SELECT user_id FROM Users WHERE email = '" + newUser.email + "';";
            sqlite3_stmt* stmt;

            rc = sqlite3_prepare_v2(DB, userIdQuery.c_str(), -1, &stmt, nullptr);
            if (rc == SQLITE_OK && sqlite3_step(stmt) == SQLITE_ROW) {
                int newUserId = sqlite3_column_int(stmt, 0);
                sqlite3_finalize(stmt);

                // Pass the user ID to the makeReservation function
                makeReservation(DB, employee_id, newUserId);
            } else {
                cerr << "Error fetching user ID: " << sqlite3_errmsg(DB) << endl;
            }
        }
    }
}


void login(sqlite3* DB) {
    string email, password;

    cout << "Login" << endl;
    cout << "--------------------" << endl;
    cout << "Enter email: ";
    cin >> email;
    cin.ignore();
    cout << "Enter password: ";
    getline(cin, password);

    // Build SQL query with direct string concatenation
    string sql = "SELECT user_id, role FROM users WHERE email = '" + email + "' AND password = '" + password + "';";

    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    // Execute the query
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        int user_id = sqlite3_column_int(stmt, 0);
        string role = (char*)(sqlite3_column_text(stmt, 1));

        if (role == "Admin") {
            cout << "Welcome, " << role << "!" << endl;
            sqlite3_finalize(stmt);
            adminMenu(DB, user_id);
        } else if (role == "Inventory Manager") {
            cout << "Welcome, " << role << "!" << endl;
            sqlite3_finalize(stmt);
            inventoryManagerMenu(DB);
        } else if (role == "Service Agent") {
            cout << "Welcome, " << role << "!" << endl;
            sqlite3_finalize(stmt);
            agentMenu(DB, user_id);
        } else {
            cerr << "Unexpected role: " << role << endl;
            sqlite3_finalize(stmt);
        }
    } else {
        cout << "Invalid email or password!" << endl;
        sqlite3_finalize(stmt);
    }
}

void viewCars(sqlite3 *DB, bool isManager = false) {
    int choice = -1;
    string status = "Available";
    string query = "SELECT * FROM cars WHERE status = '" + status + "';";

    do {
        cout << "======================Cars======================" << endl;

        if (isManager) {
            cout << "1. Available Cars." << endl;
            cout << "2. Cars Under Maintainance." << endl;
            cout << "3. All Cars." << endl;
            cout << "4. Rented Cars." << endl;
            cout << "0. Back." << endl;
            cout << "Enter your choice: ";
            cin >> choice;

            if (choice == 0) {
                return;
            } else if (choice == 1) {
                status = "Available";
                query = "SELECT * FROM cars WHERE status = '" + status + "';";
            } else if (choice == 2) {
                status = "Under Maintainance";
                query = "SELECT * FROM cars WHERE status = '" + status + "';";
            } else if (choice == 3) {
                query = "SELECT * FROM cars";
            } else if (choice == 4) {
                status = "Rented";
                query = "SELECT * FROM cars WHERE status = '" + status + "';";
            } else {
                cout << "Invalid choice. Please try again." << endl;
                continue;
            }
        }

        sqlite3_stmt *stmt;
        int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, NULL);

        if (rc != SQLITE_OK) {
            cerr << "Error: " << sqlite3_errmsg(DB) << endl;
            return;
        }

        bool car_exist = false;

        while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
            car_exist = true;
            int car_id = sqlite3_column_int(stmt, 0);
            string producer = (char*)sqlite3_column_text(stmt, 1);
            string model = (char*)sqlite3_column_text(stmt, 2);
            string license_plate = (char*)sqlite3_column_text(stmt, 3);
            float daily_rent = sqlite3_column_double(stmt, 4);
            string status = (char*)sqlite3_column_text(stmt, 5);
            string description = (char*)sqlite3_column_text(stmt, 6);
            string created_at = (char*)sqlite3_column_text(stmt, 7);

            cout << "----------------------------------------------------" << endl;
            cout << "Car ID: " << car_id << endl;
            cout << "Produced by: " << producer << endl;
            cout << "Model: " << model << endl;
            cout << "License Plate: " << license_plate << endl;
            cout << "Daily Rent Price: " << daily_rent << endl;
            cout << "Status: " << status << endl;
            cout << "Description: " << description << endl;
            cout << "Created at: " << created_at << endl;
            cout << "----------------------------------------------------" << endl;
        }

        cout << "=====================End of Car List=====================" << endl;

        if (!car_exist) {
            cout << "Currently, there is no car in the database." << endl;
        }

        if (rc != SQLITE_DONE) {
            cerr << "Error: " << sqlite3_errmsg(DB) << endl;
        }

        sqlite3_finalize(stmt);

        if (isManager) {
            cout << "\nDo you want to view the car list again? (1 for Yes, 0 for No): ";
            cin >> choice;

            if (choice != 1) {
                break;
            }
        }
    } while (isManager);
}

};



void mainMenu(sqlite3* DB) {
    int choice = 0;
    A:
    cout << endl<<"=================================" << endl;
    cout << "        Main Menu        " << endl;
    cout << "=================================" << endl;
    cout << "1. Login" << endl;
    cout << "2. View Available Cars" << endl;
    cout << "0. Exit" << endl;
    cout << "=================================" << endl;
    cout << "Enter your choice: ";
    cin >> choice;

        Users user;
        switch (choice) {
            case 0:
                cout << "Exiting the system. Goodbye!" << endl;
                return;
                break;
            case 1:
                user.login(DB);
                goto A;
                break;
            case 2:
                cout << "Viewing Available cars..." << endl;
                user.viewCars(DB);
                goto A;
                break;
            default:
                cout << "Invalid choice. Try again!" << endl;
                goto A;
        }

}


int main() {
    cout << "The program Has been updated again." << endl;
    sqlite3* DB;
    int exit = sqlite3_open("test.db", &DB);
    if (exit) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(DB) << std::endl;
        return -1;
    } else {
        std::cout << "Opened database successfully!" << std::endl;
    }

    sqlite3_exec(DB, "PRAGMA foreign_keys = ON;", 0, 0, 0);

    const char* createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password TEXT,
            phone TEXT UNIQUE NOT NULL,
            address TEXT NOT NULL,
            role TEXT CHECK(role IN ('Admin', 'Inventory Manager', 'Service Agent', 'Customer')),
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char* createCarsTable = R"(
        CREATE TABLE IF NOT EXISTS cars (
            car_id INTEGER PRIMARY KEY AUTOINCREMENT,
            produced_by TEXT NOT NULL,
            model TEXT NOT NULL,
            license_plate TEXT UNIQUE NOT NULL,
            daily_rental_price REAL NOT NULL,
            status TEXT CHECK(status IN ('Available', 'Rented', 'Under Maintainance')),
            description TEXT,
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP
        );
    )";

    const char* createReservationsTable = R"(
        CREATE TABLE IF NOT EXISTS reservations (
            reservation_id INTEGER PRIMARY KEY AUTOINCREMENT,
            user_id INTEGER NOT NULL,
            car_id INTEGER NOT NULL,
            start_date TEXT DEFAULT CURRENT_TIMESTAMP,
            end_date TEXT NOT NULL,
            total_price REAL NOT NULL,
            extra_charge REAL DEFAULT '0.0',
            status TEXT CHECK(status IN ('Pending', 'Completed', 'Canceled')) DEFAULT 'Pending',
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(user_id) ON DELETE CASCADE,
            FOREIGN KEY (car_id) REFERENCES cars(car_id) ON DELETE CASCADE
        );
    )";

    const char* createPaymentsTable = R"(
        CREATE TABLE IF NOT EXISTS payments (
            payment_id INTEGER PRIMARY KEY AUTOINCREMENT,
            reservation_id INTEGER NOT NULL,
            amount REAL NOT NULL,
            payment_method TEXT CHECK(payment_method IN ('Credit Card', 'Debit Card', 'PayPal', 'Cash', 'TeleBirr', 'CBE')) NOT NULL,
            status TEXT CHECK(status IN ('Pending', 'Success', 'Failed')) DEFAULT 'Pending',
            payment_date TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (reservation_id) REFERENCES reservations(reservation_id) ON DELETE CASCADE
        );
    )";

    // Execute each table creation query
    char* errorMessage;
    const char* tableQueries[] = {createUsersTable, createCarsTable, createReservationsTable, createPaymentsTable};
    for (const char* query : tableQueries) {
        if (sqlite3_exec(DB, query, 0, 0, &errorMessage) != SQLITE_OK) {
            cerr << "Error creating table: " << errorMessage << std::endl;
            sqlite3_free(errorMessage);
        } else {
            cout << "Table created successfully!" << std::endl;
        }
    }

const char* insertDummyData = R"(
    -- Insert Users
    INSERT INTO users (first_name, last_name, email, password, phone, address, role)
    VALUES
        ('Admin', 'User', 'admin@example.com', 'admin123', '1234567890', '123 Admin St', 'Admin'),
        ('Inventory', 'Manager', 'inventory@example.com', 'inventory123', '1234567891', '456 Inventory Ave', 'Inventory Manager'),
        ('Service', 'Agent', 'service@example.com', 'service123', '1234567892', '789 Service Blvd', 'Service Agent'),
        ('John', 'Doe', 'john.doe@example.com', 'customer123', '1234567893', '321 Customer Lane', 'Customer');

    -- Insert Cars
    INSERT INTO cars (produced_by, model, license_plate, daily_rental_price, status, description)
    VALUES
        ('Toyota', 'Corolla', 'ABC123', 50.0, 'Available', 'A reliable sedan.'),
        ('Honda', 'Civic', 'XYZ789', 55.0, 'Rented', 'A fuel-efficient compact car.'),
        ('Ford', 'Focus', 'DEF456', 60.0, 'Under Maintainance', 'A spacious compact car.');

    -- Insert Reservations
    INSERT INTO reservations (user_id, car_id, start_date, end_date, total_price, extra_charge, status)
    VALUES
        (4, 1, '2024-12-01', '2024-12-05', 250.0, 0.0, 'Completed'),
        (4, 2, '2024-12-10', '2024-12-15', 275.0, 10.0, 'Pending');

    -- Insert Payments
    INSERT INTO payments (reservation_id, amount, payment_method, status, payment_date)
    VALUES
        (1, 250.0, 'Credit Card', 'Success', '2024-12-06'),
        (2, 285.0, 'Cash', 'Pending', '2024-12-18');
)";

int rc = sqlite3_exec(DB, insertDummyData, nullptr, nullptr, nullptr);
if (rc != SQLITE_OK) {
    cerr << "Failed to insert dummy data: " << sqlite3_errmsg(DB) << endl;
} else {
    cout << "Dummy data inserted successfully!" << endl;
}

    mainMenu(DB);
    sqlite3_close(DB);
    return 0;
}
