#ifndef PRIORITY_H
#define PRIORITY_H

#include <stdio.h>
#include <stdlib.h>
#include "dungeon.h"

class QueueNode {
    public:
    QueueNode *next;
    int priority, x, y;
};

class CharacterQueueNode {
    public:
    CharacterQueueNode *next;
    Character *character;
    int priority;
};

class Queue {
    public:
    QueueNode *head;
    int size;
};

class CharacterQueue {
    public:
    CharacterQueueNode *head;
    int size;
};

Queue *initializeQueue();
CharacterQueue *initializeCharacterQueue();
void deallocateCharacterQueue(CharacterQueue *characterQueue);
QueueNode *initializeQueueNode(int x, int y, int priority);
CharacterQueueNode *initializeCharacterQueueNode(int priority, Character *character);
QueueNode peekQueue(Queue *queue);
CharacterQueueNode peekCharacterQueue(CharacterQueue *queue);
QueueNode popQueue(Queue *queue);
CharacterQueueNode popCharacterQueue(CharacterQueue *queue);
int addQueue(Queue *queue, QueueNode *node);
int addCharacterQueue(CharacterQueue *queue, CharacterQueueNode *node);
int queuePriority(Queue *queue);
int characterQueuePriority(CharacterQueue *queue);
int queueSize(Queue *queue);
int characterQueueSize(Queue *queue);

#endif