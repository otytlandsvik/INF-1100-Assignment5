#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <SDL.h>
#include <time.h>
#include "drawline.h"
#include "triangle.h"
#include "list.h"
#include "teapot_data.h"
#include "sphere_data.h"
#include "object.h"

/* Two macros that find the lesser or greater of two values */
#define MIN(x,y) (x < y ? x : y)
#define MAX(x,y) (x > y ? x : y)

/*
 * Clear the surface by filling it with 0x00000000(black).
 */
void clear_screen(SDL_Surface *surface)
{
    if(SDL_FillRect(surface, NULL, 0x00000000) < 0){
        fprintf(stderr, "Unable to clear the surface. Error returned: %s\n", SDL_GetError());
        SDL_Quit();
        exit(EXIT_FAILURE);
    }
}

/*
 * Accelerate the object; altering its speed based on the boost given.
 */
void accelerate_object(object_t *a, float boost, float maxspeed)
{
    float s;
    float news;
    
    /* Calculate lenght of speed vector */
    s = sqrtf(a->speedx * a->speedx + a->speedy * a->speedy);

    /* Boost speed */
    news = s * boost;
    if (news < 0.0)
        news = 0.0;
    if (news > maxspeed)
        news = maxspeed;    
    a->speedx *= news/s;
    a->speedy *= news/s;
}

/*
 * Update the ball object: check and handle collision, calculate speed and change the objects'
 * properties
 */

void update_ball(SDL_Surface *surface, object_t *ball, float gravity, float loopTime) {

        // Accellerate the ball with the gravity constant
        ball->speedy += gravity;

        // Where the ball will be next frame with current speed
        float newx = ball->tx + ball->speedx;
        float newy = ball->ty + ball->speedy;

        //Check whether ball will hit a wall, or the floor
        float friction = 0.7;
        if ((newx - ball->radius) <= 0) { // Ball has collided with left wall
            ball->tx = ball->radius; // Move ball away from the edge
            ball->speedx *= -1; // Change sign of speedx
            accelerate_object(ball, friction, ball->radius);
        }
        if ((newx + ball->radius) >= surface->w) { // Ball has collided with right wall
            ball->tx = surface->w - ball->radius; // Move ball away from the edge
            ball->speedx *= -1; // Change sign of speedx
            accelerate_object(ball, friction, ball->radius);
        }
        if ((newy + ball->radius) >= surface->h) { // Ball has collided with floor
            ball->ty = surface->h - ball->radius; // Move ball away from the edge
            ball->speedy *= -1; // Change sign of speedy
            accelerate_object(ball, friction, ball->radius);

            // Check if the ball has a very low speedy. If so, it should stop bouncing
            if (fabs(ball->speedy) <= 1) { // Five is a relatively low speed
                ball->speedy = 0;
            }
        }

        // Check if the ball is lying still on the floor. If so, decrement the Time To Live
        // with the amount of time that has passed this loop
        if (ball->speedy == 0) {
            if (ball->ttl < loopTime) {
                ball->ttl = 0;
            } else {
                ball->ttl -= (int)(loopTime);
            }
        }
        

        // Calculate new coordinates based on speed
        ball->tx += ball->speedx;
        ball->ty += ball->speedy;
        
        // Rotate the ball
        ball->rotation += ball->speedx;
}

/*
 * Animate bouncing balls on the screen.
 */
