#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LENGTH 2048
#define MAX_DATE_LENGTH 11

int total_entries;

const char *morsecodeletters[26] =
{
    ". -", "- . . .", "- . - .", "- . .", ".", ". . - .", "- - .", ". . . .", ". .", ". - - -",
    "- . -", ". - . .", "- -", "- .", "- - -", ". - - .", "- - . -", ". - .", ". . .", "-",
    ". . -", ". . . -", ". - -", "- . . -", "- . - -", "- - . ."
};

const char *morsecodedigits[10] =
{
    "- - - - -", ". - - - -", ". . - - -", ". . . - -", ". . . . -", ". . . . .", "- . . . .", "- - . . .",
    "- - - . .", "- - - - ."
};

struct DiaryEntry
{
    char content[MAX_LENGTH];
    char morse[MAX_LENGTH*7];
    char date[MAX_DATE_LENGTH];
};

void red()
{
    printf("\033[1;31m");
}
void yellow()
{
    printf("\033[1;33m");
}
void green()
{
    printf("\033[1;32m");
}
void blue()
{
    printf("\033[1;34m");
}
void reset()
{
    printf("\033[0m");
}

void highlight_word(const char *line, const char *word, const char *color_code)
{
    const char *p = line;
    size_t word_len = strlen(word);

    while (*p)
    {

        if (strncasecmp(p, word, word_len) == 0 &&
                (p == line || !isalnum((unsigned char)*(p - 1))) &&
                !isalnum((unsigned char)p[word_len]))
        {

            printf("\033[%sm%.*s\033[0m", color_code, (int)word_len, p);
            p += word_len;
        }
        else
        {
            putchar(*p);
            p++;
        }
    }
}

char* convert_to_morse(const char *s)
{
    char *morse = (char*)malloc(1024 * sizeof(char));
    morse[0] = '\0';
    for (int i = 0; i < strlen(s); i++)
    {
        char c = tolower(s[i]);
        if (c >= 'a' && c <= 'z')
            strcat(morse, morsecodeletters[c - 'a']);
        else if (c >= '0' && c <= '9')
            strcat(morse, morsecodedigits[c - '0']);
        else if (c == ' ')
            strcat(morse, "       ");
        else
        {
            char aux[] = {c, '\0'};
            strcat(morse, aux);
        }
        strcat(morse, "   ");
    }
    return morse;
}

int validate_date(const char *date)
{
    if (strlen(date) != 10) return 0;
    if (date[2] != '/' || date[5] != '/') return 0;
    for (int i = 0; i < 10; i++)
    {
        if ((i != 2 && i != 5) && !isdigit(date[i])) return 0;
    }
    int day = atoi(date);
    int month = atoi(date + 3);
    if (month < 1 || month > 12) return -1;
    if (day < 1 || day > 31) return -1;
    return 1;
}

int validate_entry(const char *entry)
{
    for (int i = 0; i < strlen(entry); i++)
    {
        char c = entry[i];
        if (!(isalnum(c) || isspace(c) || ispunct(c)))
        {
            return 0;
        }
    }
    return 1;
}

int total()
{
    int total_entries = 0;
    FILE *rnumber = fopen("numberofentries.txt", "r");
    if (rnumber == NULL) return 0;
    if (fscanf(rnumber, "%d", &total_entries) != 1) total_entries = 0;
    fclose(rnumber);
    return total_entries;
}

void clear_screen()
{
    printf("\033[H\033[J");
}

void header()
{
    clear_screen();
    printf("-------MORSE CODE DIARY-------\n");
    blue();
    printf("Hello! ");
    reset();
    printf("Welcome to your own ");
    blue();
    printf("Morse Code diary");
    reset();
    printf("!\n");
    printf("Please select an ");
    blue();
    printf("option");
    reset();
    printf(":\n");
    printf("1->");
    yellow();
    printf(" Add Entry\n");
    reset();
    printf("2->");
    yellow();
    printf(" View Diary\n");
    reset();
    printf("3->");
    yellow();
    printf(" Search in Diary\n");
    reset();
    printf("0->");
    yellow();
    printf(" Close Diary\n");
    reset();
    printf("------------------------------\n");
}

void save_entry(struct DiaryEntry entry, int total_entries)
{
    FILE *efile = fopen("englishdiary.txt", "a");
    FILE *mfile = fopen("morsediary.txt", "a");

    if (efile)
    {
        fprintf(efile, "Entry %d [%s]: %s\n", total_entries + 1, entry.date, entry.content);
        fclose(efile);
    }

    if (mfile)
    {
        fprintf(mfile, "Entry %d [%s]: %s\n", total_entries + 1, entry.date, entry.morse);
        fclose(mfile);
    }
}

