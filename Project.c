#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUS_COUNT 4
#define MAX_BOOKINGS 100
#define TEXT_LENGTH 64

typedef struct
{
    int id;
    char source[TEXT_LENGTH];
    char destination[TEXT_LENGTH];
    char departure[TEXT_LENGTH];
    int total_seats;
    int available_seats;
} Bus;

typedef struct
{
    int id;
    int bus_id;
    char passenger[TEXT_LENGTH];
    char phone[TEXT_LENGTH];
    int seats;
    int active;
} Booking;

static Bus buses[BUS_COUNT] = {
    {101, "Delhi", "Jaipur", "08:00", 40, 40},
    {102, "Mumbai", "Pune", "09:30", 36, 36},
    {103, "Bengaluru", "Chennai", "11:00", 40, 40},
    {104, "Delhi", "Agra", "14:15", 32, 32}};
static Booking bookings[MAX_BOOKINGS];
static int booking_count;
static int next_booking_id = 1001;

static int read_line(const char *prompt, char *buffer, size_t size)
{
    size_t length;
    int character;

    printf("%s", prompt);
    if (fgets(buffer, (int)size, stdin) == NULL)
    {
        return 0;
    }

    length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n')
    {
        buffer[length - 1] = '\0';
    }
    else
    {
        while ((character = getchar()) != '\n' && character != EOF)
        {
        }
    }

    return 1;
}

static int read_integer(const char *prompt, int minimum, int maximum, int *value)
{
    char input[TEXT_LENGTH];
    char *end;
    long parsed;

    for (;;)
    {
        if (!read_line(prompt, input, sizeof(input)))
        {
            return 0;
        }

        parsed = strtol(input, &end, 10);
        while (isspace((unsigned char)*end))
        {
            end++;
        }

        if (input[0] != '\0' && *end == '\0' &&
            parsed >= minimum && parsed <= maximum)
        {
            *value = (int)parsed;
            return 1;
        }

        printf("Please enter a number from %d to %d.\n", minimum, maximum);
    }
}

static int read_nonempty_text(const char *prompt, char *value, size_t size)
{
    for (;;)
    {
        if (!read_line(prompt, value, size))
        {
            return 0;
        }

        if (value[0] != '\0')
        {
            return 1;
        }

        printf("This field cannot be empty.\n");
    }
}

static int same_text(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0')
    {
        if (tolower((unsigned char)*left) != tolower((unsigned char)*right))
        {
            return 0;
        }
        left++;
        right++;
    }

    return *left == '\0' && *right == '\0';
}

static Bus *find_bus(int bus_id)
{
    int i;

    for (i = 0; i < BUS_COUNT; i++)
    {
        if (buses[i].id == bus_id)
        {
            return &buses[i];
        }
    }
    return NULL;
}

static void print_bus(const Bus *bus)
{
    printf("%-6d %-18s %-18s %-10s %d/%d\n",
           bus->id, bus->source, bus->destination, bus->departure,
           bus->available_seats, bus->total_seats);
}

static void print_bus_heading(void)
{
    printf("\n%-6s %-18s %-18s %-10s %s\n",
           "ID", "From", "To", "Departure", "Seats available/total");
}

static void view_buses(void)
{
    int i;

    print_bus_heading();
    for (i = 0; i < BUS_COUNT; i++)
    {
        print_bus(&buses[i]);
    }
}

static void book_bus(void)
{
    int bus_id;
    int seats;
    Bus *bus;
    Booking *booking;

    if (booking_count >= MAX_BOOKINGS)
    {
        printf("\nBooking list is full. No more bookings can be made.\n");
        return;
    }

    view_buses();
    if (!read_integer("\nEnter the bus ID to book: ", 1, 999999, &bus_id))
    {
        return;
    }
    bus = find_bus(bus_id);
    if (bus == NULL)
    {
        printf("Bus ID %d was not found.\n", bus_id);
        return;
    }

    if (bus->available_seats == 0)
    {
        printf("This bus is fully booked.\n");
        return;
    }

    if (!read_nonempty_text("Enter passenger name: ",
                            bookings[booking_count].passenger, TEXT_LENGTH) ||
        !read_nonempty_text("Enter contact phone number: ",
                            bookings[booking_count].phone, TEXT_LENGTH) ||
        !read_integer("Number of seats to book: ", 1, bus->available_seats, &seats))
    {
        return;
    }

    booking = &bookings[booking_count];
    booking->id = next_booking_id++;
    booking->bus_id = bus->id;
    booking->seats = seats;
    booking->active = 1;
    bus->available_seats -= seats;
    booking_count++;

    printf("\nBooking successful. Booking ID: %d\n", booking->id);
    printf("Passenger: %s | Bus: %d | Seats: %d\n",
           booking->passenger, booking->bus_id, booking->seats);
}

static void cancel_booking(void)
{
    int booking_id;
    int i;
    Booking *booking = NULL;
    Bus *bus;

    if (!read_integer("\nEnter the booking ID to cancel: ",
                      1, 999999, &booking_id))
    {
        return;
    }

    for (i = 0; i < booking_count; i++)
    {
        if (bookings[i].id == booking_id && bookings[i].active)
        {
            booking = &bookings[i];
            break;
        }
    }

    if (booking == NULL)
    {
        printf("Active booking ID %d was not found.\n", booking_id);
        return;
    }

    bus = find_bus(booking->bus_id);
    if (bus != NULL)
    {
        bus->available_seats += booking->seats;
    }
    booking->active = 0;
    printf("Booking %d for %s was cancelled. %d seat(s) returned.\n",
           booking->id, booking->passenger, booking->seats);
}

static void search_buses(void)
{
    char source[TEXT_LENGTH];
    char destination[TEXT_LENGTH];
    int i;
    int found = 0;

    if (!read_nonempty_text("\nEnter departure city: ", source, sizeof(source)) ||
        !read_nonempty_text("Enter destination city: ",
                            destination, sizeof(destination)))
    {
        return;
    }

    print_bus_heading();
    for (i = 0; i < BUS_COUNT; i++)
    {
        if (same_text(buses[i].source, source) &&
            same_text(buses[i].destination, destination))
        {
            print_bus(&buses[i]);
            found = 1;
        }
    }

    if (!found)
    {
        printf("No buses found for %s to %s.\n", source, destination);
    }
}

int main(void)
{
    int choice;

    do
    {
        printf("\n===== Bus Booking Menu =====\n");
        printf("1. View buses\n");
        printf("2. Book a bus\n");
        printf("3. Cancel a booking\n");
        printf("4. Search for a bus\n");
        printf("5. Exit\n");

        if (!read_integer("Enter your choice (1-5): ", 1, 5, &choice))
        {
            printf("\nInput ended. Exiting the bus booking system.\n");
            break;
        }

        switch (choice)
        {
        case 1:
            view_buses();
            break;
        case 2:
            book_bus();
            break;
        case 3:
            cancel_booking();
            break;
        case 4:
            search_buses();
            break;
        case 5:
            printf("\nThank you for using the bus booking system. Goodbye!\n");
            break;
        }
    } while (choice != 5);

    return 0;
}
