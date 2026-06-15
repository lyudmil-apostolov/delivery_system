# E-Commerce Delivery Management System

A console-based e-commerce order management system written in C (C11).  
Built for **NF06_EN** — Université de Technologie de Troyes (UTT) — Friday Group 7.

---

## Team

| Member | Responsibilities |
|--------|-----------------|
| Lyudmil Apostolov | Product Data · Data Handling & Stock Management |
| Anish Patni | Customer Profile · Queue & Data Structures |
| Krisha Devannavar | Administrator Profile · Delivery History (File Management) · CLI |

---

## Prerequisites

| Tool | Minimum version | Notes |
|------|----------------|-------|
| GCC  | 8.x or later   | Must support `-std=c11` |
| Make | Any GNU Make   | Optional — you can also compile manually |

**Windows:** Install [MinGW-w64](https://www.mingw-w64.org/) or use the GCC that ships with [Code::Blocks](https://www.codeblocks.org/). Make sure `gcc` is on your `PATH`.  
**Linux / macOS:** GCC and Make are typically pre-installed or available via your package manager (`apt install build-essential` / `brew install gcc`).

---

## Building the Project

### Option A — Using Make (recommended)

Open a terminal in the project root (the folder that contains `Makefile`) and run:

```bash
make
```

This compiles all source files and produces the `delivery_system` (or `delivery_system.exe` on Windows) executable.

To compile **and** run in one step:

```bash
make run
```

To remove compiled object files and the executable:

```bash
make clean
```

### Option B — Manual compilation with GCC

If you do not have Make installed, compile directly:

```bash
gcc -Wall -Wextra -std=c11 -g -o delivery_system main.c product.c order.c customer.c admin.c
```

On Windows (MinGW) the output file will be `delivery_system.exe` automatically.

---

## Running the Program

> **Important:** Always launch the executable from the **project root directory** (the folder that contains `data/`, `history/`, `logs/`, and `reports/`). The program uses relative paths to access these folders.

```bash
# Linux / macOS
./delivery_system

# Windows (PowerShell or CMD)
.\delivery_system.exe
```

If you compiled with Code::Blocks, use **Build → Run** (Ctrl+F10) — the IDE sets the working directory automatically.

---

## Directory Structure

```
delivery_system/
├── main.c              # Entry point and top-level menu
├── product.h / .c      # Product catalogue: load, save, display, stock
├── order.h   / .c      # Order & Queue data structures and operations
├── customer.h / .c     # Customer menus: browse, cart, checkout
├── admin.h   / .c      # Admin menus: delivery, stats, history, reports
├── Makefile            # Build configuration
│
├── data/
│   └── products.csv    # Product catalogue (read on start, saved on exit)
├── history/            # Daily delivery logs  (DD-MM-YYYY.txt per day)
├── logs/               # Admin activity log   (admin_log.txt)
└── reports/            # Exported daily reports (report_DD-MM-YYYY.txt)
```

---

## Usage

On launch you will see the main menu:

```
========================================
  E-Commerce Delivery Management System
========================================
  1. Customer mode
  2. Administrator mode
  3. Exit
```

- **Customer mode** — browse products by category, manage your cart, and place orders with high or normal delivery priority.
- **Administrator mode** — password protected (`utt`). Process deliveries, view queues and statistics, cancel orders, and export daily reports.
- **Exit** — saves updated stock levels back to `data/products.csv` before closing.

---

## Data File Format (`data/products.csv`)

Category names appear as plain lines (no comma). Products follow as `name, price, stock`:

```
Electronics
Laptop_1, 1200.00, 10
Headphones_2, 30.00, -1

Clothing
Tshirt_1, 20.00, 30
```

A stock value of `-1` means **unlimited supply**; `0` means the item is currently **unavailable**.
