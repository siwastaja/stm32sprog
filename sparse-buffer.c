#include "sparse-buffer.h"

#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HEIGHT 16

typedef struct Node Node;
struct Node {
    size_t offset;
    uint8_t *data;
    size_t n;

    int height;
    Node *prev[MAX_HEIGHT];
    Node *next[MAX_HEIGHT];
};

struct SparseBuffer {
    Node *begin;
};

static void *memdup(const void *data, size_t n);
static bool overlap(size_t offset1, size_t n1, size_t offset2, size_t n2);

static int randomHeight();
static void insertNode(Node *node, Node *prev[MAX_HEIGHT]);
static void removeNode(Node *node);

static Node *Node_create();
static void Node_addData(Node *node, size_t offset, uint8_t *data, size_t n);

SparseBuffer *SparseBuffer_create() {
    SparseBuffer *self = malloc(sizeof(SparseBuffer));
    if(!self) abort();

    self->begin = Node_create();
    self->begin->height = MAX_HEIGHT;

    return self;
}

void SparseBuffer_destroy(SparseBuffer *self) {
    Node *node = self->begin;
    while(node != NULL) {
        Node *next = node->next[0];
        free(node);
        node = next;
    }
    free(self);
}

void SparseBuffer_set(SparseBuffer *self,
        size_t offset, uint8_t *data, size_t n) {
    Node *prev[MAX_HEIGHT];
    Node *node = self->begin;
    int level = MAX_HEIGHT - 1;

    while(level >= 0) {
        Node *next = node->next[level];
        if(!next) {
            prev[level] = node;
            level--;
        } else if(overlap(offset, n, next->offset, next->n)) {
            Node_addData(next, offset, data, n);
            node = next->next[0];
            if(node && overlap(node->offset, node->n, next->offset, next->n)) {
                Node_addData(next, node->offset, node->data, node->n);
                removeNode(node);
            }
            return;
        } else if(offset > next->offset) {
            node = node->next[level];
        } else {
            prev[level] = node;
            level--;
        }
    }

    node = Node_create();
    Node_addData(node, offset, data, n);
    insertNode(node, prev);
}

static void *memdup(const void *data, size_t n) {
    void *copy = malloc(n);
    if(!copy) abort();
    return memcpy(copy, data, n);
}

static bool overlap(size_t offset1, size_t n1, size_t offset2, size_t n2) {
    size_t end1 = offset1 + n1;
    size_t end2 = offset2 + n2;

    if((offset2 <= offset1) && (offset1 <= end2)) return true;
    if((offset2 <= end1) && (end1 <= end2)) return true;
    if((offset1 <= offset2) && (offset2 <= end1)) return true;
    if((offset1 <= end2) && (end2 <= end1)) return true;

    return false;
}

static int randomHeight() {
    int r = rand();
    int pivot = (RAND_MAX / 2) + 1;
    int height = 1;

    while(r < pivot && height < MAX_HEIGHT) {
        height++;
        pivot /= 2;
    }

    return height;
}

static void insertNode(Node *node, Node *prev[MAX_HEIGHT]) {
    for(int i = 0; i < node->height; ++i) {
        Node *p = prev[i];
        Node *n = p->next[i];

        node->prev[i] = p;
        node->next[i] = n;
        p->next[i] = node;
        if(n) n->prev[i] = node;
    }
}

static void removeNode(Node *node) {
    for(int i = 0; i < node->height; ++i) {
        Node *p = node->prev[i];
        Node *n = node->next[i];
        p->next[i] = n;
        if(n) n->prev[i] = p;
    }
    free(node);
}

static Node *Node_create() {
    Node *self = malloc(sizeof(Node));
    if(!self) abort();

    self->offset = 0;
    self->data = NULL;
    self->n = 0;

    self->height = randomHeight();
    for(int i = 0; i < MAX_HEIGHT; ++i) {
        self->prev[i] = NULL;
        self->next[i] = NULL;
    }

    return self;
}

static void Node_addData(Node *self, size_t offset, uint8_t *data, size_t n) {
    if(!self->data) {
        self->offset = offset;
        self->data = memdup(data, n);
        self->n = n;
        return;
    }

    assert(overlap(offset, n, self->offset, self->n));

    size_t end = offset + n;
    size_t nodeEnd = self->offset + self->n;

    if(offset >= self->offset) {
        if(end > nodeEnd) {
            self->n = end - self->offset;
            self->data = realloc(self->data, self->n);
            if(!self->data) abort();
        }
        size_t diff = offset - self->offset;
        memcpy(self->data + diff, data, n);
    } else {
        size_t nTotal = (end >= nodeEnd) ? n : nodeEnd - offset;
        self->data = realloc(self->data, nTotal);
        if(!self->data) abort();
        if(end < nodeEnd) {
            // TODO: Avoid copying data that will be overwritten anyway.
            size_t diff = self->offset - offset;
            memmove(self->data + diff, self->data, self->n);
        }
        memcpy(self->data, data, n);
        self->offset = offset;
        self->n = nTotal;
    }
}

