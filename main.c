// ========================================
// LIBRARY BOOK MANAGEMENT SYSTEM
// ========================================

#include <stdio.h>
#include <string.h>

#pragma region Struct creation
// Stores book information
struct Book
{
    char id[20];
    char title[50];
    char status[20];
};

// Stores menu action information
struct Action
{
    int command;
    char title[20];
    int functionIndex;
};
#pragma endregion

#pragma region Function declaration
// Helpers Functions
int printActions(char title[], struct Action action[], int size);
void waitForUserInput(char action[]);

// Main Functions
void loadFile();
void displayBook();
void searchBook();
void addBook();
void borrowBook();
void returnBook();
void sortBook();
void saveFile();
#pragma endregion

#pragma region Constant variables
// File path for books data
const char booksDataPath[] = "data/books.txt";

// File path for printer data
const char printerDir[] = "data/printer";
#pragma endregion

#pragma region Global variables
// Array to store all books
struct Book library[100];

// Total number of books loaded
int totalBook = 0;

struct Action mainActions[7] = {
    {1, "Display", 0},
    {2, "Search", 1},
    {3, "Add Book", 2},
    {4, "Borrow", 3},
    {5, "Return", 4},
    {6, "Sort", 5},
    {7, "Exit"}};

// Commands for search
struct Action searchActions[4] = {
    {1, "By Id", 0},
    {2, "By Title", 1},
    {3, "By Status", 2},
    {4, "Back"},
};
struct Action filterActions[3] = {
    {1, "By Id", 0},
    {2, "By Title", 1},
    {4, "Back"},
};

// Functions saved in array
void (*homeActions[6])() = {
    displayBook,
    searchBook,
    addBook,
    borrowBook,
    returnBook,
    sortBook,
};
#pragma endregion

// ========================================
// MAIN FUNCTION
// ========================================
int main()
{
    // Local variable declaration
    int choice;
    // Local variable initialization
    int stop = 0;

    // Load books data
    loadFile();

    while (stop != 1)
    {
        // 1. Display
        // 2. Search
        // 3. Add Book
        // 4. Borrow
        // 5. Return
        // 6. Sort
        // 7. Exit
        choice = printActions("LIBRARY SYSTEM", mainActions, 7);

        if (choice == 7)
        {
            printf("\n===== SYSTEM EXITED =====\n\n");
            stop = 1;
            return 0;
        }

        int acted = 0;

        for (int i = 0; i < 6; i++)
        {
            struct Action mainAct = mainActions[i];
            if (mainAct.command == choice)
            {
                acted = 1;
                homeActions[mainAct.functionIndex]();
            }
        }

        if (acted == 0)
            printf("Invalid Action\n");
    }

    waitForUserInput("exit");

    return 0;
}

#pragma region Helper functions
// Prints command action list
int printActions(char title[], struct Action actions[], int size)
{
    printf("===== %s =====\n\n", title);

    int choice;
    for (int i = 0; i < size; i++)
    {
        printf("%d. %s\n", actions[i].command, actions[i].title);
    }
    printf("\nEnter choice: ");
    scanf("%d", &choice);
    int commandFound = 0;
    for (int i = 0; i < size; i++)
    {
        if (actions[i].command == choice)
        {
            commandFound = 1;
        }
    }
    if (commandFound)
        return choice;
    else
        return -1;
}

// Waits for user input before proceeding
void waitForUserInput(char action[])
{
    printf("\nPress \"Enter\" to %s.", action);
    getchar();
    getchar();
    printf("\n");
}

// Prints filters based on choice & set searchParam for outside use
void filterPrints(int choice, char searchParam[])
{
    switch (choice)
    {
    case 1:
        printf("Enter Book Id: ");
        break;
    case 2:
        printf("Enter Book Title: ");
        break;
    case 3:
        printf("Enter Book Status: ");
        break;
    case 4:
        printf("Search book cancelled\n");
        strcpy(searchParam, "Exit");
        return;
    default:
        printf("Invalid Action\n");
        strcpy(searchParam, "Invalid");
        return;
    }

    scanf(" %[^\n]", searchParam); // get the input including the spaces
}

// Used by Borrow & Return book since both have similar function but different outcome
// title: "BORROW BOOK" | "RETURN BOOK"
// status: "Borrowed" | "Available"
void setBookStatus(char title[], char status[])
{
    while (1)
    {
        // 1. By Id
        // 2. By Title
        // 4. Back
        int choice = printActions(title, filterActions, 3);
        char searchParam[50];
        filterPrints(choice, searchParam);

        if (strcmp(searchParam, "Invalid") == 0)
            continue;

        if (strcmp(searchParam, "Exit") == 0)
            return;

        printf("\n===== Book Status Update =====\n");
        int found = 0;
        for (int i = 0; i < totalBook; i++)
        {
            int isSame = 1;
            struct Book book = library[i];

            switch (choice)
            {
            case 1:
                isSame = strcmp(searchParam, book.id);
                break;
            case 2:
                isSame = strcmp(searchParam, book.title);
                break;
            }

            if (isSame == 0)
            {
                found = 1;
                if (strcmp(book.status, status) == 0)
                {
                    printf("Invalid Action\n");
                }
                else
                {
                    printf("Old -> ID: %s TITLE: %s STATUS: %s\n", book.id, book.title, book.status);
                    strcpy(library[i].status, status);
                    printf("New -> ID: %s TITLE: %s STATUS: %s\n", library[i].id, library[i].title, library[i].status);
                }
            }
        }
        if (found == 0)
        {
            printf("No books found.\n");
        }

        waitForUserInput("continue");
    }
}

