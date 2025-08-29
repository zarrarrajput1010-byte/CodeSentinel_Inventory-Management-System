#include <iostream>
#include <vector>
#include <fstream>
#include <string>
#include <iomanip>
#include <algorithm>
#include <limits>

using namespace std;

class Product {
private:
    int id;
    string name;
    double price;
    int quantity;
    
public:


    Product() : id(0), name(""), price(0.0), quantity(0) {}
    

    Product(int productId, string productName, double productPrice, int productQuantity)
        : id(productId), name(productName), price(productPrice), quantity(productQuantity) {}
        
    int getId() const { return id; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    int getQuantity() const { return quantity; }
    

    void setId(int productId) { id = productId; }
    void setName(const string& productName) { name = productName; }
    void setPrice(double productPrice) { price = productPrice; }
    void setQuantity(int productQuantity) { quantity = productQuantity; }
    
    void displayProduct() const {
        cout << left << setw(5) << id
             << setw(20) << name
             << setw(10) << fixed << setprecision(2) << price
             << setw(10) << quantity << endl;
    }
    
    string toFileString() const {
        return to_string(id) + "," + name + "," + to_string(price) + "," + to_string(quantity);
    }
    

    bool fromFileString(const string& line) {
        try {
            size_t pos1 = line.find(',');
            size_t pos2 = line.find(',', pos1 + 1);
            size_t pos3 = line.find(',', pos2 + 1);
            
            if (pos1 == string::npos || pos2 == string::npos || pos3 == string::npos) {
                return false; 
            }
            
            id = stoi(line.substr(0, pos1));
            name = line.substr(pos1 + 1, pos2 - pos1 - 1);
            price = stod(line.substr(pos2 + 1, pos3 - pos2 - 1));
            quantity = stoi(line.substr(pos3 + 1));
            
            return true;
        }
        catch (const exception& e) {
            return false; 
        }
    }
};

class InventoryManager {
private:
    vector<Product> products;
    string filename;

    void clearInputBuffer() {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
    }

    int getValidInteger(const string& prompt) {
        int value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                clearInputBuffer();
                return value;
            } else {
                cout << "Invalid input! Please enter a valid integer." << endl;
                clearInputBuffer();
            }
        }
    }

    double getValidDouble(const string& prompt) {
        double value;
        while (true) {
            cout << prompt;
            if (cin >> value) {
                clearInputBuffer();
                return value;
            } else {
                cout << "Invalid input! Please enter a valid number." << endl;
                clearInputBuffer();
            }
        }
    }
    
