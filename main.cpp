// ============================================================
//  OrderFlow - Order Management System  ♡
//  C++17 | single file | CSV storage | console interface
//  Build: g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o OrderFlow.exe
// ============================================================
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <windows.h>
#include <io.h>
#endif
using namespace std;

// ----- Palette -------------------------------------------------
struct Palette { string cream, blush, rose, mocha, pearl, dim, bold, reset; } P;
bool g_color = false, g_vt = false, g_interactive = false;
string rgb(int r, int g, int b) {
    return "\x1b[38;2;" + to_string(r) + ";" + to_string(g) + ";" + to_string(b) + "m";
}
void initTheme() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (out != INVALID_HANDLE_VALUE && GetConsoleMode(out, &mode))
        g_vt = SetConsoleMode(out, mode | ENABLE_VIRTUAL_TERMINAL_PROCESSING) != 0;
    g_interactive = _isatty(_fileno(stdout)) != 0;
#endif
    g_color = g_vt && g_interactive && getenv("NO_COLOR") == nullptr;
    if (!g_color) return;
    P.cream = rgb(247, 237, 232);  // #F7EDE8 vanilla cream
    P.blush = rgb(235, 198, 210);  // #EBC6D2 blush pink
    P.rose  = rgb(217, 151, 175);  // #D997AF rose pink
    P.mocha = rgb(121, 87, 71);    // #795747 mocha brown
    P.pearl = rgb(255, 249, 245);  // #FFF9F5 pearl white
    P.dim   = rgb(170, 145, 135);
    P.bold  = "\x1b[1m";
    P.reset = "\x1b[0m";
}
string paint(const string& text, const string& style) {
    return (!g_color || style.empty()) ? text : style + text + P.reset;
}
void clearScreen() { if (g_interactive && g_vt) cout << "\x1b[2J\x1b[H"; }

// ----- Text helpers --------------------------------------------
string trim(const string& s) {
    size_t a = s.find_first_not_of(" \t\r\n");
    size_t b = s.find_last_not_of(" \t\r\n");
    return a == string::npos ? "" : s.substr(a, b - a + 1);
}
string lower(string s) {
    for (char& c : s) c = static_cast<char>(tolower(static_cast<unsigned char>(c)));
    return s;
}
size_t ulen(const string& s) {
    size_t n = 0;
    for (unsigned char c : s) if ((c & 0xC0) != 0x80) ++n;
    return n;
}
string rep(const string& unit, int times) { string out; while (times-- > 0) out += unit; return out; }
string center(const string& s, int width) {
    int n = static_cast<int>(ulen(s));
    if (n >= width) return s;
    int left = (width - n) / 2;
    return string(left, ' ') + s + string(width - n - left, ' ');
}
string clip(const string& s, int width) {
    if (static_cast<int>(s.size()) <= width) return s;
    return width <= 2 ? "" : s.substr(0, width - 2) + "..";
}
string pad(const string& s, int width, bool right = false) {
    string cut = clip(s, width);
    int fill = width - static_cast<int>(ulen(cut));
    string spaces(fill > 0 ? fill : 0, ' ');
    return right ? spaces + cut : cut + spaces;
}

// ----- Frames and messages -------------------------------------
const int HEAD_W = 46;
void edge(int indent, int width, bool top) {
    string line = top ? "╭" : "╰";
    line += rep("─", width) + (top ? "╮" : "╯");
    cout << string(indent, ' ') << paint(line, P.rose) << "\n";
}
void row(int indent, int width, const string& text, const string& style) {
    cout << string(indent, ' ') << paint("│", P.rose)
         << paint(center(text, width), style) << paint("│", P.rose) << "\n";
}
void screenHeader(const string& title, const string& subtitle) {
    cout << "\n";
    edge(2, HEAD_W, true);
    row(2, HEAD_W, "♡  " + title + "  ♡", P.bold + P.rose);
    row(2, HEAD_W, subtitle, P.blush);
    edge(2, HEAD_W, false);
    cout << "\n";
}
void info(const string& t) { cout << "  " << paint("♡", P.rose) << "  " << paint(t, P.cream) << "\n"; }
void success(const string& t) { cout << "\n  " << paint("♡", P.blush) << "  " << paint(t, P.bold + P.blush) << "\n"; }
void warning(const string& t) { cout << "\n  " << paint("!", P.rose) << "  " << paint(t, P.cream) << "\n"; }