// Create a book sticker to be sent to printer (saved in "data/printer/" directory)
void createBookSticker(struct Book book)
{
    FILE *file;
    char path[100];

    sprintf(path, "%s/%s.txt", printerDir, book.title);

    file = fopen(path, "w");

    if (file == NULL)
        return;

    fprintf(file, "BOOK ID: %s\n", book.id);
    fprintf(file, "TITLE: %s\n", book.title);

    fclose(file);
}
#pragma endregion

#pragma region Main Functions
// Loads data from "data/books.txt"
void loadFile()
{
    FILE *file;
    file = fopen(booksDataPath, "r");
    if (file == NULL)
        return;
    while (fscanf(file, "%s %s %s", library[totalBook].id, library[totalBook].title, library[totalBook].status) != EOF)
    {
        totalBook++;
    }

    fclose(file);
}

// Saves data into "data/books.txt"
void saveFile()
{
    FILE *file;

    file = fopen(booksDataPath, "w");

    if (file == NULL)
        return;

    for (int i = 0; i < totalBook; i++)
    {
        fprintf(file, "%s %s %s\n", library[i].id, library[i].title, library[i].status);
    }

    fclose(file);

    printf("\nData saved successfully!\n");
}

// Display books data
void displayBook()
{
    printf("\n===== DISPLAY BOOKS =====\n\n");
    for (int i = 0; i < totalBook; i++)
    {
        struct Book book = library[i];
        printf("ID: %s TITLE: %s STATUS: %s\n", book.id, book.title, book.status);
    }

    waitForUserInput("back");
}

// Add books into data
void addBook()
{
    printf("\n===== ADD BOOK =====\n\n");

    printf("Enter Book ID: ");
    scanf("%s", library[totalBook].id);

    printf("Enter Book Title: ");
    scanf("%s", library[totalBook].title);

    strcpy(library[totalBook].status, "Available");
    createBookSticker(library[totalBook]);

    totalBook++;

    printf("\nBook added successfully!");
    saveFile();
    waitForUserInput("continue");
}

// Search for books by Id, Title or Status
void searchBook()
{
    while (1)
    {
        // 1. By Id
        // 2. By Title
        // 3. By Status
        // 4. Back
        int choice = printActions("SEARCH BOOK", searchActions, 4);
        char searchParam[50];
        filterPrints(choice, searchParam);

        if (strcmp(searchParam, "Invalid") == 0)
            continue;

        if (strcmp(searchParam, "Exit") == 0)
            return;

        printf("===== Books Found =====\n");
        int found = 0;
        for (int i = 0; i < totalBook; i++)
        {
            int isSame = 1;
            struct Book book = library[i];

            switch (choice)
            {
            case 1:
                isSame = strcmp(searchParam, book.id);
                break;
            case 2:
                isSame = strcmp(searchParam, book.title);
                break;
            case 3:
                isSame = strcmp(searchParam, book.status);
                break;
            }

            if (isSame == 0)
            {
                found = 1;
                printf("ID: %s TITLE: %s STATUS: %s\n", book.id, book.title, book.status);
            }
        }
        if (found == 0)
            printf("- No books found.");
        waitForUserInput("continue");
    }
}

// Set book's status to "Borrowed" is possible
void borrowBook()
{
    setBookStatus("BORROW BOOK", "Borrowed");
    saveFile();
}

// Set book's status to "Available" is possible
void returnBook()
{
    setBookStatus("RETURN BOOK", "Available");
    saveFile();
}

// Sorts books by Id, Title or Status
void sortBook()
{
    // 1. By Id
    // 2. By Title
    // 3. By Status
    // 4. Back
    int choice = printActions("SORT BOOK", searchActions, 4);

    if (choice == 4)
    {
        printf("Sort cancelled\n");
        return;
    }

    struct Book temp;

    for (int i = 0; i < totalBook - 1; i++)
    {
        for (int j = 0; j < totalBook - i - 1; j++)
        {
            int compare = 0;

            switch (choice)
            {
            case 1:
                compare = strcmp(library[j].id,
                                 library[j + 1].id);
                break;

            case 2:
                compare = strcmp(library[j].title,
                                 library[j + 1].title);
                break;

            case 3:
                compare = strcmp(library[j].status,
                                 library[j + 1].status);
                break;

            default:
                printf("Invalid action\n");
                return;
            }

            if (compare > 0)
            {
                temp = library[j];
                library[j] = library[j + 1];
                library[j + 1] = temp;
            }
        }
    }
    displayBook();
}
#pragma endregion