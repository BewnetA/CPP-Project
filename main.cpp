#include <iostream>
#include <cstdlib> // used for system("cls");
#include "sqlite3.h"

using namespace std;

string admin_key = "admin";

struct User{
    string first_name, last_name, email, password, phone, address;
    int is_admin;

    User(){
        is_admin = 0;
    }
};

class Users{

void addNewCar(sqlite3 *DB) {
    system("cls");
    string produced_by, model, license_plate, status = "available";
    float daily_rental_price;

    cout << "Enter Car Manufacturer (Produced By): ";
    cin >> produced_by;
    cout << "Enter Car Model: ";
    cin >> model;
    cout << "Enter License Plate: ";
    cin >> license_plate;
    cout << "Enter Daily Rental Price: ";
    cin >> daily_rental_price;

    string query = "INSERT INTO cars (produced_by, model, license_plate, daily_rental_price, status) VALUES ('" +
                   produced_by + "', '" + model + "', '" + license_plate + "', " + to_string(daily_rental_price) + ", '" + status + "');";

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
    system("cls");
    int car_id;
    string produced_by, model, license_plate, status;
    float daily_rental_price;

    cout << "Enter Car ID to Update: ";
    cin >> car_id;

    // Check if the car exists
    string checkQuery = "SELECT * FROM cars WHERE car_id = " + to_string(car_id) + ";";
    sqlite3_stmt *stmt;
    int rc = sqlite3_prepare_v2(DB, checkQuery.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Error checking car existence: " << sqlite3_errmsg(DB) << endl;
        sqlite3_finalize(stmt);
        return;
    }

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW) {
        cerr << "Car with ID " << car_id << " does not exist." << endl;
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);

    // Proceed with update if the car exists
    cout << "Enter New Manufacturer (Produced By): ";
    cin >> produced_by;
    cout << "Enter New Model: ";
    cin >> model;
    cout << "Enter New License Plate: ";
    cin >> license_plate;
    cout << "Enter New Daily Rental Price: ";
    cin >> daily_rental_price;
    cout << "Enter New Status (available/rented): ";
    cin >> status;

    string query = "UPDATE cars SET produced_by = '" + produced_by +
                   "', model = '" + model +
                   "', license_plate = '" + license_plate +
                   "', daily_rental_price = " + to_string(daily_rental_price) +
                   ", status = '" + status +
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
    system("cls");
    int car_id;
    cout << "Enter Car ID to Remove: ";
    cin >> car_id;

    string query = "DELETE FROM cars WHERE car_id = " + to_string(car_id) + ";";

    char *errMsg = 0;
    int rc = sqlite3_exec(DB, query.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK) {
        cerr << "Error removing car: " << errMsg << endl;
        sqlite3_free(errMsg);
    } else {
        cout << "Car removed successfully!" << endl;
    }
}



