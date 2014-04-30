/**
 * "We should forget about small efficiencies, say about 97% of the time: premature optimization
 * is the root of all evil. Yet we should not pass up our opportunities in that critical 3%."
 * -Donald Knuth
 */
#include <stdio.h>
#include <SDL2/SDL.h>
#include "GLEW/glew.h"
#include "drawUtils.h"
#include "collide.h"
#include "gstructures.h"
#include "mapGen.h"
#include <math.h>

#define SDL_MAIN_HANDLED

//structure to hold the coordinates of a four point polygon
//should be moved to a new file
struct coords
{
    float pt1x;
    float pt1y;
    float pt2x;
    float pt2y;
    float pt3x;
    float pt3y;
    float pt4x;
    float pt4y;
};

//structure for defining an entity, or player
//should be moved to a new file
struct entity
{
    char type;
    int num;
    float xPos;
    float yPos;
    GLuint* image;
    int xScale;
    int yScale;
    int stack;
    int collidable;
    int isColliding;
    //struct coords pts;
    struct polygon pol;
    
    //physics
    float zpos;
    int isJumping;
    float zVel;
    //float grav;
};




static const float tileXScale = 64;
static const float tileXScaleCalc = 64;
static const float tileYScale = 32;
static const float tileYScaleCalc = 32;
static const int screenScale = 250;

void updateCoords(float x, float y, struct entity* e)
{
    e->pol.p1.xComp += x;
    e->pol.p1.yComp += y;
    
    e->pol.p2.xComp += x;
    e->pol.p2.yComp += y;
    
    e->pol.p3.xComp += x;
    e->pol.p3.yComp += y;
    
    e->pol.p4.xComp += x;
    e->pol.p4.yComp += y;
    
    e->pol = assemblePolygon(e->pol);
    
}

//should be moved to a new file
void playerJump(struct entity* e)
{
    if(e->isJumping)
    {
        return;
    }
    e->isJumping = 1;
    e->zVel = 6;
}

//should be moved to a new file
void playerTick(struct entity* e)
{
    float newz = e->zpos + e->zVel;
    e->zpos = newz;
    e->zVel -= .3;
    
    if(e->zpos < e->stack * 10 && e->stack >1)
    {
        e->zVel = 0;
        e->zpos = e->stack * 10;
        e->isJumping = 0;
    }
    else if(e->zpos < 0)
    {
        e->zpos = 0;
        e->isJumping = 0;
    }
}