public:
    InventoryManager(const string& file = "inventory.txt") : filename(file) {
        loadFromFile();
    }
    
    ~InventoryManager() {
        saveToFile(false); 
    }

    void addProduct() {
        cout << "\n=== ADD NEW PRODUCT ===" << endl;
        
        int id = getValidInteger("Enter Product ID: ");
        
        if (findProductById(id) != nullptr) {
            cout << "Error: Product with ID " << id << " already exists!" << endl;
            return;
        }
        
        cout << "Enter Product Name: ";
        string name;
        getline(cin, name);
        
        if (name.empty()) {
            cout << "Error: Product name cannot be empty!" << endl;
            return;
        }
        
        double price = getValidDouble("Enter Product Price: ");
        if (price < 0) {
            cout << "Error: Price cannot be negative!" << endl;
            return;
        }
        
        int quantity = getValidInteger("Enter Product Quantity: ");
        if (quantity < 0) {
            cout << "Error: Quantity cannot be negative!" << endl;
            return;
        }
        
        Product newProduct(id, name, price, quantity);
        products.push_back(newProduct);
        
        cout << "Product added successfully!" << endl;
    }

    void viewProducts() {
        cout << "\n=== PRODUCT INVENTORY ===" << endl;
        
        if (products.empty()) {
            cout << "No products in inventory." << endl;
            return;
        }
        
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Price"
             << setw(10) << "Quantity" << endl;
        cout << string(45, '-') << endl;
        
        for (const auto& product : products) {
            product.displayProduct();
        }
        
        cout << "\nTotal Products: " << products.size() << endl;
    }

    void searchProduct() {
        cout << "\n=== SEARCH PRODUCT ===" << endl;
        int id = getValidInteger("Enter Product ID to search: ");
        
        Product* product = findProductById(id);
        if (product != nullptr) {
            cout << "\nProduct Found:" << endl;
            cout << left << setw(5) << "ID"
                 << setw(20) << "Name"
                 << setw(10) << "Price"
                 << setw(10) << "Quantity" << endl;
            cout << string(45, '-') << endl;
            product->displayProduct();
        }
        else {
            cout << "Product with ID " << id << " not found!" << endl;
        }
    }

    void updateQuantity() {
        cout << "\n=== UPDATE PRODUCT QUANTITY ===" << endl;
        int id = getValidInteger("Enter Product ID: ");
        
        Product* product = findProductById(id);
        if (product != nullptr) {
            cout << "Current quantity: " << product->getQuantity() << endl;
            int newQuantity = getValidInteger("Enter new quantity: ");
            
            if (newQuantity < 0) {
                cout << "Error: Quantity cannot be negative!" << endl;
                return;
            }
            
            product->setQuantity(newQuantity);
            cout << "Quantity updated successfully!" << endl;
        }
        else {
            cout << "Product with ID " << id << " not found!" << endl;
        }
    }

    void saveToFile(bool showMessage = true) {
        ofstream file(filename);
        if (file.is_open()) {
            for (const auto& product : products) {
                file << product.toFileString() << endl;
            }
            file.close();
            if (showMessage) {
                cout << "Data saved to file successfully!" << endl;
            }
        }
        else {
            if (showMessage) {
                cout << "Error: Unable to save data to file!" << endl;
            }
        }
    }

    void loadFromFile() {
        ifstream file(filename);
        if (file.is_open()) {
            string line;
            products.clear();
            int loadedCount = 0;
            int errorCount = 0;
            
            while (getline(file, line)) {
                if (!line.empty()) {
                    Product product;
                    if (product.fromFileString(line)) {
                        products.push_back(product);
                        loadedCount++;
                    } else {
                        errorCount++;
                    }
                }
            }
            file.close();
            cout << "Data loaded from file successfully! (" << loadedCount << " products)" << endl;
            if (errorCount > 0) {
                cout << "Warning: " << errorCount << " invalid entries were skipped." << endl;
            }
        }
        else {
            cout << "No existing inventory file found. Starting with empty inventory." << endl;
        }
    }

    Product* findProductById(int id) {
        for (auto& product : products) {
            if (product.getId() == id) {
                return &product;
            }
        }
        return nullptr;
    }
    
    void displayLowStock() {
        cout << "\n=== LOW STOCK ALERT ===" << endl;
        bool foundLowStock = false;
        
        cout << left << setw(5) << "ID"
             << setw(20) << "Name"
             << setw(10) << "Price"
             << setw(10) << "Quantity" << endl;
        cout << string(45, '-') << endl;
        
        for (const auto& product : products) {
            if (product.getQuantity() < 5) {
                product.displayProduct();
                foundLowStock = true;
            }
        }
        
        if (!foundLowStock) {
            cout << "No products with low stock." << endl;
        }
    }
};

void displayMenu() {
    cout << "\n===========================================" << endl;
    cout << "        INVENTORY MANAGEMENT SYSTEM       " << endl;
    cout << "===========================================" << endl;
    cout << "1. Add New Product" << endl;
    cout << "2. View All Products" << endl;
    cout << "3. Search Product by ID" << endl;
    cout << "4. Update Product Quantity" << endl;
    cout << "5. Save Data to File" << endl;
    cout << "6. Load Data from File" << endl;
    cout << "7. View Low Stock Products" << endl;
    cout << "8. Exit" << endl;
    cout << "===========================================" << endl;
    cout << "Enter your choice (1-8): ";
}

int getValidChoice() {
    int choice;
    while (true) {
        if (cin >> choice && choice >= 1 && choice <= 8) {
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            return choice;
        } else {
            cout << "Invalid choice! Please enter a number between 1 and 8: ";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
        }
    }
}

int main() {
    InventoryManager inventory;
    int choice;
    
    cout << "Welcome to Inventory Management System!" << endl;
    
    do {
        displayMenu();
        choice = getValidChoice();
        
        switch (choice) {
            case 1:
                inventory.addProduct();
                break;
            case 2: 
                inventory.viewProducts();
                break;
            case 3:
                inventory.searchProduct();
                break;
            case 4:
                inventory.updateQuantity();
                break;
            case 5:
                inventory.saveToFile();
                break;
            case 6:
                inventory.loadFromFile();
                break;
            case 7:
                inventory.displayLowStock();
                break;
            case 8:
                cout << "\nThank you for using Inventory Management System!" << endl;
                cout << "Data will be automatically saved." << endl;
                break;
        }
        
        if (choice != 8) {
            cout << "\nPress Enter to continue...";
            cin.get();
        }
    }
    while (choice != 8);
    
    return 0;
}