// ----- Money and input -----------------------------------------
string money(double v) { ostringstream out; out << fixed << setprecision(2) << "$" << v; return out.str(); }
string readLine(const string& label) {
    cout << "  " << paint("♡", P.rose) << "  " << paint(label + ": ", P.cream);
    string line;
    if (!getline(cin, line)) { cout << "\n"; exit(0); }
    return trim(line);
}
string askText(const string& label) {
    string value;
    while ((value = readLine(label)).empty()) warning("This field cannot be empty.");
    return value;
}
bool number(const string& text, double& value) {
    istringstream in(text);
    in >> value;
    return in && in.eof();
}
int askInt(const string& label, int low, int high) {
    while (true) {
        double v = 0;
        if (number(readLine(label), v) && v == floor(v) && v >= low && v <= high)
            return static_cast<int>(v);
        warning("Please enter a number between " + to_string(low) + " and " + to_string(high) + ".");
    }
}
double askMoney(const string& label, double low, double high) {
    while (true) {
        double v = 0;
        if (number(readLine(label), v) && v >= low && v <= high)
            return round(v * 100.0) / 100.0;
        warning("Please enter an amount between " + money(low) + " and " + money(high) + ".");
    }
}
bool confirm(const string& question) {
    string answer = readLine(question + " [y/N]");
    return !answer.empty() && (answer[0] == 'y' || answer[0] == 'Y');
}

// ----- Order model and CSV storage -----------------------------
struct Order {
    int id;
    string customer;
    string item;
    int quantity;
    double unitPrice;
    string status;
    string date;
    string note;
};
const string DATA_FILE = "orders.csv";
const vector<string> STATUSES = {"Pending", "Processing", "Shipped", "Delivered", "Cancelled"};
string dateOffset(int daysAgo) {
    time_t moment = time(nullptr) - static_cast<time_t>(daysAgo) * 86400;
    tm local{};
#ifdef _WIN32
    localtime_s(&local, &moment);
#else
    localtime_r(&moment, &local);
#endif
    char buffer[11] = {};
    strftime(buffer, sizeof(buffer), "%Y-%m-%d", &local);
    return buffer;
}
string csvEscape(const string& field) {
    if (field.find_first_of(",\"\n\r") == string::npos) return field;
    string out = "\"";
    for (char ch : field) {
        if (ch == '"') out += "\"\"";
        else out += ch;
    }
    return out + "\"";
}
vector<string> parseCsvLine(const string& line) {
    vector<string> fields;
    string field;
    bool quoted = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char ch = line[i];
        if (quoted && ch == '"' && i + 1 < line.size() && line[i + 1] == '"') {
            field += '"';
            ++i;
        } else if (ch == '"') {
            quoted = !quoted;
        } else if (!quoted && ch == ',') {
            fields.push_back(field);
            field.clear();
        } else {
            field += ch;
        }
    }
    fields.push_back(field);
    return fields;
}
void saveOrders(const vector<Order>& orders) {
    ofstream out(DATA_FILE, ios::trunc);
    if (!out) { warning("Could not save " + DATA_FILE + ". Check folder permissions."); return; }
    out << "id,customer,item,quantity,unit_price,status,date,note\n";
    for (const Order& o : orders) {
        out << o.id << ',' << csvEscape(o.customer) << ',' << csvEscape(o.item) << ','
            << o.quantity << ',' << fixed << setprecision(2) << o.unitPrice << ','
            << csvEscape(o.status) << ',' << csvEscape(o.date) << ',' << csvEscape(o.note) << '\n';
    }
}
vector<Order> makeSampleOrders() {
    return {
        {1001, "Mia Chen",    "Silk Hair Bow",    2, 18.50, "Delivered",  dateOffset(9),  "Gift wrap, please"},
        {1002, "Hana Kim",    "Pearl Claw Clip",  3, 12.00, "Shipped",    dateOffset(6),  "Handle with care"},
        {1003, "Sofia Reyes", "Lace Ribbon Set",  1, 24.75, "Processing", dateOffset(4),  ""},
        {1004, "Yuna Park",   "Cream Tote Bag",   1, 42.00, "Pending",    dateOffset(2),  "Customer asked for express"},
        {1005, "Lila Moreau", "Velvet Scrunchie", 4,  9.25, "Cancelled",  dateOffset(10), "Out of stock"}
    };
}
vector<Order> loadOrders() {
    vector<Order> orders;
    ifstream in(DATA_FILE);
    if (!in) {
        orders = makeSampleOrders();
        saveOrders(orders);
        return orders;
    }
    string line;
    getline(in, line); // header
    while (getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        vector<string> f = parseCsvLine(line);
        if (f.size() < 8) continue;
        double id = 0, qty = 0, price = 0;
        if (!number(f[0], id) || !number(f[3], qty) || !number(f[4], price) ||
            id != floor(id) || qty != floor(qty) || qty <= 0 || price < 0) continue;
        Order o;
        o.id = static_cast<int>(id);
        o.quantity = static_cast<int>(qty);
        o.unitPrice = price;
        o.customer = f[1]; o.item = f[2]; o.status = f[5]; o.date = f[6]; o.note = f[7];
        if (find(STATUSES.begin(), STATUSES.end(), o.status) == STATUSES.end())
            o.status = "Pending";
        orders.push_back(o);
    }
    return orders;
}
int nextId(const vector<Order>& orders) {
    int highest = 1000;
    for (const Order& o : orders) highest = max(highest, o.id);
    return highest + 1;
}
int findOrderIndex(const vector<Order>& orders, int id) {
    for (size_t i = 0; i < orders.size(); ++i)
        if (orders[i].id == id) return static_cast<int>(i);
    return -1;
}

