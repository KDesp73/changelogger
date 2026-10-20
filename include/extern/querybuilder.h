#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_QUERY_SIZE 256
#define INITIAL_COLUMNS_SIZE 10
#define INITIAL_VALUES_SIZE 10

typedef struct {
    char *query;
    size_t query_capacity;
    char **columns;
    int columns_count;
    int columns_capacity;
    char *table;
    char *condition;
    char **values;
    int values_count;
    int values_capacity;
} query_builder_t;

#ifndef QBAPI
    #define QBAPI static
#endif

#pragma GCC diagnostic ignored "-Wunused-function"


QBAPI query_builder_t* create_query_builder();
QBAPI void free_query_builder(query_builder_t *qb);
QBAPI query_builder_t* select_q(query_builder_t *qb, const char *columns);
QBAPI query_builder_t* from_q(query_builder_t *qb, const char *table);
QBAPI query_builder_t* where_q(query_builder_t *qb, const char *condition);
QBAPI query_builder_t* insert_q(query_builder_t *qb, const char *table);
QBAPI query_builder_t* columns_q(query_builder_t *qb, const char *columns);
QBAPI query_builder_t* values_q(query_builder_t *qb, const char *values);
QBAPI query_builder_t* update_q(query_builder_t *qb, const char *table);
QBAPI query_builder_t* set_q(query_builder_t *qb, const char *column, const char *value);
QBAPI query_builder_t* delete_q(query_builder_t *qb, const char *table);
QBAPI char* build_query(query_builder_t *qb);
QBAPI int qb_append(query_builder_t *qb, const char *s);

/*
 * Append `s` to qb->query, growing the buffer as needed.
 * Returns 1 on success, 0 if the (re)allocation failed.
 */
QBAPI int qb_append(query_builder_t *qb, const char *s) 
{
    if (qb == NULL || qb->query == NULL || s == NULL) {
        return 0;
    }

    size_t len = strlen(qb->query);
    size_t add = strlen(s);

    if (len + add + 1 > qb->query_capacity) {
        size_t capacity = qb->query_capacity ? qb->query_capacity : INITIAL_QUERY_SIZE;
        while (len + add + 1 > capacity) {
            capacity *= 2;
        }

        char *resized = (char *)realloc(qb->query, capacity);
        if (resized == NULL) {
            return 0;
        }

        qb->query = resized;
        qb->query_capacity = capacity;
    }

    memcpy(qb->query + len, s, add + 1);
    return 1;
}

QBAPI query_builder_t* create_query_builder() 
{
    query_builder_t *qb = (query_builder_t *)malloc(sizeof(query_builder_t));
    qb->query = (char *)malloc(INITIAL_QUERY_SIZE);
    qb->query[0] = '\0'; // Initialize as empty string
    qb->query_capacity = INITIAL_QUERY_SIZE;
    qb->columns = (char **)malloc(INITIAL_COLUMNS_SIZE * sizeof(char *));
    qb->columns_count = 0;
    qb->columns_capacity = INITIAL_COLUMNS_SIZE;
    qb->table = NULL;
    qb->condition = NULL;
    qb->values = (char **)malloc(INITIAL_VALUES_SIZE * sizeof(char *));
    qb->values_count = 0;
    qb->values_capacity = INITIAL_VALUES_SIZE;
    return qb;
}

QBAPI void free_query_builder(query_builder_t *qb) 
{
    free(qb->query);
    for (int i = 0; i < qb->columns_count; i++) {
        free(qb->columns[i]);
    }
    free(qb->columns);
    free(qb->table);
    free(qb->condition);
    for (int i = 0; i < qb->values_count; i++) {
        free(qb->values[i]);
    }
    free(qb->values);
    free(qb);
}

QBAPI query_builder_t* select_q(query_builder_t *qb, const char *columns) 
{
    qb_append(qb, "SELECT ");
    if (strlen(columns) == 0) {
        qb_append(qb, "*");
    } else {
        qb_append(qb, columns);
    }
    return qb;
}

QBAPI query_builder_t* from_q(query_builder_t *qb, const char *table) 
{
    free(qb->table);
    qb->table = strdup(table);
    qb_append(qb, " FROM ");
    qb_append(qb, table);
    return qb;
}

