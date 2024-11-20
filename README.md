# Library Management System - TUI (Text-based User Interface)

## Overview

This repository contains the implementation of a library management system with a Text-based User Interface (TUI). The system allows users to interact with the library, issue books, manage accounts, and perform administrative tasks such as adding, editing, or removing books from the collection. The user interface is built using `ncurses` to provide an efficient and interactive way to manage the library system through the terminal.

### Files Overview

- **tui.h**: Contains the core functions for displaying and interacting with the TUI, including commands for displaying books, issuing books, and navigating the system.
- **issuing.h**: Manages the issuance and return of books. It includes functions for issuing, returning, and tracking issued books.
- **chatbot.h**: Implements a simple chatbot interface, which can be used for user interactions.
- **bookrecords.h**: Manages operations related to book records such as adding, removing, or updating the number of copies of books.
- **book.h**: Contains the structure definitions and utility functions for managing book information such as ISBN, title, and author.
- **accounts.h**: Handles user account creation, login, and password management using SHA-256 for secure password hashing.

## Key Functions

### TUI Functions (tui.h)

- **print_header**: Displays the header of the book list table.
- **print_issued_header**: Displays the header for the list of issued books.
- **print_footer**: Prints the footer with user information at the bottom of the TUI window.
- **show_error_message**: Displays error messages on the screen.
- **print_book**: Prints the details of a single book on the screen, with formatting.
- **print_issued_book**: Prints the details of an issued book.
- **display_books**: Displays the list of books available in the library.
- **update_highlight**: Updates the highlighted book in the list when navigating.
- **command_mode**: Starts the command mode for user interactions with the library (includes both user and admin modes).
- **login_tui**: Handles the login process via the TUI.
- **log_out**: Logs the user out of the system.
- **signup_tui**: Handles user sign-up via the TUI.
- **issue_tui**: Facilitates book issuing for users based on their credentials.
- **admin_mode**: Allows administrators to perform management tasks such as adding, editing, and removing books.

### Issuing Functions (issuing.h)

- **get_current_date**: Retrieves the current system date.
- **issuedbyuser**: Retrieves a list of books issued by a specific user.
- **returnbook**: Returns a book by a user.
- **issuebook**: Issues a book to a user, depending on their user type (admin/user).

### Chatbot Functions (chatbot.h)

- **find_answer**: Searches for an answer to a user's query based on a knowledge base.
- **strip_string**: Strips any unnecessary characters from the input string.
- **tokenize**: Breaks down the input string into tokens (words).
- **compute_similarity**: Measures the similarity between two input strings based on tokenization.

### Book Management Functions (bookrecords.h)

- **book_add**: Adds a new book to the library database.
- **book_remove**: Removes a book from the library by its ISBN.
- **change_copies**: Changes the number of copies available for a specific book.

### Book Structure (book.h)

- **Book**: Structure that defines a book with fields for ISBN, title, author, and the number of copies.
- **window**: Returns a window of books starting from a specific index.
- **read_csv_range**: Reads a range of books from a CSV file and stores them in a `Book` array.
- **contains_substring**: Checks if a book's title or author contains a specific substring.
- **filter_books**: Filters books based on a search string and returns the results.

### Account Management Functions (accounts.h)

- **generate_random_string**: Generates a random string of a specified length (useful for creating random account details).
- **compute_sha256**: Computes the SHA-256 hash of a password or string.
- **signup**: Handles user registration, including username and password hashing.
- **login**: Manages user login by validating the username and password.

## Dependencies (Best to do in Linux Environment)

- **ncurses**: Used for creating the TUI and managing window layouts.
- **OpenSSL**: Required for password hashing in the account management system.
- **wchar_t**: Used for handling wide characters in book titles and author names to support Unicode characters.

## Installation Instructions (In Linux, can directly skip to step 3)

1. Install the required libraries:
   - For **ncurses**:

     ```bash
     sudo apt-get install libncurses5-dev libncursesw5-dev
     ```

   - For **OpenSSL**:

     ```bash
     sudo apt-get install libssl-dev
     ```

2. Compile the code:

   ```bash
   make
   ```

3. Run the application:

   ```bash
   ./library_system
   ```

## How to Use

- Passwords for testing
  - Administrator Privileges
    - Username: admin, Password: adminpaswd
    - Can also sigup.
    - Use the application in fullscreen mode, i.e, keep the terminal fully extended.
    - Most of the instructions are on the taskbar / footer of the screen.
    - A lot of the modes are composable, i.e, you can seach and issue a book or in admin mode you can seach and delete / edit a book.
- **Login/Sign Up**: Upon launching the program, you can either log in if you have an existing account or sign up to create a new one. The login system supports different user types (e.g., users and administrators).
  
- **User Mode**: Once logged in, users can:
  - Browse the catalog of books.
  - Search for books by title or author.
  - Issue or return books based on availability.
  
- **Admin Mode**: Admin users can:
  - Add new books to the library.
  - Edit book details (e.g., title, author, or number of copies).
  - Remove books from the library.

- **Chatbot**: The chatbot allows users to ask questions and get responses based on a predefined knowledge base.

## Project Contributors

1. Gautam Singh (22MS023)
2. Rishab P. Hariharan ()

This README provides an overview of the functionalities and setup for the library management system. It is designed to help users understand the core modules and quickly get started with the system.
