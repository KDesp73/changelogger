#ifndef QUERYBUILDER_H
#define QUERYBUILDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INITIAL_QUERY_SIZE 256
#define INITIAL_COLUMNS_SIZE 10
#define INITIAL_VALUES_SIZE 10

typedef struct {
    char *query;
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

QBAPI query_builder_t* create_query_builder() 
{
    query_builder_t *qb = (query_builder_t *)malloc(sizeof(query_builder_t));
    qb->query = (char *)malloc(INITIAL_QUERY_SIZE);
    qb->query[0] = '\0'; // Initialize as empty string
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
    strcat(qb->query, "SELECT ");
    if (strlen(columns) == 0) {
        strcat(qb->query, "*");
    } else {
        strcat(qb->query, columns);
    }
    return qb;
}

QBAPI query_builder_t* from_q(query_builder_t *qb, const char *table) 
{
    qb->table = strdup(table);
    strcat(qb->query, " FROM ");
    strcat(qb->query, table);
    return qb;
}

QBAPI query_builder_t* where_q(query_builder_t *qb, const char *condition) 
{
    qb->condition = strdup(condition);
    strcat(qb->query, " WHERE ");
    strcat(qb->query, condition);
    return qb;
}

QBAPI query_builder_t* insert_q(query_builder_t *qb, const char *table)
{
    strcat(qb->query, "INSERT INTO ");
    strcat(qb->query, table);
    return qb;
}

QBAPI query_builder_t* columns_q(query_builder_t *qb, const char *columns)
{
    if (qb->columns_count >= qb->columns_capacity) {
        qb->columns_capacity *= 2;
        qb->columns = (char **)realloc(qb->columns, qb->columns_capacity * sizeof(char *));
    }
    char *token = strtok(strdup(columns), ", ");
    while (token != NULL) {
        qb->columns[qb->columns_count++] = strdup(token);
        token = strtok(NULL, ", ");
    }
    strcat(qb->query, " (");
    for (int i = 0; i < qb->columns_count; i++) {
        strcat(qb->query, qb->columns[i]);
        if (i < qb->columns_count - 1) {
            strcat(qb->query, ", ");
        }
    }
    strcat(qb->query, ")");
    return qb;
}

QBAPI query_builder_t* values_q(query_builder_t *qb, const char *values) 
{
    if (qb->values_count >= qb->values_capacity) {
        qb->values_capacity *= 2;
        qb->values = (char **)realloc(qb->values, qb->values_capacity * sizeof(char *));
    }
    char *token = strtok(strdup(values), ", ");
    while (token != NULL) {
        if (token[0] == '\"' || token[0] == '\'') {
            qb->values[qb->values_count++] = strdup(token);
        } else {
            char *value = (char *)malloc(20);
            sprintf(value, "%s", token);
            qb->values[qb->values_count++] = value;
        }
        token = strtok(NULL, ", ");
    }
    strcat(qb->query, " VALUES (");
    for (int i = 0; i < qb->values_count; i++) {
        strcat(qb->query, qb->values[i]);
        if (i < qb->values_count - 1) {
            strcat(qb->query, ", ");
        }
    }
    strcat(qb->query, ")");
    return qb;
}

QBAPI query_builder_t* update_q(query_builder_t *qb, const char *table)
{
    qb->table = strdup(table);
    strcat(qb->query, "UPDATE ");
    strcat(qb->query, table);
    return qb;
}

QBAPI query_builder_t* set_q(query_builder_t *qb, const char *column, const char *value) 
{
    strcat(qb->query, " SET ");
    strcat(qb->query, column);
    strcat(qb->query, " = ");
    if (value[0] == '\"' || value[0] == '\'') {
        strcat(qb->query, value);
    } else {
        strcat(qb->query, value);
    }
    return qb;
}

QBAPI query_builder_t* delete_q(query_builder_t *qb, const char *table) 
{
    qb->table = strdup(table);
    strcat(qb->query, "DELETE FROM ");
    strcat(qb->query, table);
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
    return queryString;
}

#endif // QUERYBUILDER_H
