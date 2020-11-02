#include <stdlib.h>
#include <stdio.h>
#include <SDL.h>
#include "drawline.h"
#include "triangle.h"
#include "object.h"

/* Two macros that find the lesser or greater of two values */
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)


/*
 * Return a newly created object based on the arguments provided.
 */
object_t *create_object(SDL_Surface *surface, triangle_t *model, int numtriangles)
{
    object_t *newObjectptr = malloc(sizeof(object_t)); // Allocate space for our new object
    if (newObjectptr == NULL) { // Return NULL to indicate we could not allocate space
        return NULL;
    }
    triangle_t *newModelArrayptr = malloc(sizeof(triangle_t) * numtriangles); // Allocate space for our copy of the triangle array
    if (newModelArrayptr == NULL) { // Return NULL to indicate we could not allocate space 
        free(newObjectptr); // Also free the now useless newObjectptr
        return NULL;
    }

    // Copy the array to our newly allocated space
    memcpy(newModelArrayptr, model, (sizeof(triangle_t)) * numtriangles);
    

    // Initialize values passed to function
    newObjectptr->surface = surface;
    newObjectptr->model = newModelArrayptr;
    newObjectptr->numtriangles = numtriangles;
    // Initialize the remaining values
    newObjectptr->tx = 100;
    newObjectptr->ty = 100;
    newObjectptr->ttl = 5000; // Time To Live, in milliseconds
    newObjectptr->rotation = 0;
    newObjectptr->scale = 0.1;
    newObjectptr->speedx = rand() % 51 + 20; // Random value between 20 and 70
    newObjectptr->speedy = rand() % 3;

    // Calculate radius
    int xmax = 0; // We know that the original x coordinates (not sx) are specified such
    int xmin = 0; // that the object centre is in 0,0.
    int i;
    for (i = 0; i < numtriangles; i++) {
        xmax = MAX(xmax, model[i].x1);
        xmax = MAX(xmax, model[i].x2);
        xmax = MAX(xmax, model[i].x3);
        
        xmin = MIN(xmin, model[i].x1);
        xmin = MIN(xmin, model[i].x2);
        xmin = MIN(xmin, model[i].x3);
    }
    // we add a padding of 3 to ensure none of the object sticks out of the surface
    newObjectptr->radius = (xmax - xmin)/2 * newObjectptr->scale + 3;


    return newObjectptr;

}

/*
 * Destroy the object, freeing the memory.
 */
void destroy_object(object_t *object)
{
    free(object->model); // Free the model array first
    free(object);
}

/*
 * Draw the object on its surface.
 */
void draw_object(object_t *object)
{
    int i;
    // Iterate through our model array, and move/rotate/scale and draw each triangle
    for (i = 0; i < object->numtriangles; i++) {
        object->model[i].tx = (int)object->tx; // Cast from float to int before drawing
        object->model[i].ty = (int)object->ty;
        object->model[i].rotation = object->rotation;
        object->model[i].scale = object->scale;

        draw_triangle(object->surface, &object->model[i]);
        
    }
    
}