int main(void)
{
    //Initialize SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        return 1;
    }
    //Create the window, OpenGL context
    SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_Window* window = SDL_CreateWindow("TestSDL",
                                          SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                                          800, 600,
                                          SDL_WINDOW_OPENGL);
    if(!window)
    {
        SDL_Quit();
        return 1;
    }
    SDL_GL_CreateContext(window);
    
    //Make sure we have a recent version of OpenGL
    
    GLenum glewError = glewInit();
    if(glewError != GLEW_OK)
    {
        SDL_Quit();
        return 1;
    }
    if(!GLEW_VERSION_1_5)
    {
        SDL_Quit();
        return 1;
    }
    
    
    int tileW = 64;
    int tileH = 64;
    GLuint empty = glTexImageTGAFile("t3.tga", &tileW, &tileH );
    GLuint cube = glTexImageTGAFile("t2.tga", &tileW, &tileW);
    GLuint ch1 = glTexImageTGAFile("char1.tga", &tileW, &tileH );
    GLuint ch2 = glTexImageTGAFile("char2.tga", &tileW, &tileH );
    GLuint ch3 = glTexImageTGAFile("char3.tga", &tileW, &tileH );
    GLuint ch4 = glTexImageTGAFile("char4.tga", &tileW, &tileH );
    GLuint ch5 = glTexImageTGAFile("char5.tga", &tileW, &tileH );
    GLuint ch6 = glTexImageTGAFile("char6.tga", &tileW, &tileH );
    GLuint ch7 = glTexImageTGAFile("char7.tga", &tileW, &tileH );
    GLuint ch8 = glTexImageTGAFile("char8.tga", &tileW, &tileH );

    
    //initialize player
    struct entity player;
    player.xPos = 0;
    player.yPos = 0;
    //struct coords crd;
   
    struct polygon pg;
    //bounding box for player
    pg.p1.xComp = 33.0;
    pg.p1.yComp = 1.0;
    
    pg.p2.xComp = 28.0;
    pg.p2.yComp = 0.0;
    
    pg.p3.xComp = 37.0;
    pg.p3.yComp = 0.0;
    
    pg.p4.xComp = 30.0;
    pg.p4.yComp = -1.0;
  
    player.pol = assemblePolygon(pg);
    player.zpos = 0;
    player.zVel = 0;
    player.isJumping = 0;
    player.stack = 0;
    player.image = &ch1;
    


    
    /*int tileMap[12][12] = {{1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,1,1,1,1,1,1,1},
                           {1,1,1,1,1,1,1,1,1,1,1,1},
                           {1,1,1,1,1,1,1,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1},
                           {1,1,1,1,1,2,2,1,1,1,1,1}};
    
        
    
    int tileMapHeight[12][12] = {{2,2,2,2,3,1,1,4,3,2,2,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,2,2,2,1,1,1,2},
                                 {2,1,1,1,1,1,1,1,1,1,1,2},
                                 {2,1,1,1,2,2,2,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,1,1,1,2,1,1,2,1,1,1,2},
                                 {2,2,2,2,2,1,1,2,2,2,2,2}}; */
    
    
    struct chunk map = generateChunk();
    
    int **tileMap = map.chunkBase;
    int **tileMapHeight = map.chunkBlocks;
    
    
    int mapHeight = 100; //sizeof(tileMap)/sizeof(tileMap[0]);
    int mapWidth  = 100; // sizeof(tileMap[0])/sizeof(tileMap[0][0]);
    
    struct Tile tiles[(mapWidth * mapHeight)]; // may need to be 2d array
    int tileType;
    int tileHeight;
    int count  =0;
    for(int i = 0; i< mapHeight; i++)
    {
        for(int j = mapWidth-1; j>= 0; j--)
        {
            tileType = tileMap[i][j];
            tileHeight = tileMapHeight[i][j];
            struct Tile t;
            t.stack = tileHeight;
            t.num = tileType;
            
            float xp = (j * tileXScaleCalc / 2) + (i * tileXScaleCalc / 2);
            float yp = (i * tileYScaleCalc / 2) - (j * tileYScaleCalc / 2);
            
            t.xPos = xp;
            t.yPos = yp;
            
            struct polygon pg;
            
            
            pg.p1.xComp = xp + 32.0;
            pg.p1.yComp = yp - 16.0;
            
            pg.p2.xComp = xp;
            pg.p2.yComp = yp - 32.0;
            
            pg.p3.xComp = xp + 30.0;
            pg.p3.yComp = yp - 48.0;
            
            pg.p4.xComp = xp + 62.0;
            pg.p4.yComp = yp - 32.0;
            
            t.pol = assemblePolygon(pg);

            if(tileType == 0)
            {
                t.image = &empty;
                t.xScale = tileXScale;
                t.yScale = tileYScale;

            }
            else if(tileType == 1)
            {
                t.image = &cube;
                t.xScale = tileXScale;
                t.yScale = tileXScale;
            }

            tiles[count] = t;
            count++;
        }
    }
    
    
    

    
    unsigned char kbPrevState[SDL_NUM_SCANCODES] = {0};
    //The current frame's keyboard state
    const unsigned char* kbState = NULL;
    //Keep a pointer to SDL's internal keyboard state
    kbState = SDL_GetKeyboardState(NULL);

    
    glViewport( 0, 0, 800, 600 );
    glMatrixMode(GL_PROJECTION);
    glOrtho( 0, 800, 600, 0, 0, 1 );
    glEnable( GL_TEXTURE_2D );
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    

    
    //lighting
    //glEnable(GL_LIGHTING);
    
  
    int shouldExit = 0;  //change to int
    
    
    float xoffset = 0;
    float yoffset = 0;
    //int x;
    //int y;


    
    //start position. if player is colliding with a tile, move him till he doest
    updateCoords(350, -10, &player);
    int colindex = 0;
    while(checkCollision(player.pol, tiles,(mapWidth * mapHeight) , &colindex))
    {
        updateCoords(1.5, 0, &player);
        xoffset += 1.5;
    }
    
    
    
    int ticksPerFrame = 1000 / 60;
    int prevTick = SDL_GetTicks();
    int ticksPerPhysics = 1000 / 100;
    int prevPhysicsTick = prevTick;
    
    int mx, my; //testing, move to outside of loop
    
    int castEnable = 0;
    int shaddow;
    
    
    while(!shouldExit)
    {
        //kbState is updated by the message pump. Copy over the old state before the pump!
        memcpy(kbPrevState, kbState, sizeof(kbPrevState));
        //Handle OS message pump
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_QUIT:
                    return 0;
            }
        }
        
        
        int tick = SDL_GetTicks();
        do {
            SDL_Delay((int) fmax( 0, ticksPerFrame - (tick - prevTick) ));
            tick = SDL_GetTicks();
        } while( ticksPerFrame - (tick - prevTick) > 0 );
        prevTick = tick;

        
        const unsigned char* keys = SDL_GetKeyboardState(NULL);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        while( tick > prevPhysicsTick + ticksPerPhysics )
        {
            
            SDL_GetMouseState(&mx, &my);
            mx -= player.pol.p1.xComp - xoffset;
            my -= player.pol.p1.yComp - player.zpos + screenScale - yoffset + 50;
            double angle = -(atan2(my, mx) * 180 *  3.14159265)/10;
            if(angle < 0)
            {
                angle += 360;
            }
            if(angle < 25)
            {
                player.image = &ch2;
            }
            else if(angle < 55)
            {
                player.image = &ch8;
            }
            else if(angle < 120)
            {
                player.image = &ch1;
            }
            else if(angle < 170)
            {
                player.image = &ch7;
            }
            else if(angle < 195)
            {
                player.image = &ch3;
            }
            else if(angle < 230)
            {
                player.image = &ch6;
            }
            else if(angle < 300)
            {
                player.image = &ch4;
            }
            else
            {
                player.image = &ch5;
            }
            
            //printf("mouse x = %i  mouse y = %i angle = %f \n", mx, my, angle);
            // Update physics
            
            
            if(!checkCollision(player.pol, tiles,(mapWidth * mapHeight), &colindex) )
            {
                player.stack = 0;
                colindex = 0;
            }
            

            if(keys[SDL_SCANCODE_SPACE] )
            {
                playerJump(&player);
                //printf("jump");
                 //player.image = &ch3;
            }
            
            if(keys[SDL_SCANCODE_O] )
            {
                castEnable = 1;
            }
            if(keys[SDL_SCANCODE_P] )
            {
                castEnable = 0;
            }
            
            if(keys[SDL_SCANCODE_K] )
            {
                shaddow = 1;
            }
            if(keys[SDL_SCANCODE_L] )
            {
                shaddow = 0;
            }

            
            if(keys[SDL_SCANCODE_A] )
            {
                xoffset -= 1.5;
                updateCoords(-1.5, 0, &player);
                if(checkCollision(player.pol, tiles,(mapWidth * mapHeight), &colindex) )
                {
                    if(player.zpos > tiles[colindex].stack * 10)
                    {
                        player.stack = tiles[colindex].stack;
                    }
                    else if(player.stack < tiles[colindex].stack)
                    {
                        //player.stack = 0;
                        updateCoords(1.5, 0, &player);
                        xoffset += 1.5;
                    }

                }
            }
            
            if(keys[SDL_SCANCODE_D] )
            {
                xoffset += 1.5;
                updateCoords(1.5, 0, &player);
                if(checkCollision(player.pol, tiles, (mapWidth * mapHeight), &colindex) )
                {
                    if(player.zpos > tiles[colindex].stack * 10)
                    {
                        player.stack = tiles[colindex].stack;
                    }
                    else if(player.stack < tiles[colindex].stack)
                    {
                        //player.stack = 0;
                        updateCoords(-1.5, 0, &player);
                        xoffset -= 1.5;
                    }

                }
            }
            
            if(keys[SDL_SCANCODE_W] )
            {
                yoffset -= 1.5 ;
                updateCoords(0, -1.5, &player);
                if(checkCollision(player.pol, tiles, (mapWidth * mapHeight), &colindex) )
                {
                    if(player.zpos > tiles[colindex].stack * 10)
                    {
                        player.stack = tiles[colindex].stack;
                    }
                    else if(player.stack < tiles[colindex].stack)
                    {
                        //player.stack = 0;
                        yoffset += 1.5 ;
                        updateCoords(0, 1.5, &player);
                        //player.isColliding = 0;
                    }

                }
            }
            
            if(keys[SDL_SCANCODE_S] )
            {
               
                yoffset += 1.5;
                updateCoords(0, 1.5, &player);
                if(checkCollision(player.pol, tiles, (mapWidth * mapHeight), &colindex) )
                {
                    if(player.zpos > tiles[colindex].stack * 10)
                    {
                        player.stack = tiles[colindex].stack;
                    }
                    else if(player.stack < tiles[colindex].stack)
                    {
                        //player.stack = 0;
                        updateCoords(0, -1.5, &player);
                        yoffset -= 1.5;
                    }

                    
                }
            }
            playerTick(&player);
            prevPhysicsTick += ticksPerPhysics;
        }
        
        

        

        //draw base layer
        for(int z = 0; z < (mapWidth * mapHeight); z++)
        {
            if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270)
            {
                glDrawSprite(*tiles[z].image, tiles[z].xPos - xoffset, tiles[z].yPos+screenScale - yoffset, tiles[z].xScale, tiles[z].yScale );
            }
        }
        
        
        //cast rays; optimise when to recalculate
        //This is debug code
        glLineWidth(.1);
        glColor3f(1.0f, 0.0f, 0.0f);
        
        if(castEnable)
        {
            for(int z = 0; z < (mapWidth * mapHeight); z++)
            {
                if(tiles[z].stack > 1)
                {
                    if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270)
                    {
                        
                        glBegin(GL_LINES);
                        glVertex2f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos) ;
                        glVertex2f(tiles[z].pol.p1.xComp - xoffset, tiles[z].pol.p1.yComp +screenScale - yoffset + 64);
                        
                        glVertex2f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos) ;
                        glVertex2f(tiles[z].pol.p2.xComp - xoffset, tiles[z].pol.p2.yComp +screenScale - yoffset + 64);
                        
                        glVertex2f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos) ;
                        glVertex2f(tiles[z].pol.p3.xComp - xoffset, tiles[z].pol.p3.yComp +screenScale - yoffset + 64);
                        
                        glVertex2f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos) ;
                        glVertex2f(tiles[z].pol.p4.xComp - xoffset, tiles[z].pol.p4.yComp +screenScale - yoffset + 64);
                        glEnd();
                    }
                }
            }
        }
        
  
     



        //cast shaddows
        //This is debug code
        if(shaddow)
        {
            int z = 999;
            while(tiles[z].stack <= 2)
            {
                z--;
            }
            glShadeModel(GL_SMOOTH);
            glBegin(GL_TRIANGLES);
            glColor4f(0.1, 0.2, 0.3, .0);
            glVertex3f(tiles[z].pol.p1.xComp - xoffset, tiles[z].pol.p1.yComp +screenScale - yoffset + 64, 0);
            glColor4f(0.4, 0.5, 0.6, .0);
            glVertex3f(tiles[z].pol.p2.xComp - xoffset, tiles[z].pol.p2.yComp +screenScale - yoffset + 64, 0);
            glColor4f(0.7, 0.8, 0.9, 1);
            glVertex3f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos, 0);
            glEnd();
            
       
            for(z = 0; z < (mapWidth * mapHeight); z++)
            {
                if(tiles[z].stack > 1)
                {
                    if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270)
                    {
                        glShadeModel(GL_SMOOTH);
     
                        //glBegin(GL_TRIANGLES);
                        glBegin( GL_TRIANGLE_STRIP );
                        
                        
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p1.xComp - xoffset, tiles[z].pol.p1.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p2.xComp - xoffset, tiles[z].pol.p2.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.7, 0.8, 0.9, .6);
                        glVertex3f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos, 0);
                        
                        
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p2.xComp - xoffset, tiles[z].pol.p2.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p3.xComp - xoffset, tiles[z].pol.p3.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.7, 0.8, 0.9, .6);
                        glVertex3f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos, 0);
                       
                        
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p3.xComp - xoffset, tiles[z].pol.p3.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p4.xComp - xoffset, tiles[z].pol.p4.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.7, 0.8, 0.9, .6);
                        glVertex3f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos, 0);
                        
                        
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p1.xComp - xoffset, tiles[z].pol.p1.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.4, 0.5, 0.6, .0);
                        glVertex3f(tiles[z].pol.p2.xComp - xoffset, tiles[z].pol.p2.yComp +screenScale - yoffset + 64, 0);
                        glColor4f(0.7, 0.8, 0.9, .6);
                        glVertex3f(player.pol.p1.xComp - xoffset, player.pol.p1.yComp - yoffset + screenScale + 64 - player.zpos, 0);

                        

                        glEnd();
                    

                    }
                }
            }


        }

    
        //############################################################################## here be dragons.
        //draw things behind  and under player.
        for(int z = 0; z < (mapWidth * mapHeight); z++)
        {
             if(tiles[z].stack > 1)
             {
                 if(player.pol.p1.yComp <= tiles[z].yPos - 16 - 14)
                 {
                   //do nothing
                 }
                 else
                 {
                     for(int ix = 1; ix <  tiles[z].stack; ix++)
                     {
                         if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270 )
                         {
                            glDrawSprite(*tiles[z].image, tiles[z].xPos - xoffset , tiles[z].yPos+screenScale -(ix * 16) - yoffset, tiles[z].xScale, tiles[z].yScale );
                         }
                             
                         }

                     }
             
            
            if(player.stack >= tiles[z].stack -2)
            {
                for(int ix = 1; ix <  tiles[z].stack; ix++)
                {
                    if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270 )
                    {

                        glDrawSprite(*tiles[z].image, tiles[z].xPos - xoffset , tiles[z].yPos+screenScale -(ix * 16) - yoffset, tiles[z].xScale, tiles[z].yScale );
                    }
                }
             }
          }
            
        }
        
        
        //draw player
        glDrawSprite(*player.image, player.pol.p1.xComp -30 - xoffset,  player.pol.p1.yComp - player.zpos + screenScale - yoffset, 64, 64 );

        
        //draw things in front of player
        for(int z = 0; z < (mapWidth * mapHeight); z++)
        {
            if(tiles[z].stack > 1)
            {
        
                if(player.pol.p1.yComp <= tiles[z].yPos - 16 - 14  )
                {
                    
               
                    for(int ix = 1; ix <  tiles[z].stack; ix++)
                    {
  
                        if( tiles[z].xPos > xoffset &&  tiles[z].xPos < xoffset +750 && tiles[z].yPos > yoffset -250 && tiles[z].yPos < yoffset +270 )
                        {
    
                            if(tiles[z].stack > player.stack && ix >= player.stack)
                            {
                                glDrawSprite(*tiles[z].image, tiles[z].xPos - xoffset , tiles[z].yPos+screenScale -(ix * 16) - yoffset, tiles[z].xScale, tiles[z].yScale );
                            }
                            
                        }
                    }
                    if(z == colindex && tiles[z].stack == player.stack)
                    {
                        glDrawSprite(*player.image, player.pol.p1.xComp -30 - xoffset,  player.pol.p1.yComp - player.zpos + screenScale - yoffset, 64, 64 );
                    }
              }
            }
    
        }

        //##############################################################################



        
        /*
        glLineWidth(2.5);
        glColor3f(1.0f, 0.0f, 0.0f);
     
        glBegin(GL_LINES);
        glVertex2f(10, 10);
        glVertex2f(20, 20);
        glEnd();
         */

        SDL_GL_SwapWindow(window);
        //glFlush();
    }
    SDL_Quit();
    return 0;
    
    
}