QBAPI query_builder_t* where_q(query_builder_t *qb, const char *condition) 
{
    free(qb->condition);
    qb->condition = strdup(condition);
    qb_append(qb, " WHERE ");
    qb_append(qb, condition);
    return qb;
}

QBAPI query_builder_t* insert_q(query_builder_t *qb, const char *table)
{
    qb_append(qb, "INSERT INTO ");
    qb_append(qb, table);
    return qb;
}

QBAPI query_builder_t* columns_q(query_builder_t *qb, const char *columns)
{
    if (qb->columns_count >= qb->columns_capacity) {
        qb->columns_capacity *= 2;
        qb->columns = (char **)realloc(qb->columns, qb->columns_capacity * sizeof(char *));
    }
    char *columns_copy = strdup(columns);
    if (columns_copy == NULL) {
        return qb;
    }

    char *token = strtok(columns_copy, ", ");
    while (token != NULL) {
        qb->columns[qb->columns_count++] = strdup(token);
        token = strtok(NULL, ", ");
    }
    free(columns_copy);

    qb_append(qb, " (");
    for (int i = 0; i < qb->columns_count; i++) {
        qb_append(qb, qb->columns[i]);
        if (i < qb->columns_count - 1) {
            qb_append(qb, ", ");
        }
    }
    qb_append(qb, ")");
    return qb;
}

QBAPI query_builder_t* values_q(query_builder_t *qb, const char *values) 
{
    if (qb->values_count >= qb->values_capacity) {
        qb->values_capacity *= 2;
        qb->values = (char **)realloc(qb->values, qb->values_capacity * sizeof(char *));
    }

    char *values_copy = strdup(values); 
    if (values_copy == NULL) {
        return qb;
    }

    char *token = strtok(values_copy, ",");
    size_t value_count = qb->values_count;

    while (token != NULL) {
        if (value_count >= (size_t)qb->values_capacity) {
            qb->values_capacity *= 2;
            char **resized = (char **)realloc(qb->values, qb->values_capacity * sizeof(char *));
            if (resized == NULL) {
                free(values_copy);
                return qb;
            }
            qb->values = resized;
        }

        char *value = (char *)malloc(strlen(token) + 1);
        if (value == NULL) {
            free(values_copy);
            return qb;
        }
        strcpy(value, token);

        char *start = value;
        while (isspace((unsigned char)*start)) {
            start++;
        }
        char *end = value + strlen(value) - 1;
        while (end > start && isspace((unsigned char)*end)) {
            *end-- = '\0';
        }

        /* Shift the trimmed content to the start of the block so the
           pointer we store is always the one that must be freed. */
        if (start != value) {
            memmove(value, start, strlen(start) + 1);
        }

        qb->values[value_count++] = value;
        token = strtok(NULL, ",");
    }

    qb->values_count = value_count;

    qb_append(qb, " VALUES (");
    for (int i = 0; i < qb->values_count; i++) {
        qb_append(qb, qb->values[i]);
        if (i < qb->values_count - 1) {
            qb_append(qb, ", ");
        }
    }
    qb_append(qb, ")");

    free(values_copy);

    return qb;
}
QBAPI query_builder_t* update_q(query_builder_t *qb, const char *table)
{
    free(qb->table);
    qb->table = strdup(table);
    qb_append(qb, "UPDATE ");
    qb_append(qb, table);
    return qb;
}

QBAPI query_builder_t* set_q(query_builder_t *qb, const char *column, const char *value) 
{
    qb_append(qb, " SET ");
    qb_append(qb, column);
    qb_append(qb, " = ");
    qb_append(qb, value);
    return qb;
}

QBAPI query_builder_t* delete_q(query_builder_t *qb, const char *table) 
{
    free(qb->table);
    qb->table = strdup(table);
    qb_append(qb, "DELETE FROM ");
    qb_append(qb, table);
    return qb;
}

QBAPI char* build_query(query_builder_t *qb) 
{
    if (qb->query == NULL) {
        fprintf(stderr, "Incomplete query construction.\n");
        exit(EXIT_FAILURE);
    }
    char *queryString = strdup(qb->query);
    free(qb->query);
    qb->query = (char *)malloc(INITIAL_QUERY_SIZE);
    qb->query[0] = '\0'; // Reset the query
    qb->query_capacity = INITIAL_QUERY_SIZE;
    return queryString;
}

#endif // QUERYBUILDER_H
