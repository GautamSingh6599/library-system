// Copyright [2024]
// Gautam Singh
// Rishab P. Hariharan

#include "../include/tui.h"
#include <ctype.h>
#include <curses.h>
#include <form.h>
#include <menu.h>
#include <ncurses.h>
#include <panel.h>
#include <stdarg.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>

#define EXIT_SUCCESS 0

extern char *user;

// Global variables for windows and panels
WINDOW *title_win, *main_win, *bottom_win;
PANEL *title_panel, *main_panel, *bottom_pnl;

// Function to create a window
WINDOW *create_newwin(int height, int width, int starty, int startx) {
  WINDOW *local_win;
  local_win = newwin(height, width, starty, startx);
  box(local_win, 0, 0);
  wrefresh(local_win);
  return local_win;
}

// Function to create the title bar
void title_bar(const char *title, const char *button) {
  werase(title_win);
  box(title_win, 0, 0);

  // Print the title
  mvwprintw(title_win, 1, 2, "%s", title);

  // Print the button on the right side
  int button_pos = getmaxx(title_win) - strlen(button) - 3;
  mvwprintw(title_win, 1, button_pos, "[%s]", button);

  wrefresh(title_win);
}

// Function to display content in the main window
void display_main_content() {
  werase(main_win);
  box(main_win, 0, 0);
  mvwprintw(main_win, 1, 2, "Main Content Window");
  wrefresh(main_win);
}

// Function to display content in the bottom window
void display_bottom_content() {
  werase(bottom_win);
  box(bottom_win, 0, 0);
  mvwprintw(bottom_win, 1, 2, "Bottom Window");
  wrefresh(bottom_win);
}

// Function to initialize the UI
void initialize_ui() {
  // Initialize the ncurses mode
  initscr();
  cbreak();
  noecho();
  keypad(stdscr, TRUE);
  refresh();

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  // Create windows
  title_win = create_newwin(3, max_x, 0, 0);
  main_win = create_newwin(max_y - 6, max_x, 3, 0);
  bottom_win = create_newwin(3, max_x, max_y - 3, 0);

  // Create panels
  title_panel = new_panel(title_win);
  main_panel = new_panel(main_win);
  bottom_pnl = new_panel(bottom_win);

  // Display initial content
  title_bar("Library IISEK", "login");
  display_main_content();
  display_bottom_content();
}

void cleanup_ui() {
  // Delete panels
  del_panel(title_panel);
  del_panel(main_panel);
  del_panel(bottom_pnl);

  // Delete windows
  delwin(title_win);
  delwin(main_win);
  delwin(bottom_win);
  refresh();

  // End ncurses mode
}

void clear_line() {
  int y;
  y = getcury(stdscr);
  move(y, 0);
  clrtoeol();
}

void loading_bar() {
  int width, y;
  width = getmaxx(stdscr) - 15;
  y = getcury(stdscr);
  for (int i = 0; i < width; ++i) {
    mvprintw(y, 0, "[%d %% loading] ", i * 100 / width);
    for (int j = 0; j < i; ++j) {
      printw("█");
      usleep(100);
    }
    refresh();
    clear_line();
  }
}

void slow_print(char *str, int n) {
  for (int i = 0; i < strlen(str); i++) {
    printw("%c", str[i]);
    usleep(n);
    refresh();
  }
}

