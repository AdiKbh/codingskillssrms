/*
student.c
New data format: id|reg_num|name|age|course_name|cgpa
Provides commands: add, list, get, update, delete
All outputs are JSON on stdout.
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DBFILE "students.txt"
#define MAXLINE 1024

// -------------------- Helpers --------------------

void json_error(const char *msg) {
    printf("{\"error\":\"%s\"}\n", msg);
}

void sanitize(char *str) {
    // Sanitizes input by replacing '|', newline, and carriage return
    for (char *p = str; *p; ++p) {
        if (*p == '|' || *p == '\n' || *p == '\r')
            *p = ' ';
    }
}

int next_id() {
    FILE *f = fopen(DBFILE, "r");
    if (!f) return 1;

    char line[MAXLINE];
    int max = 0;

    while (fgets(line, sizeof(line), f)) {
        char line_copy[MAXLINE];
        strncpy(line_copy, line, MAXLINE);
        line_copy[MAXLINE - 1] = 0;
        
        int id = atoi(strtok(line_copy, "|"));
        if (id > max) max = id;
    }

    fclose(f);
    return max + 1;
}

// -------------------- ADD --------------------

// Now takes cgpa (double)
void add_student(const char *reg_num, const char *name, int age, const char *course_name, double cgpa) {
    int id = next_id();
    FILE *f = fopen(DBFILE, "a");
    if (!f) {
        json_error("cannot open db");
        return;
    }

    char rncopy[MAXLINE]; strncpy(rncopy, reg_num, MAXLINE - 1); rncopy[MAXLINE - 1] = 0; sanitize(rncopy);
    char ncopy[MAXLINE]; strncpy(ncopy, name, MAXLINE - 1); ncopy[MAXLINE - 1] = 0; sanitize(ncopy);
    char ccopy[MAXLINE]; strncpy(ccopy, course_name, MAXLINE - 1); ccopy[MAXLINE - 1] = 0; sanitize(ccopy);

    // New file format: id|reg_num|name|age|course_name|cgpa
    fprintf(f, "%d|%s|%s|%d|%s|%.2f\n", id, rncopy, ncopy, age, ccopy, cgpa);
    fclose(f);

    printf("{\"id\":%d,\"reg_num\":\"%s\",\"name\":\"%s\",\"age\":%d,\"course_name\":\"%s\",\"cgpa\":%.2f}\n", 
           id, rncopy, ncopy, age, ccopy, cgpa);
}

// -------------------- LIST --------------------

void list_students() {
    FILE *f = fopen(DBFILE, "r");
    if (!f) {
        printf("[]\n");
        return;
    }

    char line[MAXLINE];
    int first = 1;

    printf("[\n");

    while (fgets(line, sizeof(line), f)) {
        char line_copy[MAXLINE];
        strncpy(line_copy, line, MAXLINE);
        line_copy[MAXLINE - 1] = 0;
        
        // Parse the line (6 fields now)
        char *id_str      = strtok(line_copy, "|");
        char *reg_num_str = strtok(NULL, "|");
        char *name_str    = strtok(NULL, "|");
        char *age_str     = strtok(NULL, "|");
        char *course_str  = strtok(NULL, "|");
        char *cgpa_str    = strtok(NULL, "|\n"); // Last field

        if (!id_str || !reg_num_str || !name_str || !age_str || !course_str || !cgpa_str) continue;

        if (!first) printf(",\n");
        first = 0;

        // Updated JSON output (uses "id")
        printf("{\"id\":%s,\"reg_num\":\"%s\",\"name\":\"%s\",\"age\":%s,\"course_name\":\"%s\",\"cgpa\":%s}",
               id_str, reg_num_str, name_str, age_str, course_str, cgpa_str);
    }

    printf("\n]");
    fclose(f);
}

// -------------------- GET --------------------

void get_student(int id) {
    FILE *f = fopen(DBFILE, "r");
    if (!f) {
        json_error("db missing");
        return;
    }

    char line[MAXLINE];
    while (fgets(line, sizeof(line), f)) {
        char line_copy[MAXLINE];
        strncpy(line_copy, line, MAXLINE);
        line_copy[MAXLINE - 1] = 0;

        int file_id = atoi(strtok(line_copy, "|"));
        char *reg_num = strtok(NULL, "|");
        char *name    = strtok(NULL, "|");
        char *age     = strtok(NULL, "|");
        char *course  = strtok(NULL, "|");
        char *cgpa    = strtok(NULL, "|\n");

        if (file_id == id) {
            printf("{\"id\":%d,\"reg_num\":\"%s\",\"name\":\"%s\",\"age\":%s,\"course_name\":\"%s\",\"cgpa\":%s}\n", 
                   file_id, reg_num, name, age, course, cgpa);
            fclose(f);
            return;
        }
    }

    fclose(f);
    json_error("student not found");
}

// -------------------- UPDATE --------------------

// Now takes cgpa (double)
void update_student(int id, const char *new_reg_num, const char *newname, int newage, const char *new_course_name, double new_cgpa) {
    FILE *f = fopen(DBFILE, "r");
    if (!f) {
        json_error("db missing");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        json_error("cannot write temp");
        fclose(f);
        return;
    }

    char line[MAXLINE];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {
        char line_copy[MAXLINE];
        strncpy(line_copy, line, MAXLINE);
        line_copy[MAXLINE - 1] = 0;

        int file_id = atoi(strtok(line_copy, "|"));
        char *reg_num = strtok(NULL, "|");
        char *name    = strtok(NULL, "|");
        char *age     = strtok(NULL, "|");
        char *course  = strtok(NULL, "|");
        char *cgpa    = strtok(NULL, "|\n");

        if (file_id == id) {
            found = 1;

            char rncopy[MAXLINE]; strncpy(rncopy, new_reg_num, MAXLINE - 1); rncopy[MAXLINE - 1] = 0; sanitize(rncopy);
            char ncopy[MAXLINE]; strncpy(ncopy, newname, MAXLINE - 1); ncopy[MAXLINE - 1] = 0; sanitize(ncopy);
            char ccopy[MAXLINE]; strncpy(ccopy, new_course_name, MAXLINE - 1); ccopy[MAXLINE - 1] = 0; sanitize(ccopy);

            // Write updated record
            fprintf(temp, "%d|%s|%s|%d|%s|%.2f\n", id, rncopy, ncopy, newage, ccopy, new_cgpa);

        } else {
            // Write original record (6 fields)
            fprintf(temp, "%d|%s|%s|%s|%s|%s\n", file_id, reg_num, name, age, course, cgpa);
        }
    }

    fclose(f);
    fclose(temp);

    remove(DBFILE);
    rename("temp.txt", DBFILE);

    if (!found) json_error("student not found");
    else printf("{\"status\":\"updated\"}\n");
}

// -------------------- DELETE --------------------
// (No change needed here, logic remains the same)
void delete_student(int id) {
    FILE *f = fopen(DBFILE, "r");
    if (!f) {
        json_error("db missing");
        return;
    }

    FILE *temp = fopen("temp.txt", "w");
    if (!temp) {
        json_error("cannot write temp");
        fclose(f);
        return;
    }

    char line[MAXLINE];
    int found = 0;

    while (fgets(line, sizeof(line), f)) {
        char line_copy[MAXLINE];
        strncpy(line_copy, line, MAXLINE);
        line_copy[MAXLINE - 1] = 0;

        int file_id = atoi(strtok(line_copy, "|"));
        
        // Save the rest of the tokens for re-writing
        char *reg_num = strtok(NULL, "|");
        char *name    = strtok(NULL, "|");
        char *age     = strtok(NULL, "|");
        char *course  = strtok(NULL, "|");
        char *cgpa    = strtok(NULL, "|\n");

        if (file_id == id) {
            found = 1;
            continue; // skip writing this record
        }

        // Write original record (6 fields)
        fprintf(temp, "%d|%s|%s|%s|%s|%s\n", file_id, reg_num, name, age, course, cgpa);
    }

    fclose(f);
    fclose(temp);

    remove(DBFILE);
    rename("temp.txt", DBFILE);

    if (!found) json_error("student not found");
    else printf("{\"status\":\"deleted\"}\n");
}

// -------------------- MAIN --------------------

int main(int argc, char *argv[]) {
    if (argc < 2) {
        json_error("no command");
        return 1;
    }

    // add reg_num name age course_name cgpa (6 arguments)
    if (strcmp(argv[1], "add") == 0) {
        if (argc != 7) { json_error("usage: add reg_num name age course_name cgpa"); return 1; }
        // cgpa is parsed as a double
        add_student(argv[2], argv[3], atoi(argv[4]), argv[5], atof(argv[6]));
    }

    // list
    else if (strcmp(argv[1], "list") == 0) {
        list_students();
    }

    // get id
    else if (strcmp(argv[1], "get") == 0) {
        if (argc != 3) { json_error("usage: get id"); return 1; }
        get_student(atoi(argv[2]));
    }

    // update id reg_num name age course_name cgpa (7 arguments)
    else if (strcmp(argv[1], "update") == 0) {
        if (argc != 8) { json_error("usage: update id reg_num name age course_name cgpa"); return 1; }
        // cgpa is parsed as a double
        update_student(atoi(argv[2]), argv[3], argv[4], atoi(argv[5]), argv[6], atof(argv[7]));
    }

    // delete id
    else if (strcmp(argv[1], "delete") == 0) {
        if (argc != 3) { json_error("usage: delete id"); return 1; }
        delete_student(atoi(argv[2]));
    }

    else {
        json_error("unknown command");
    }

    return 0;
}