// ----- Table and order card ------------------------------------
const int ID_W = 4, CUSTOMER_W = 18, ITEM_W = 16, QTY_W = 3;
const int PRICE_W = 10, STATUS_W = 11, DATE_W = 10;
string statusStyle(const string& status) {
    if (status == "Pending") return P.blush;
    if (status == "Processing") return P.rose;
    if (status == "Shipped") return P.cream;
    if (status == "Delivered") return P.bold + P.pearl;
    if (status == "Cancelled") return P.dim;
    return P.cream;
}
void tableHeader() {
    cout << "  " << paint(pad("ID", ID_W), P.bold + P.rose) << "  " << paint(pad("Customer", CUSTOMER_W), P.bold + P.rose) << "  "
         << paint(pad("Item", ITEM_W), P.bold + P.rose) << "  " << paint(pad("Qty", QTY_W, true), P.bold + P.rose) << "  "
         << paint(pad("Price", PRICE_W, true), P.bold + P.rose) << "  "
         << paint(pad("Status", STATUS_W), P.bold + P.rose) << "  " << paint(pad("Date", DATE_W), P.bold + P.rose) << "\n"
         << "  " << paint(rep("─", 86), P.mocha) << "\n";
}
void tableRow(const Order& o) {
    cout << "  " << paint(pad(to_string(o.id), ID_W), P.cream) << "  " << paint(pad(o.customer, CUSTOMER_W), P.cream) << "  "
         << paint(pad(o.item, ITEM_W), P.cream) << "  " << paint(pad(to_string(o.quantity), QTY_W, true), P.cream) << "  "
         << paint(pad(money(o.unitPrice), PRICE_W, true), P.pearl) << "  "
         << paint(pad(o.status, STATUS_W), statusStyle(o.status)) << "  " << paint(pad(o.date, DATE_W), P.cream) << "\n";
}
void printTable(const vector<Order>& orders) {
    tableHeader();
    for (const Order& o : orders) tableRow(o);
    cout << "  " << paint(rep("─", 86), P.mocha) << "\n";
}
void printOrderCard(const Order& o, const string& title) {
    cout << "\n  " << paint("♡", P.rose) << "  "
         << paint(title + " #" + to_string(o.id) + " · " + o.customer, P.bold + P.blush) << "\n";
    cout << "  " << paint(o.item + "  ×" + to_string(o.quantity) + "  ·  " +
                             money(o.quantity * o.unitPrice) + "  ·  " + o.status +
                             "  ·  " + o.date, P.cream) << "\n";
    if (!o.note.empty()) cout << "  " << paint("note: " + o.note, P.dim) << "\n";
    cout << "  " << paint(rep("─", 60), P.mocha) << "\n";
}

