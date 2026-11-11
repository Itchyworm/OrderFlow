# OrderFlow — Order Management System ♡

> Your orders, beautifully managed.

**OrderFlow** is a small, polished console-based order management system written in
**C++17**. It was built as a portfolio project for an **Order Management / Expeditor**
role: it demonstrates order tracking, status coordination, search, and simple
reporting in an interface that feels more like soft Korean stationery than a
programming assignment.

The whole application lives in **one source file** (`main.cpp`) and stores its data
in a plain **CSV file** (`orders.csv`). No external libraries, no database, no build
system.

---

## ✨ Features

| # | Feature | What it does |
|---|---------|--------------|
| 01 | Create New Order | Capture customer, item, quantity, price, and a note |
| 02 | View All Orders | See every order in a clean aligned ledger |
| 03 | Search Orders | Case-insensitive search by ID, customer, item, status, date, or note |
| 04 | Update Order Status | Move an order through Pending → Processing → Shipped → Delivered |
| 05 | Delete Order | Remove an order after a confirmation prompt |
| 06 | Order Summary | Status counts, active value, cancelled value, units in flow, average order, top customer |
| 07 | Exit | Save state and close gracefully |

Other details:

- Input validation that never crashes on bad numbers or empty required fields.
- Automatic CSV creation with a few sample orders on first run.
- CSV fields with commas or quotes are escaped and parsed correctly.
- UTF-8 output, ANSI truecolor theme, and a plain-text fallback.
- The screen clears only in an interactive terminal, so piped/scripted output stays readable.
- Graceful exit if the input stream closes.

---

## 🎀 Interface

```text
        ╭──────────────────────────────────────────────╮
        │                                              │
        │               ♡  WELCOME TO  ♡               │
        │                                              │
        │                  ORDERFLOW                   │
        │                                              │
        │           Order Management System            │
        │        Organized with a little grace.        │
        ╰──────────────────────────────────────────────╯

               ⋆  Your orders, beautifully managed.  ⋆
```

```text
  ╭──────────────────────────────────────────────╮
  │                 ORDERFLOW ♡                  │
  │              ──  MAIN MENU  ──               │
  ╰──────────────────────────────────────────────╯

    01.  Create New Order
    02.  View All Orders
    03.  Search Orders
    04.  Update Order Status
    05.  Delete Order
    06.  Order Summary
    07.  Exit

  ♡  Please select an option:
```

---

## 🎨 Palette

The theme uses these design references:

| Colour | Hex | Used for |
|--------|-----|----------|
| Vanilla Cream | `#F7EDE8` | Body text |
| Blush Pink | `#EBC6D2` | Highlights and messages |
| Rose Pink | `#D997AF` | Headings and frames |
| Mocha Brown | `#795747` | Separators and quiet detail |
| Pearl White | `#FFF9F5` | Emphasised values |

Colours are rendered with 24-bit ANSI escape codes. They are enabled only when the
output is an interactive terminal that supports ANSI. In every other case — or when
the `NO_COLOR` environment variable is set — OrderFlow falls back to clean plain
text automatically. The layout is designed for a **dark terminal** (Windows
Terminal default) around 100 columns wide.

---

## 📁 Project structure

```text
OrderFlow/
├── main.cpp      # the entire application (498 lines, C++17)
├── orders.csv    # automatically created and maintained data file
└── README.md     # this document
```

There is exactly one source file, no headers, no extra folders, and no third-party
dependencies.

---

## 🛠 Requirements

- Windows 10 or Windows 11
- g++ with C++17 support (MSYS2 **UCRT64** environment is recommended)

Check your compiler:

```bash
g++ --version
```

The project was built and tested with g++ 16.2.0 from MSYS2 UCRT64.

---

## 🚀 Build and run

### Option 1 — MSYS2 UCRT64 terminal (recommended)

```bash
cd "/c/Users/panha/Documents/Default Project/OrderFlow"
g++ -std=c++17 -Wall -Wextra -O2 main.cpp -o OrderFlow.exe
./OrderFlow.exe
```

### Option 2 — PowerShell

```powershell
$env:PATH = "C:\msys64\ucrt64\bin;$env:PATH"
cd "C:\Users\panha\Documents\Default Project\OrderFlow"
g++ -std=c++17 -Wall -Wextra -O2 -static main.cpp -o OrderFlow.exe
.\OrderFlow.exe
```

### Optional: build a self-contained executable

If `OrderFlow.exe` complains about missing MSYS2 DLLs when moved to another
computer, link statically:

