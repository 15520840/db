#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>


typedef struct InputBuffer {
        char* buffer;
        size_t buffer_length;
        ssize_t input_length;
} InputBuffer;

typedef enum MetaCommandResult_t {
                          META_COMMAND_SUCCESS,
                          META_COMMAND_UNRECOGNIZED_COMMAND
} MetaCommandResult;

typedef enum PrepareResult {
                            PREPARE_SUCCESS,
                            PREPARE_UNRECOGNIZED_STATEMENT,
                            PREPARE_SYNTAX_ERROR
} PrepareResult;

typedef enum StatementType {
                            STATEMENT_INSERT,
                            STATEMENT_SELECT
} StatementType;

#define COLUMN_USERNAME_SIZE 32
#define COLUMN_EMAIL_SIZE 255

typedef struct Row {
        uint32_t id;
        char username[COLUMN_USERNAME_SIZE];
        char email[COLUMN_EMAIL_SIZE];
} Row;

typedef struct Statement {
        StatementType type;
        Row row_to_insert;
} Statement;

#define size_of_attribute

InputBuffer* new_input_buffer(void);
void print_prompt(void);
void read_input(InputBuffer* input_buffer);
void close_input_buffer(InputBuffer* input_buffer);
MetaCommandResult do_meta_command(InputBuffer* input_buffer);
void execute_statement(Statement* statement);
PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement);

int main(int argc, char* argv[])
{
        InputBuffer* input_buffer = new_input_buffer();

        while (true) {
                print_prompt();
                read_input(input_buffer);

                if (input_buffer->buffer[0] == '.') {
                        switch (do_meta_command(input_buffer)) {
                        case (META_COMMAND_SUCCESS):
                                continue;
                        case (META_COMMAND_UNRECOGNIZED_COMMAND):
                                printf("Unrecognized command '%s'\n", input_buffer->buffer);
                                continue;
                        }

                }

                Statement statement;
                switch (prepare_statement(input_buffer, &statement)) {
                case (PREPARE_SUCCESS):
                        break;
                case (PREPARE_UNRECOGNIZED_STATEMENT):
                        printf("Unrecognized keyword at start of '%s'\n", input_buffer->buffer);
                        continue;
                }

                execute_statement(&statement);
                printf("Executed\n");
        }

}
InputBuffer* new_input_buffer(void)
{
        InputBuffer* input_buffer = malloc(sizeof(InputBuffer));
        input_buffer->buffer = NULL;
        input_buffer->buffer_length = 0;
        input_buffer->input_length = 0;

        return input_buffer;
}


void print_prompt(void)
{
        printf("db> ");
}

void read_input(InputBuffer* input_buffer)
{
        ssize_t bytes_read = getline(&(input_buffer->buffer), &(input_buffer->buffer_length), stdin);

        if (bytes_read <= 0) {
                fprintf(stderr, "Error reading input\n");
                exit(EXIT_FAILURE);
        }

        // Ignore trailing newline
        input_buffer->input_length = bytes_read - 1;
        input_buffer->buffer[bytes_read - 1] = 0;
}

void close_input_buffer(InputBuffer* input_buffer)
{
        free(input_buffer->buffer);
        free(input_buffer);
}

MetaCommandResult do_meta_command(InputBuffer* input_buffer)
{
        if (strcmp(input_buffer->buffer, ".exit") == 0) {
                close_input_buffer(input_buffer);
                exit(EXIT_SUCCESS);
        } else {
                return META_COMMAND_UNRECOGNIZED_COMMAND;
        }
}

void execute_statement(Statement* statement)
{
        switch (statement->type) {
        case (STATEMENT_INSERT):
                printf("This is where we would do an insert\n");
                break;
        case (STATEMENT_SELECT):
                printf("This is where we would do an select\n");
                break;
        }
}

PrepareResult prepare_statement(InputBuffer* input_buffer, Statement* statement)
{
        if (strncmp(input_buffer->buffer, "insert", 6)) {
                statement->type = STATEMENT_INSERT;

                int args_assigned = sscanf(input_buffer->buffer, "insert %d %s %s", &(statement->row_to_insert.id),
                                                                                      statement->row_to_insert.username,
                                                                                      statement->row_to_insert.email);

                if (args_assigned < 3) {
                        return PREPARE_SYNTAX_ERROR;
                }
                return PREPARE_SUCCESS;
        }

        if (strncmp(input_buffer->buffer, "select", 6)) {
                statement->type = STATEMENT_SELECT;
                return PREPARE_SUCCESS;
        }

        return PREPARE_UNRECOGNIZED_STATEMENT;
}