// ----- Menu actions --------------------------------------------
void viewOrders(const vector<Order>& orders) {
    screenHeader("ALL ORDERS", "your little order ledger");
    if (orders.empty()) { info("No orders yet. Create your first one with option 01 ♡"); return; }
    printTable(orders);
    double active = 0;
    for (const Order& o : orders) if (o.status != "Cancelled") active += o.quantity * o.unitPrice;
    cout << "\n  " << paint(to_string(orders.size()) + " order(s)", P.blush)
         << "   " << paint("·", P.mocha) << "   "
         << paint("Active value " + money(active), P.pearl) << "\n";
}
void searchOrders(const vector<Order>& orders) {
    screenHeader("SEARCH ORDERS", "find a name, item, status or date");
    if (orders.empty()) { info("The ledger is empty, so there is nothing to search yet."); return; }
    string query = readLine("Search (Enter to cancel)");
    if (query.empty()) { info("Search cancelled. Take your time ♡"); return; }
    vector<Order> matches;
    for (const Order& o : orders) {
        string haystack = lower(to_string(o.id) + " " + o.customer + " " + o.item + " " +
                                o.status + " " + o.date + " " + o.note);
        if (haystack.find(lower(query)) != string::npos) matches.push_back(o);
    }
    if (matches.empty()) { warning("No matches for \"" + query + "\". Try a shorter word ♡"); return; }
    cout << "\n";
    printTable(matches);
    cout << "\n  " << paint(to_string(matches.size()) + " match(es) found ♡", P.blush) << "\n";
}
void createOrder(vector<Order>& orders) {
    screenHeader("CREATE ORDER", "a fresh entry for the ledger");
    Order o;
    o.id = nextId(orders);
    o.customer = askText("Customer name");
    o.item = askText("Item");
    o.quantity = askInt("Quantity (1-9999)", 1, 9999);
    o.unitPrice = askMoney("Unit price", 0.01, 99999.00);
    o.status = "Pending";
    o.date = dateOffset(0);
    o.note = readLine("Note (optional)");
    printOrderCard(o, "ORDER PREVIEW");
    if (!confirm("Save this order?")) { info("Order discarded. Nothing was written."); return; }
    orders.push_back(o);
    saveOrders(orders);
    success("Order #" + to_string(o.id) + " saved for " + o.customer + " ♡");
}
void updateStatus(vector<Order>& orders) {
    screenHeader("UPDATE STATUS", "keep every order moving");
    if (orders.empty()) { info("There are no orders to update yet."); return; }
    int id = askInt("Order ID (0 to go back)", 0, 999999);
    if (id == 0) return;
    int index = findOrderIndex(orders, id);
    if (index < 0) { warning("Order #" + to_string(id) + " was not found."); return; }
    Order& o = orders[index];
    printOrderCard(o, "ORDER");
    cout << "\n  " << paint("Choose a new status:", P.mocha) << "\n";
    for (int i = 0; i < static_cast<int>(STATUSES.size()); ++i) {
        bool current = STATUSES[i] == o.status;
        cout << "   " << paint(to_string(i + 1) + ".", P.rose) << " "
             << paint(STATUSES[i], current ? P.bold + P.pearl : P.cream)
             << (current ? paint("   ← current", P.dim) : "") << "\n";
    }
    int choice = askInt("New status (0 to cancel)", 0, static_cast<int>(STATUSES.size()));
    if (choice == 0) { info("Status unchanged."); return; }
    string next = STATUSES[choice - 1];
    if (next == o.status) { info("This order is already " + next + "."); return; }
    string previous = o.status;
    o.status = next;
    saveOrders(orders);
    success("Order #" + to_string(id) + " moved from " + previous + " to " + next + ".");
}
void deleteOrder(vector<Order>& orders) {
    screenHeader("DELETE ORDER", "gently remove an entry");
    if (orders.empty()) { info("There is nothing to delete yet."); return; }
    int id = askInt("Order ID (0 to go back)", 0, 999999);
    if (id == 0) return;
    int index = findOrderIndex(orders, id);
    if (index < 0) { warning("Order #" + to_string(id) + " was not found."); return; }
    printOrderCard(orders[index], "ORDER");
    if (!confirm("Delete this order permanently?")) {
        info("Nothing was deleted. Your ledger is safe ♡");
        return;
    }
    orders.erase(orders.begin() + index);
    saveOrders(orders);
    success("Order #" + to_string(id) + " was removed from the ledger.");
}

