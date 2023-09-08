#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"
#include "priority.h"


Queue *initializeQueue() {
    Queue *queue = new Queue;
    queue->head = NULL;
    queue->size = 0;

    return queue;
} 

CharacterQueue *initializeCharacterQueue() {
    CharacterQueue *queue = new CharacterQueue;
    queue->head = NULL;
    queue->size = 0;

    return queue;
}

void deallocateCharacterQueue(CharacterQueue *characterQueue) {
    if(characterQueue->size == 0) {
        free(characterQueue);
        return;
    } else {
        CharacterQueueNode *prev = characterQueue->head;
        CharacterQueueNode *current;
        for(int i = 1; i < characterQueue->size; i++) {
            current = prev->next;
            free(prev);
            prev = current;
        }
        free(prev);
    }
}

QueueNode *initializeQueueNode(int x, int y, int priority) {
    QueueNode *node = new QueueNode;
    node->next = NULL;
    node->priority = priority;
    node->x = x;
    node->y = y;
}

CharacterQueueNode *initializeCharacterQueueNode(int priority, Character *character) {
    CharacterQueueNode *node = new CharacterQueueNode;
    node->next = NULL;
    node->priority = priority;
    node->character = character;
}

QueueNode popQueue(Queue *queue) {
    QueueNode *head = queue->head;
    QueueNode headReturn = *(queue->head);
    queue->head = queue->head->next;
    queue->size--;
    free(head);

    return headReturn;
}

CharacterQueueNode popCharacterQueue(CharacterQueue *queue) {
    CharacterQueueNode *head = queue->head;
    CharacterQueueNode headReturn = *(queue->head);
    queue->head = queue->head->next;
    queue->size--;
    free(head);

    return headReturn;
}

int addQueue(Queue *queue, QueueNode *node) {
    if(queue->size == 0) {
        queue->head = node;
        queue->size++;
        return queue->size;
    }

    QueueNode *currentNode = queue->head;

    if(node->priority <= currentNode->priority) {
        node->next = currentNode;
        queue->head = node;
        queue->size++;
        
        return queue->size;
    }

    
    while(currentNode->next != NULL) {
        QueueNode *previousNode = currentNode;
        currentNode = currentNode->next;

        if(node->priority <= currentNode->priority) {
            node->next = currentNode;
            previousNode->next = node;
            queue->size++;
            
            return queue->size;
        }
    }

    currentNode->next = node;
    queue->size++;

    return queue->size;
}

int addCharacterQueue(CharacterQueue *queue, CharacterQueueNode *node) {
    if(queue->size == 0) {
        queue->head = node;
        queue->size++;
        return queue->size;
    }

    CharacterQueueNode *currentNode = queue->head;

    if(node->priority <= currentNode->priority) {
        node->next = currentNode;
        queue->head = node;
        queue->size++;
        
        return queue->size;
    }

    
    while(currentNode->next != NULL) {
        CharacterQueueNode *previousNode = currentNode;
        currentNode = currentNode->next;

        if(node->priority <= currentNode->priority) {
            node->next = currentNode;
            previousNode->next = node;
            queue->size++;
            
            return queue->size;
        }
    }

    currentNode->next = node;
    queue->size++;

    return queue->size;
}

QueueNode peekQueue(Queue *queue) {
    return *(queue->head);
}

CharacterQueueNode peekCharacterQueue(CharacterQueue *queue) {
    return *(queue->head);
}

int queueSize(Queue *queue) {
    return queue->size;
}

int queuePriority(Queue *queue) {
    return queue->head->priority;
}