void bouncing_balls(SDL_Window *window)
{
    // Get the surface
    SDL_Surface *surface = SDL_GetWindowSurface(window);
    if (surface == NULL) { // Report error and quit if we could not get surface
        printf("Unable to get window surface\n");
        return;
    }
    
    // Create a new list
    list_t *ballList = list_create();
    if (ballList == NULL) { // Report error and quit if we could not create a list
        printf("Unable to allocate list\n");
        return;
    }

    // A variable to hold our current ball
    object_t *ball;
 

    // Create a couple of ball (and teapot) objects and add them into the list
    int i;
    srand(time(NULL)); // Set the seed of rand
    for (i = 0; i < rand() % 10 + 10; i++) { // Between 2 and 8 balls
        // Create object
        if ((SDL_GetPerformanceCounter() % 2) == 0) { // Create ball if even, teapot if odd. 50/50
            ball = create_object(surface, sphere_model, SPHERE_NUMTRIANGLES);
        } else {
            ball = create_object(surface, teapot_model, TEAPOT_NUMTRIANGLES);
        }
        // Report error and quit if an object was not succesfully created
        if (ball == NULL) { 
            printf("Unable to allocate object\n");
            return;
        }
        // Add object to list
        list_addlast(ballList, ball); // Add object to our list
    }
    
    // Create a new iterator to traverse our list
    list_iterator_t *ballListIter = list_createiterator(ballList);
    if (ballListIter == NULL) { // Report error and quit if we could not create iterator
        printf("Unable to allocate list iterator\n");
        return;
    }

    // Gravitational accelleration
    float gravity = 0.25; 


    // Variables we need for event handling
    int done = 0;
    SDL_Event event;

    
    // The amount of time to wait before drawing again, in milliseconds
    float drawTime;
    // The time at the start of our loop, for calculating drawTime
    float startTime;

    while (!done) {
        // The time it took for the previous loop, including SDL_Delay()
        float loopTime = SDL_GetTicks() - startTime;
        // Get the time at the start of the loop
        startTime = SDL_GetTicks();

        // Update and clear surface
        SDL_UpdateWindowSurface(window);
        clear_screen(surface);

        // We need a new variable to hold our new ball if we make one, so that we may
        // check for NULL
        object_t *newball; 


        // Update and draw our ball list
        for (i = 0; i < list_size(ballList); i++) {
            ball = list_next(ballListIter);
            if (ball == NULL) { // Report error and quit if item we extract is NULL
                printf("Iterator out of bounds\n");
                return;
            }

            if (ball->ttl <= 0) { // If ball has been still for 5 seconds, remove it
                list_remove(ballList, ball);
                destroy_object(ball);
                // Create new object
                if ((SDL_GetTicks() % 2) == 0) { // Create ball if even, teapot if odd. 50/50
                    newball = create_object(surface, sphere_model, SPHERE_NUMTRIANGLES);
                } else {
                    newball = create_object(surface, teapot_model, TEAPOT_NUMTRIANGLES);

                }
                // Report error and quit if an object was not succesfully created
                if (newball == NULL) {
                    printf("Unable to allocate object\n");
                    return;
                }
                // Add new ball to list
                list_addlast(ballList, newball);
            } else { // If not, update and draw
                update_ball(surface, ball, gravity, loopTime);
                draw_object(ball);
            }
        }

        // Reset our iterator for later use
        list_resetiterator(ballListIter);
        
        // Calculate how long it took to draw everything
        drawTime = (SDL_GetTicks() - startTime);


        // Wait before next animation loop
        float waitTime = 16.66666 - drawTime; // Ensure we are waiting ~16 and 2/3 ms before next loop (approx 60fps)
        if (waitTime < 0.0) {
            waitTime = 0.0; // We cannot wait a negative amount of time
        }
        SDL_Delay(waitTime);

        // Event handling, stop the loop if we want to quit the program
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    // Destroy our objects, list and iterator, freeing the allocated memory
                    for (i = 0; i < list_size(ballList); i++) {
                        ball = list_next(ballListIter);
                        destroy_object(ball);
                    }
                    list_destroy(ballList);
                    list_destroyiterator(ballListIter);
                    done = 1;       
                    break;
                case SDL_WINDOWEVENT:
                    if (event.window.event == SDL_WINDOWEVENT_SHOWN)
                        SDL_SetWindowPosition(window, 50, 50);
                    break;
            }          
        }
    }

    
}



int main(void)
{
    const size_t bufsize = 100;
    
    /* Change the screen width and height to your own liking */
    const int screen_w = 1500;
    const int screen_h = 800;

    char errmsg[bufsize];
    SDL_Window *window;

    /* Initialize SDL */
    if (SDL_Init(SDL_INIT_VIDEO) < -1) {
        snprintf(errmsg, bufsize, "Unable to initialize SDL.");
        goto error;
    }
    
    /* Create a 1600x900 window */
    window = SDL_CreateWindow("The Amazing Bouncing Balls and Teapots",
                              SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED,
                              screen_w, screen_h,
                              0);
    if(!window) {
        snprintf(errmsg, bufsize, "Unable to get video surface.");
        goto error;
    }

    /* Start bouncing some balls */
    bouncing_balls(window);
    

    /* Shut down SDL */
    SDL_Quit();

    /* Wait a little bit jic something went wrong (so that printfs can be read) */
    SDL_Delay(2000);
    
    return 0;

    /* Upon an error, print message and quit properly */
error:
    fprintf(stderr, "%s Error returned: %s\n", errmsg, SDL_GetError());
    SDL_Quit();
    exit(EXIT_FAILURE);
}