// ----- Summary ------------------------------------------------
void summaryLine(const string& label, const string& value) {
    int dots = 38 - static_cast<int>(ulen(label)) - static_cast<int>(ulen(value));
    if (dots < 2) dots = 2;
    cout << "  " << paint(label, P.cream) << " " << paint(rep(".", dots), P.mocha)
         << " " << paint(value, P.pearl) << "\n";
}
void showSummary(const vector<Order>& orders) {
    screenHeader("ORDER SUMMARY", "a gentle look at the ledger");
    if (orders.empty()) { info("Nothing to summarise yet. Create an order first."); return; }
    double active = 0, cancelled = 0;
    int activeOrders = 0, units = 0;
    map<string, double> spend;
    int counts[5] = {0};
    for (const Order& o : orders) {
        double total = o.quantity * o.unitPrice;
        for (int i = 0; i < 5; ++i) if (o.status == STATUSES[i]) ++counts[i];
        if (o.status == "Cancelled") { cancelled += total; continue; }
        active += total; ++activeOrders; units += o.quantity;
        spend[o.customer] += total;
    }
    summaryLine("Total orders", to_string(orders.size()));
    for (int i = 0; i < 5; ++i) summaryLine("  " + STATUSES[i], to_string(counts[i]));
    cout << "  " << paint(rep("─", 46), P.mocha) << "\n";
    summaryLine("Active order value", money(active));
    summaryLine("Cancelled value", money(cancelled));
    summaryLine("Units in flow", to_string(units));
    summaryLine("Average active order", money(activeOrders ? active / activeOrders : 0));
    string top = "—";
    double best = 0;
    for (const auto& entry : spend)
        if (entry.second > best) { best = entry.second; top = entry.first; }
    summaryLine("Top customer", top + " (" + money(best) + ")");
    cout << "\n";
    info("Keep the little details moving ♡");
}

// ----- Welcome, menu and main ----------------------------------
void welcome() {
    int left = 8;
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO info;
    int width = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)
        ? info.srWindow.Right - info.srWindow.Left + 1 : 0;
    if (width > 60) left = (width - HEAD_W - 2) / 2;
#endif
    cout << "\n\n";
    edge(left, HEAD_W, true);
    row(left, HEAD_W, "", P.cream);
    row(left, HEAD_W, "♡  WELCOME TO  ♡", P.blush);
    row(left, HEAD_W, "", P.cream);
    row(left, HEAD_W, "ORDERFLOW", P.bold + P.rose);
    row(left, HEAD_W, "", P.cream);
    row(left, HEAD_W, "Order Management System", P.cream);
    row(left, HEAD_W, "Organized with a little grace.", P.blush);
    edge(left, HEAD_W, false);
    cout << "\n" << string(left + 7, ' ')
         << paint("⋆  Your orders, beautifully managed.  ⋆", P.mocha) << "\n\n";
    cout << string(left + 13, ' ') << paint("Press Enter to begin ♡", P.dim);
    string line;
    if (!getline(cin, line)) { cout << "\n"; exit(0); }
    cout << "\n";
}
void showMenu() {
    cout << "\n";
    edge(2, HEAD_W, true);
    row(2, HEAD_W, "ORDERFLOW ♡", P.bold + P.rose);
    row(2, HEAD_W, "──  MAIN MENU  ──", P.blush);
    edge(2, HEAD_W, false);
    cout << "\n";
    const vector<string> items = {"Create New Order", "View All Orders", "Search Orders",
                                  "Update Order Status", "Delete Order", "Order Summary", "Exit"};
    for (size_t i = 0; i < items.size(); ++i)
        cout << "    " << paint((i < 9 ? "0" : "") + to_string(i + 1) + ".", P.rose)
             << "  " << paint(items[i], P.cream) << "\n";
    cout << "\n";
}
void pause() {
    cout << "\n  " << paint("♡", P.rose) << "  "
         << paint("Press Enter to return to the menu", P.dim) << " ";
    string line;
    if (!getline(cin, line)) { cout << "\n"; exit(0); }
}
void goodbye() {
    cout << "\n  " << paint("♡  Thank you for using ORDERFLOW  ♡", P.bold + P.rose) << "\n"
         << "  " << paint("Organized with grace, always. Your ledger is saved ♡", P.blush) << "\n\n";
}
int main() {
    initTheme();
    vector<Order> orders = loadOrders();
    welcome();
    clearScreen();
    bool running = true;
    while (running) {
        showMenu();
        int choice = askInt("Please select an option", 1, 7);
        clearScreen();
        switch (choice) {
            case 1: createOrder(orders); break;
            case 2: viewOrders(orders); break;
            case 3: searchOrders(orders); break;
            case 4: updateStatus(orders); break;
            case 5: deleteOrder(orders); break;
            case 6: showSummary(orders); break;
            case 7: goodbye(); running = false; break;
        }
        if (running) pause();
        clearScreen();
    }
    return 0;
}