void begin() {
  slow_print("Starting library system.\n", 1000);
  loading_bar();
  clear();
  refresh();
  slow_print(R"EOF(void login(const char *role) {
  FIELD *fields[3];
  FORM *login_form;
  WINDOW *form_win;
  int ch, rows, cols;

  // Create fields for Username and Password
  fields[0] = new_field(1, 20, 2, 10, 0, 0);
  fields[1] = new_field(1, 20, 4, 10, 0, 0);
  fields[2] = NULL; // Terminate the field array

  set_field_back(fields[0], A_UNDERLINE);
  set_field_back(fields[1], A_UNDERLINE);
  field_opts_off(fields[1], O_PUBLIC); // Password hidden

  // Create the form
  login_form = new_form(fields);
  scale_form(login_form, &rows, &cols);

  // Create window for the form
  form_win = newwin(rows + 4, cols + 4, 10, 20);
  keypad(form_win, TRUE);
  box(form_win, 0, 0);
  set_form_win(login_form, form_win);
  set_form_sub(login_form, derwin(form_win, rows, cols, 1, 1));

  post_form(login_form);
  wrefresh(form_win);

  char username[20], password[20];
  int valid = 0;

  while ((ch = wgetch(form_win)) != KEY_F(1)) {
    switch (ch) {
    case KEY_DOWN:
      form_driver(login_form, REQ_NEXT_FIELD);
      form_driver(login_form, REQ_END_LINE);
      break;
    case KEY_UP:
      form_driver(login_form, REQ_PREV_FIELD);
      form_driver(login_form, REQ_END_LINE);
      break;
    case KEY_BACKSPACE:
    case 127:
      form_driver(login_form, REQ_DEL_PREV);
      break;
    case 10: // Enter key
      form_driver(login_form, REQ_NEXT_FIELD);
      form_driver(login_form, REQ_PREV_FIELD);

      // Get the input data
      snprintf(username, sizeof(username), "%s",
               trim_whitespaces(field_buffer(fields[0], 0)));
      snprintf(password, sizeof(password), "%s",
               trim_whitespaces(field_buffer(fields[1], 0)));

      // Validate credentials
      if (strcmp(role, "Admin") == 0) {
        valid = check_admin_credentials(username, password);
      } else {
        valid = check_student_credentials(username, password);
      }

      if (valid) {
        show_message("Access Granted");

        // Cleanup login form and window
        unpost_form(login_form);
        free_form(login_form);
        free_field(fields[0]);
        free_field(fields[1]);
        delwin(form_win);

        // Clear screen and reinitialize the entire UI
        clear();
        refresh();
        initialize_ui(); // Redraw everything
        title_bar("Library IISERK", username); // Update title bar with username
        return;
      } else {
        show_message("Invalid Credentials");
      }
      break;
    default:
      form_driver(login_form, ch);
      break;
    }
  }

  // Cleanup login form and window
  unpost_form(login_form);
  free_form(login_form);
  free_field(fields[0]);
  free_field(fields[1]);
  delwin(form_win);

  // Redraw UI if login was cancelled
  clear();
  refresh();
  initialize_ui(); // Redraw everything
  title_bar("Library IISERK", "login");
}
      _/  _/  _/
     _/      _/_/_/    _/  _/_/    _/_/_/  _/  _/_/  _/    _/
    _/  _/  _/    _/  _/_/      _/    _/  _/_/      _/    _/  _/_/_/_/_/
   _/  _/  _/    _/  _/        _/    _/  _/        _/    _/
  _/  _/  _/_/_/    _/          _/_/_/  _/          _/_/_/
                                                       _/
                                                  _/_/

                                      _/
       _/_/_/  _/    _/    _/_/_/  _/_/_/_/    _/_/    _/_/_/  _/_/
    _/_/      _/    _/  _/_/        _/      _/_/_/_/  _/    _/    _/
       _/_/  _/    _/      _/_/    _/      _/        _/    _/    _/
  _/_/_/      _/_/_/  _/_/_/        _/_/    _/_/_/  _/    _/    _/
                 _/
            _/_/

  )EOF",
             1000);
  sleep(1);
  clear();
  refresh();
}

// Dummy credential check functions
int check_admin_credentials(const char *username, const char *password) {
  return (strcmp(username, "admin") == 0 && strcmp(password, "admin123") == 0);
}

int check_student_credentials(const char *username, const char *password) {
  return (strcmp(username, "student") == 0 &&
          strcmp(password, "student123") == 0);
}

// Function to display a message in a popup window
void show_message(const char *message) {
  int rows, cols;
  getmaxyx(stdscr, rows, cols);

  // Create a new window for the message
  WINDOW *msg_win = newwin(5, cols - 10, rows / 2 - 2, 5);
  box(msg_win, 0, 0);

  // Print the message in the center of the window
  mvwprintw(msg_win, 2, 2, "%s", message);
  wrefresh(msg_win);

  // Wait for a short duration (e.g., 2 seconds)
  nodelay(msg_win, TRUE); // Make window non-blocking
  napms(2000);            // Wait for 200 milliseconds (2 seconds)

  // Refresh the windows below to restore their content
}

// Function to trim leading and trailing whitespaces
char *trim_whitespaces(char *str) {
  // Trim leading spaces
  while (isspace((unsigned char)*str))
    str++;

  // If the string is now empty, return it
  if (*str == 0)
    return str;

  // Trim trailing spaces
  char *end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;

  // Write the null terminator after the last non-whitespace character
  *(end + 1) = '\0';

  return str;
}