void makeReservation(sqlite3* DB, int user_id) {
    int car_id;
    string end_date;
    cout << "Enter the car ID: ";
    cin >> car_id;
    cout << "When will you return the car? (yyyy-mm-dd): ";
    cin >> end_date;

    // Query to check availability of car 
    string carQuery = "SELECT daily_rental_price, status FROM cars WHERE car_id = " + to_string(car_id) + ";";
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(DB, carQuery.c_str(), -1, &stmt, NULL);
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
    string status = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
    sqlite3_finalize(stmt);

    if (status != "available") {
        cout << "Car is not available for reservation." << endl;
        return;
    }

    // Calculate total price (assuming a fixed daily rate)
    float total_price = daily_price * 7; // Replace with proper date calculation logic

    // Adding reservation
    string sql = "INSERT INTO reservations (user_id, car_id, end_date, total_price) VALUES (?, ?, ?, ?);";
    sqlite3_stmt* insert_stmt;
    rc = sqlite3_prepare_v2(DB, sql.c_str(), -1, &insert_stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare insert statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    sqlite3_bind_int(insert_stmt, 1, user_id);
    sqlite3_bind_int(insert_stmt, 2, car_id);
    sqlite3_bind_text(insert_stmt, 3, end_date.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(insert_stmt, 4, total_price);

    rc = sqlite3_step(insert_stmt);
    if (rc != SQLITE_DONE) {
        cerr << "Error inserting reservation: " << sqlite3_errmsg(DB) << endl;
    } else {
        cout << "Reservation made successfully!" << endl;
    }
sqlite3_finalize(insert_stmt);
    string query_to_update_car = "UPDATE cars SET status = 'unavailable' WHERE car_id = " + to_string(car_id) + " ;";
    
    rc = sqlite3_exec(DB, query_to_update_car.c_str(), 0, 0, nullptr);
    if(rc != SQLITE_OK){
        cerr << "Error updating car status: " << sqlite3_errmsg(DB) << endl;
    }else{
        cout << "Car status updated successfully!" << endl;
    }

}

void cancelReservation(sqlite3 *DB, int id){
    int reserve_id, choice;
    float penality = 100.0;
    cout << "Enter the Reservation ID: ";
    cin >> reserve_id;
    if (reserve_id <= 0){
        cout << endl << "You have entered invalid Reservation ID" << endl;
        return;
    }
    string query = "SELECT * FROM reservations WHERE reservation_id = " + to_string(reserve_id) + " ;";

    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(DB, query.c_str(),-1, &stmt, NULL);
    if (rc != SQLITE_OK){
        cerr << "Error preparing: " << sqlite3_errmsg(DB) << endl;
        sqlite3_finalize(stmt);
        return;
    }
    rc = sqlite3_step(stmt);
    if (rc != SQLITE_ROW){
        cerr << "Error preparing: " << sqlite3_errmsg(DB) << endl;
        
        sqlite3_finalize(stmt);
        return;
    }

    int user_id = sqlite3_column_int(stmt, 1);
    int car_id = sqlite3_column_int(stmt, 2);
    //string end_date = (char *)sqlite3_column_text(stmt, 4);
    float total_price = sqlite3_column_double(stmt,5); 

    if (user_id != id) {
        cerr << "This reservation doesn`t belong to you sir." << endl;
        
        sqlite3_finalize(stmt);
        return;
    }
    sqlite3_finalize(stmt);
    int current_date = 100, end_date = 100;
// current_date is accessed from the machine
// end_date is accessed from the reservations table
    if(current_date > end_date){
        penality *= (current_date - end_date);
        total_price += penality;
    }
    else if (current_date < end_date) {
        string rentalQuery = "SELECT daily_rent FROM cars WHERE car_id = " + to_string(car_id) + " ;";
        
        rc = sqlite3_prepare_v2(DB, rentalQuery.c_str(), -1, &stmt, NULL);
        if (rc != SQLITE_OK) {
            cerr << "Error preparing rental query: " << sqlite3_errmsg(DB) << endl;
            sqlite3_finalize(stmt);
            return;
        }
        rc = sqlite3_step(stmt);
        if (rc != SQLITE_ROW) {
            cerr << "Error fetching rental price: " << sqlite3_errmsg(DB) << endl;
            sqlite3_finalize(stmt);
            return;
        }

        float daily_rent = sqlite3_column_double(stmt, 0);
        total_price -= (end_date - current_date) * daily_rent; 
        sqlite3_finalize(stmt);
    }
    cout << endl << "Send total rent of: " << total_price << ": ";
    cin >> total_price;

    string delQuery = "DELETE FROM reservations WHERE reservation_id = " + to_string(reserve_id) +" ;";

    rc = sqlite3_exec(DB, delQuery.c_str(),0,0,nullptr);

    if (rc != SQLITE_OK){
        cerr << "Error: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    string query_to_update_car = "UPDATE cars SET status = 'available' WHERE car_id = " + to_string(car_id) + " ;";
    
    rc = sqlite3_exec(DB, query_to_update_car.c_str(), 0, 0, nullptr);
    if(rc != SQLITE_OK){
        cerr << "Error updating car status: " << sqlite3_errmsg(DB) << endl;
    }else{
        cout << "Car status updated successfully!" << endl;
    }

    cout << "===================Reservation Cancled successfully =====================" << endl;

}

void viewReservationDetails(sqlite3 *DB, int id) {
    cout << "================== Reservation Details ====================" << endl;

    string query = "SELECT * FROM reservations WHERE chat_id = " + to_string(id) + " ;";
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(DB, query.c_str(), -1, &stmt, NULL);
    if (rc != SQLITE_OK) {
        cerr << "Error preparing statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    bool hasResults = false; 

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        hasResults = true; 

        int reserve_id = sqlite3_column_int(stmt, 0);
        int car_id = sqlite3_column_int(stmt, 2);

        string query = "SELECT * FROM cars WHERE car_id = "+ to_string(car_id) + " ;";
        
        sqlite3_stmt *stmt_car;

        int rc = sqlite3_prepare_v2(DB, query.c_str(),-1,&stmt_car,NULL);

        if (rc != SQLITE_OK){
            cerr << "Error: " << sqlite3_errmsg(DB) << endl;
            return;
        }
        rc = sqlite3_step(stmt_car);
        if (rc!= SQLITE_ROW) {
            cerr << "Error fetching car details: " << sqlite3_errmsg(DB) << endl;
            return;
        }
        string producer = (char *)sqlite3_column_text(stmt_car, 1);
        string model = (char *)sqlite3_column_text(stmt_car, 2);
        string license_plate = (char *)sqlite3_column_text(stmt_car, 3);

        sqlite3_finalize(stmt_car);

        const unsigned char *start_date_text = sqlite3_column_text(stmt, 3);
        const unsigned char *end_date_text = sqlite3_column_text(stmt, 4);
        const unsigned char *status_text = sqlite3_column_text(stmt, 6);

        string start_date = start_date_text ? reinterpret_cast<const char *>(start_date_text) : "NULL";
        string end_date = end_date_text ? reinterpret_cast<const char *>(end_date_text) : "NULL";
        string status = status_text ? reinterpret_cast<const char *>(status_text) : "NULL";

        float total_price = sqlite3_column_double(stmt, 5);

        cout << "----------------------------------------------------" << endl;
        cout << "Reservation ID: " << reserve_id << endl;
        cout << "Car ID: " << car_id << endl;
        cout << "Produced by: " << producer << endl;
        cout << "Model: " << model << endl;
        cout << "Lisence Plate: " << license_plate << endl;
        cout << "Start Date: " << start_date << endl;
        cout << "End Date: " << end_date << endl;
        cout << "Total Price: " << total_price << endl;
        cout << "Status: " << status << endl;
        cout << "----------------------------------------------------" << endl;
    }

    if (!hasResults) {
        cout << "No reservations found for the given ID." << endl;
    } else if (rc != SQLITE_DONE) {
        cerr << "Error while fetching reservations: " << sqlite3_errmsg(DB) << endl;
    }

    sqlite3_finalize(stmt);
}


void user_menu(sqlite3 *DB, int id){
    int choice = 0;
        cout << endl << "User Menu" << endl;
        cout << "--------------------" << endl;
        cout << "1. Make a Reservation" << endl;
        cout << "2. Cancel a Reservation" << endl;
        cout << "3. View Reservation Details" << endl;
        cout << "0. Log Out" << endl;
        cin >> choice;
        switch(choice){
            case 1:
                makeReservation(DB, id);
                break;
            case 2:
                cancelReservation(DB, id);
                break;
            case 3:
                viewReservationDetails(DB, id);
                break;
            case 0:
                return;
            default:
                cout << "Invalid choice! Please try again." << endl;
        }
    }

void displayAdminMenu(sqlite3* DB) {
    int choice = 0;
    A:
    cout << "=================================" << endl;
    cout << "          Admin Menu             " << endl;
    cout << "=================================" << endl;
    cout << "1. Add a New Car" << endl;
    cout << "2. Update Car Details" << endl;
    cout << "3. Remove a Car" << endl;
    cout << "4. View Car Status" << endl;
    cout << "5. Generate Reservation Reports" << endl;
    cout << "0. Return to Main Menu" << endl;
    cout << "=================================" << endl;
    cout << "Please select an option: ";
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
                cout << "Viewing car status..." << endl;
                viewCars(DB);
                goto A;
                break;
            case 5:
                cout << "Generating reservation reports..." << endl;
                // Add code to generate reports
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


    public:
bool register_user(sqlite3* DB){
    // implement needed to show the id of the user
            User newUser;
            string key;
            cout << endl << "Register New User" << endl;
            cout << "--------------------" << endl;
            cout << "Enter first name: ";
            cin.ignore();
            getline(cin, newUser.first_name);
            cout << "Enter last name: ";
            getline(cin, newUser.last_name);
            cout << "Enter email: ";
            getline(cin, newUser.email);
            cout << "Enter password: ";
            getline(cin, newUser.password);
            cout << "Enter phone number: ";
            getline(cin, newUser.phone);
            cout << "Enter address: ";
            getline(cin, newUser.address);
            cout << "Are you an admin? Enter(Admin Key/0): ";
            getline(cin, key);

            if (key == admin_key){
                newUser.is_admin = 1;
            }

            char* errMsg = nullptr;
            const char* sql = "INSERT INTO Users(first_name, last_name, email, password, phone, address, is_admin) VALUES(?,?,?,?,?,?,?);";
            sqlite3_stmt* stmt;
            int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, 0);
            if (rc != SQLITE_OK) {
                cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
                return false;
            }

            sqlite3_bind_text(stmt, 1, newUser.first_name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 2, newUser.last_name.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 3, newUser.email.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 4, newUser.password.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 5, newUser.phone.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_text(stmt, 6, newUser.address.c_str(), -1, SQLITE_STATIC);
            sqlite3_bind_int(stmt, 7, newUser.is_admin);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                cerr << "Error inserting data: " << sqlite3_errmsg(DB) << endl;
                sqlite3_finalize(stmt);
                return false;
            } else {
                cout << "User registered successfully!" << endl;
                sqlite3_finalize(stmt);
                return true;
            }

        }

void login(sqlite3* DB) {
    string email, password;
    
    cout << "Login" << endl;
    cout << "--------------------" << endl;
    cout << "Enter email: ";
    cin.ignore();
    getline(cin, email);
    cout << "Enter password: ";
    getline(cin, password);

    const char* sql = "SELECT * FROM Users WHERE email = ? AND password = ?";
    
    sqlite3_stmt* stmt;
    int rc = sqlite3_prepare_v2(DB, sql, -1, &stmt, 0);
    if (rc != SQLITE_OK) {
        cerr << "Failed to prepare statement: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    // Bind values
    sqlite3_bind_text(stmt, 1, email.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, password.c_str(), -1, SQLITE_STATIC);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        cout << "Login successful!" << endl;
        int is_admin = sqlite3_column_int(stmt, 7); // check the admin status
        int user_id = sqlite3_column_int(stmt,0);
        if (is_admin == 1) {
            cout << "Welcome, Admin!" << endl;
            sqlite3_finalize(stmt);
            displayAdminMenu(DB);
            return;
        } else {
            sqlite3_finalize(stmt);
           user_menu(DB, user_id);
           return;
        }

    } else {
        cout << "Invalid email or password!" << endl;
    }

    
    return;
}

void viewCars(sqlite3 *DB){
    cout << "======================Cars======================" << endl;
    string query = "SELECT * FROM cars";
    
    sqlite3_stmt *stmt;

    int rc = sqlite3_prepare_v2(DB, query.c_str(),-1,&stmt,NULL);

    if (rc != SQLITE_OK){
        cerr << "Error: " << sqlite3_errmsg(DB) << endl;
        return;
    }

    bool car_exist = false;

    while((rc = sqlite3_step(stmt)) == SQLITE_ROW){
        car_exist = true;
        int car_id = sqlite3_column_int(stmt,0);
        string producer = (char*)sqlite3_column_text(stmt, 1);
        string model = (char*)sqlite3_column_text(stmt, 2);
        string license_plate = (char*)sqlite3_column_text(stmt, 3);
        float daily_rent = sqlite3_column_double(stmt, 4);
        string status = (char*)sqlite3_column_text(stmt, 5);
        string created_at = (char*)sqlite3_column_text(stmt, 6);

        cout << "----------------------------------------------------" << endl;
        cout << "car ID: " << car_id << endl;
        cout << "Produced by: " << producer << endl;
        cout << "Model: " << model << endl;
        cout << "Lisence Plate: " << license_plate << endl;
        cout << "Daily Rent Price: " << daily_rent << endl;
        cout << "Status: " << status << endl;
        cout << "Created at: " << created_at << endl;
        cout << "----------------------------------------------------" << endl;
        
    }

    if (!car_exist){
        cout << "Currently their is no car in the database" << endl;
    }
    if (rc != SQLITE_DONE){
        cerr << "Error: " << sqlite3_errmsg(DB) << endl;
    }

    sqlite3_finalize(stmt);
}

};



void mainMenu(sqlite3* DB) {
    system("cls");
    int choice = 0;
    A:
    cout << endl<<"=================================" << endl;
    cout << "       Car Rental System         " << endl;
    cout << "=================================" << endl;
    cout << "1. Register New Customer" << endl;
    cout << "2. Login" << endl;
    cout << "3. View Available Cars" << endl;
    cout << "0. Exit" << endl;
    cout << "=================================" << endl;
    cout << "Please select an option: ";
    cin >> choice;

        Users user;
        switch (choice) {
            case 0:
                cout << "Exiting the system. Goodbye!" << endl;
                return;
                break;
            case 1:
                cout << "Registering a new user..." << endl;
                user.register_user(DB);
                goto A;
                break;
            case 2:
                cout << "Logging in..." << endl;
                user.login(DB);
                goto A;
                break;
            case 3:
                cout << "Viewing available cars..." << endl;
                user.viewCars(DB);
                goto A;
                break;
            default:
                cout << "Invalid choice. Try again!" << endl;
                goto A;
        }
        system("cls");
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

    // Enable foreign key enforcement
    sqlite3_exec(DB, "PRAGMA foreign_keys = ON;", 0, 0, 0);

    // Create tables one by one for better error handling
    const char* createUsersTable = R"(
        CREATE TABLE IF NOT EXISTS users (
            user_id INTEGER PRIMARY KEY AUTOINCREMENT,
            first_name TEXT NOT NULL,
            last_name TEXT NOT NULL,
            email TEXT UNIQUE NOT NULL,
            password TEXT NOT NULL,
            phone TEXT UNIQUE NOT NULL,
            address TEXT NOT NULL,
            is_admin INTEGER DEFAULT 0,
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
            status TEXT DEFAULT 'available',
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
            status TEXT DEFAULT 'pending',
            created_at TEXT DEFAULT CURRENT_TIMESTAMP,
            updated_at TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (user_id) REFERENCES users(user_id),
            FOREIGN KEY (car_id) REFERENCES cars(car_id)
        );
    )";

    const char* createPaymentsTable = R"(
        CREATE TABLE IF NOT EXISTS payments (
            payment_id INTEGER PRIMARY KEY AUTOINCREMENT,
            reservation_id INTEGER NOT NULL,
            amount REAL NOT NULL,
            payment_method TEXT NOT NULL,
            payment_date TEXT DEFAULT CURRENT_TIMESTAMP,
            FOREIGN KEY (reservation_id) REFERENCES reservations(reservation_id)
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

    // const char *userInsertQuery = R"(
    //     INSERT INTO users (first_name, last_name, email, password, phone, address, is_admin) VALUES
    //     ('John', 'Doe', 'john.doe@example.com', 'password123', '123-456-7890', '123 Elm St, Springfield', 0),
    //     ('Jane', 'Smith', 'jane.smith@example.com', 'passw0rd!', '987-654-3210', '456 Oak St, Springfield', 0),
    //     ('Alice', 'Brown', 'alice.brown@example.com', 'alicepass', '555-111-2222', '789 Maple St, Springfield', 0),
    //     ('Bob', 'Johnson', 'bob.johnson@example.com', 'bobsecure', '444-222-3333', '321 Pine St, Springfield', 0),
    //     ('Emily', 'Davis', 'emily.davis@example.com', 'emilypass', '666-333-4444', '654 Cedar St, Springfield', 0);
    // )";

    // const char *carInsertQuery = R"(
    //     INSERT INTO cars (produced_by, model, license_plate, daily_rental_price, status) VALUES
    //     ('Toyota', 'Corolla', 'ABC-123', 50.0, 'available'),
    //     ('Honda', 'Civic', 'XYZ-987', 55.0, 'available'),
    //     ('Ford', 'Focus', 'LMN-456', 60.0, 'available'),
    //     ('Chevrolet', 'Malibu', 'QWE-789', 65.0, 'available'),
    //     ('Nissan', 'Altima', 'RTY-654', 58.0, 'available');
    // )";

    // char *errMsg = 0;

    // // Insert Users
    // int rc = sqlite3_exec(DB, userInsertQuery, 0, 0, &errMsg);
    // if (rc != SQLITE_OK) {
    //     cerr << "Error inserting users: " << errMsg << endl;
    //     sqlite3_free(errMsg);
    // } else {
    //     cout << "Dummy users added successfully!" << endl;
    // }

    // // Insert Cars
    // rc = sqlite3_exec(DB, carInsertQuery, 0, 0, &errMsg);
    // if (rc != SQLITE_OK) {
    //     cerr << "Error inserting cars: " << errMsg << endl;
    //     sqlite3_free(errMsg);
    // } else {
    //     cout << "Dummy cars added successfully!" << endl;
    // }

    mainMenu(DB);
    sqlite3_close(DB);
    return 0;
}