```bash
g++ -std=c++17 -Wall -Wextra -O2 -static main.cpp -o OrderFlow.exe
```

Run the program **from the same folder as `orders.csv`**. The application looks for
the file in the current working directory.

---

## 💾 How the data is stored

`orders.csv` is a small, human-readable ledger:

```csv
id,customer,item,quantity,unit_price,status,date,note
1001,Mia Chen,Silk Hair Bow,2,18.50,Delivered,2026-11-02,"Gift wrap, please"
1002,Hana Kim,Pearl Claw Clip,3,12.00,Shipped,2026-11-05,Handle with care
```

Rules:

- The first row is always the header.
- One row represents one order.
- Fields containing a comma, quote, or line break are wrapped in double quotes.
- A literal quote inside a field is written twice (`""`), following standard CSV rules.
- The file is rewritten after every create, update, or delete.

On first launch, if `orders.csv` does not exist, OrderFlow creates it with five
sample orders. To reset the project, simply delete `orders.csv` and run again.

### Sample orders

| ID | Customer | Item | Qty | Unit price | Status |
|----|----------|------|-----|------------|--------|
| 1001 | Mia Chen | Silk Hair Bow | 2 | $18.50 | Delivered |
| 1002 | Hana Kim | Pearl Claw Clip | 3 | $12.00 | Shipped |
| 1003 | Sofia Reyes | Lace Ribbon Set | 1 | $24.75 | Processing |
| 1004 | Yuna Park | Cream Tote Bag | 1 | $42.00 | Pending |
| 1005 | Lila Moreau | Velvet Scrunchie | 4 | $9.25 | Cancelled |

---

## 🧠 How the code is organised

The single file is divided into small, readable sections:

1. **Palette and terminal setup** — truecolor ANSI, UTF-8 console mode on Windows,
   and the `NO_COLOR`/non-interactive fallback.
2. **Text helpers** — trimming, case conversion, visible-width-aware centring,
   padding, and clipping.
3. **Frames and messages** — the reusable box drawing and status messages.
4. **Money and input** — safe number parsing, validated integers and prices, and
   confirmation prompts.
5. **Order model and CSV storage** — the `Order` struct plus load, save, escape, and
   parse functions.
6. **Table and order card** — aligned ledger output and a compact detail card.
7. **Menu actions** — create, view, search, update, delete, and summary.
8. **Welcome, menu, and main loop** — the screen flow and the seven-option switch.

### Key beginner-friendly concepts demonstrated

- `struct` for modelling a real-world record
- `std::vector<Order>` as an in-memory collection
- `std::ifstream` / `std::ofstream` for persistent storage
- CSV escaping and a small quoted-field parser
- Input validation loops with `try`/`catch`-free numeric parsing
- Functions with a single clear responsibility
- `switch`-driven menu flow
- ANSI escape sequences and platform-aware terminal setup
- Graceful fallbacks instead of assuming a fancy terminal

### Order lifecycle

The five supported statuses are:

```text
Pending  →  Processing  →  Shipped  →  Delivered
                    ↘
                   Cancelled
```

The app lets the user choose any status because real order desks sometimes correct
mistakes; the intended happy path is the left-to-right flow above.

---

## 🗣 Talking points for the interview

- **Why CSV?** It is transparent, editable, and enough for a single-user desktop
  tool. It also demonstrates file I/O and data validation without database setup.
- **Why one file?** The project is deliberately small. Keeping everything in
  `main.cpp` makes the beginner-level scope honest and easy to walk through.
- **How is data kept safe?** The file is rewritten atomically enough for this scale;
  input is validated before saving, and CSV escaping protects values with commas.
- **What would you add next?** A database, sorting/filtering options, date validation,
  cursor-based editing, and automated tests.
- **What does the expeditor connection mean?** Orders move through operational
  statuses, and the summary gives a quick view of workload: what is pending, what is
  in flow, and what has been delivered.

---

## 🧸 Limitations

This is an intentionally small beginner+ project, not a production system:

- Single-user, local file only.
- No login or access control.
- No date picker; new orders use today's date.
- Status transitions are flexible rather than enforced.
- Table columns are tuned for Latin-script demo data; very wide CJK names may shift
  alignment slightly.
- No undo after a confirmed delete.

---

## 💗 Credits

Design and code: OrderFlow portfolio project.
Made with C++17, a little patience, and a lot of pink.

```text
        ♡  Organized with a little grace.  ♡
```