void login(const char *role) {
  FIELD *fields[3];
  FORM *login_form;
  WINDOW *form_win;
  int ch, rows, cols;

  // Create fields for Username and Password
  fields[0] = new_field(1, 20, 2, 10, 0, 0);
  fields[1] = new_field(1, 20, 4, 10, 0, 0);
  fields[2] = NULL; // Terminate the field array

  set_field_back(fields[0], A_UNDERLINE);
  set_field_back(fields[1], A_UNDERLINE);
  field_opts_off(fields[1], O_PUBLIC); // Password hidden

  // Create the form
  login_form = new_form(fields);
  scale_form(login_form, &rows, &cols);

  // Create window for the form
  form_win = newwin(rows + 4, cols + 4, 10, 20);
  keypad(form_win, TRUE);
  box(form_win, 0, 0);
  set_form_win(login_form, form_win);
  set_form_sub(login_form, derwin(form_win, rows, cols, 1, 1));

  post_form(login_form);
  wrefresh(form_win);

  char username[20], password[20];
  int valid = 0;

  while ((ch = wgetch(form_win)) != KEY_F(1)) {
    switch (ch) {
    case KEY_DOWN:
      form_driver(login_form, REQ_NEXT_FIELD);
      form_driver(login_form, REQ_END_LINE);
      break;
    case KEY_UP:
      form_driver(login_form, REQ_PREV_FIELD);
      form_driver(login_form, REQ_END_LINE);
      break;
    case KEY_BACKSPACE:
    case 127:
      form_driver(login_form, REQ_DEL_PREV);
      break;
    case 10: // Enter key
      form_driver(login_form, REQ_NEXT_FIELD);
      form_driver(login_form, REQ_PREV_FIELD);

      // Get the input data
      snprintf(username, sizeof(username), "%s",
               trim_whitespaces(field_buffer(fields[0], 0)));
      snprintf(password, sizeof(password), "%s",
               trim_whitespaces(field_buffer(fields[1], 0)));

      // Validate credentials
      if (strcmp(role, "Admin") == 0) {
        valid = check_admin_credentials(username, password);
      } else {
        valid = check_student_credentials(username, password);
      }

      if (valid) {
        show_message("Access Granted");

        // Cleanup login form and window
        unpost_form(login_form);
        free_form(login_form);
        free_field(fields[0]);
        free_field(fields[1]);
        delwin(form_win);

        // Clear screen and reinitialize the entire UI
        clear();
        refresh();
        initialize_ui();
        user = username;
        title_bar("Library IISERK", username);
        return;
      } else {
        show_message("Invalid Credentials");
      }
      break;
    case 27: // Escape key pressed
      // Exit login and redraw everything
      unpost_form(login_form);
      free_form(login_form);
      free_field(fields[0]);
      free_field(fields[1]);
      delwin(form_win);

      clear();
      refresh();
      initialize_ui();
      title_bar("Library IISERK", user);
      return;
    default:
      form_driver(login_form, ch);
      break;
    }
  }

  // Cleanup if login was cancelled
  unpost_form(login_form);
  free_form(login_form);
  free_field(fields[0]);
  free_field(fields[1]);
  delwin(form_win);

  clear();
  refresh();
  initialize_ui();
  title_bar("Library IISERK", user);
}

void show_role_menu() {
  ITEM *items[3];
  MENU *menu;
  WINDOW *menu_win;

  char *choices[] = {"Admin", "Student", (char *)NULL};
  int n_choices = sizeof(choices) / sizeof(char *) - 1;
  int ch, selected;

  // Create menu items
  for (int i = 0; i < n_choices; ++i)
    items[i] = new_item(choices[i], "");
  items[n_choices] = (ITEM *)NULL;

  // Create menu
  menu = new_menu(items);
  menu_win = newwin(10, 40, 4, 10);
  keypad(menu_win, TRUE);

  set_menu_win(menu, menu_win);
  set_menu_sub(menu, derwin(menu_win, 6, 38, 3, 1));
  set_menu_mark(menu, " * ");
  box(menu_win, 0, 0);
  mvwprintw(menu_win, 1, 2, "Select Role:");
  post_menu(menu);
  wrefresh(menu_win);

  while ((ch = wgetch(menu_win)) != 'q') {
    switch (ch) {
    case KEY_DOWN:
      menu_driver(menu, REQ_DOWN_ITEM);
      break;
    case KEY_UP:
      menu_driver(menu, REQ_UP_ITEM);
      break;
    case 10: // Enter key
      selected = item_index(current_item(menu));
      unpost_menu(menu);
      free_menu(menu);
      for (int i = 0; i < n_choices; ++i)
        free_item(items[i]);
      delwin(menu_win);
      login(choices[selected]);
      return;
    case 27: // Escape key pressed
      // Exit role menu and redraw everything
      unpost_menu(menu);
      free_menu(menu);
      for (int i = 0; i < n_choices; ++i)
        free_item(items[i]);
      delwin(menu_win);

      clear();
      refresh();
      initialize_ui();
      title_bar("Library IISERK", user);
      return;
    }
    wrefresh(menu_win);
  }

  // Cleanup if menu was cancelled
  unpost_menu(menu);
  free_menu(menu);
  for (int i = 0; i < n_choices; ++i)
    free_item(items[i]);
  delwin(menu_win);

  clear();
  refresh();
  initialize_ui();
  title_bar("Library IISERK", user);
}

int main() {
  curs_set(0);
  initialize_ui();

  int ch;
  while ((ch = getch())) {
    switch (ch) {
    case 'l':
      // Change button text on pressing 'b'
      show_role_menu();
      break;
    default:
      // Restore default button text
      title_bar("Library IISERK", user);
      break;
    }
  }

  cleanup_ui();
  endwin();
  return 0;
}