void add_entry()
{

    printf("---ADD DIARY ENTRY---\n");


    struct DiaryEntry entry;
    int valid_date = 0;

    while (valid_date == 0 || valid_date == -1)
    {
        printf("Enter the entry date ");
        yellow();
        printf("(DD/MM/YYYY)");
        reset();
        printf(":");
        scanf("%s", entry.date);
        valid_date = validate_date(entry.date);
        if (valid_date == 0)
        {
            red();
            printf("Invalid date format.");
            reset();
            printf("Please enter the date in the format ");
            red();
            printf("DD/MM/YYYY");
            reset();
            printf("!\n");
        }
        else if (valid_date == -1)
        {
            red();
            printf("Invalid date.");
            reset();
            printf("Please enter a valid date.\n");
        }
    }

    getchar();

    int valid_entry = 0;
    while (valid_entry == 0)
    {

        printf("Enter your diary entry ");
        yellow();
        printf("(Use letters, numbers, spaces and punctuation)");
        reset();
        printf(":");

        fgets(entry.content, MAX_LENGTH, stdin);
        entry.content[strcspn(entry.content, "\n")] = 0;
        valid_entry = validate_entry(entry.content);
        if (!valid_entry)
        {
            red();
            printf("Invalid characters in entry. Please only use letters, numbers, spaces, and punctuation.\n");
            reset();
        }
    }

    char *morse_temp = convert_to_morse(entry.content);
    strncpy(entry.morse, morse_temp, sizeof(entry.morse) - 1);
    free(morse_temp);

    blue();
    printf("Your Entry in Morse Code: %s\n", entry.morse);
    reset();

    total_entries = total();
    save_entry(entry, total_entries);

    FILE *wnumber = fopen("numberofentries.txt", "w");
    fprintf(wnumber, "%d", total_entries + 1);
    fclose(wnumber);


    printf("\nPress");
    green();
    printf(" ENTER ");
    reset();
    printf("to continue...\n");
    reset();
    getchar();
}

char *to_lower_str(const char *s)
{
    char *lower = malloc(strlen(s) + 1);
    if (!lower) return NULL;
    for (int i = 0; s[i]; i++)
    {
        lower[i] = tolower((unsigned char)s[i]);
    }
    lower[strlen(s)] = '\0';
    return lower;
}




void view_diary() {
    FILE *file = fopen("englishdiary.txt", "r");
    char line[MAX_LENGTH];
    while (fgets(line, sizeof(line), file)) {
        char entry_number[16];
        char date[16];
        char *content;

        if (sscanf(line, "Entry %15s [%15[^]]]:", entry_number, date) == 2) {

            printf("\033[1;34mEntry %s\033[0m ", entry_number);
            printf("\033[1;33m[%s]\033[0m:\033[0m ", date);
            content = strchr(line, ':');
            if (content) {
                content++;
                highlight_word(content, "Highlight", "1;33");
            }

        } else {

            printf("%s", line);
        }
    }

    fclose(file);
    printf("\nPress");
    green();
    printf(" ENTER ");
    reset();
    printf("to continue...\n");
    reset();
    getchar();
}
void search_in_diary()
{
    printf("---SEARCH IN DIARY---\n");
    printf("Please enter a word: ");

    char word[256], c, line[MAX_LENGTH];
    int found = 0;

    scanf("%s", word);
    while ((c = getchar()) != '\n' && c != EOF);

    char *lower_word = to_lower_str(word);

    FILE *renglish = fopen("englishdiary.txt", "r");
    printf("All entries with the word '%s' are:\n", word);

    while (fgets(line, sizeof(line), renglish))
    {
        char *lower_line = to_lower_str(line);
        if (!lower_line) continue;

        if (strstr(lower_line, lower_word))
        {
            highlight_word(line, word,"1;33");
            found = 1;
        }

        free(lower_line);
    }

    fclose(renglish);
    free(lower_word);

    if (!found)
    {
        red();
        printf("No entries found with the word '%s'.\n", word);
        reset();
    }

    printf("\nPress");
    green();
    printf(" ENTER ");
    reset();
    printf("to continue...\n");
    reset();
    getchar();
}

void menu(int op)
{
    getchar();
    switch (op)
    {
    case 1:
        add_entry();
        break;
    case 2:
        view_diary();
        break;
    case 3:
        search_in_diary();
        break;
    default:
        red();
        printf("CLOSING DIARY\n");
        reset();
        printf("\nPress");
        green();
        printf(" ENTER ");
        reset();
        printf("to continue...\n");
        reset();
    }
}

int main()
{
    int option;
    do
    {
        header();
        green();
        printf("Enter option: ");
        reset();
        scanf("%d", &option);
        clear_screen();
        menu(option);
    }
    while (option > 0 && option <= 3);
    return 0;
}
