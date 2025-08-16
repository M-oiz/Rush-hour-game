//============================================================================
// Name        : .cpp
// Author      : Dr. Sibt Ul Hussain
// Version     :
// Copyright   : (c) Reserved
// Description : Basic 2D game...
//============================================================================

#ifndef RushHour_CPP_
#define RushHour_CPP_

#include "util.h"
#include <iostream>
#include <string>
#include<fstream>
#include <cmath> // for basic math functions such as cos, sin, sqrt
using namespace std;



int numObstacles = 0;
int numOtherCars = 0;
int xI = 4;  // Start position X
int yI = 800;// Start position Y
int gameBoard[25][21] = {0};





float buildingColors[25][21][3]; // To store building colors
float ROAD_GRAY[] = {0.3, 0.3, 0.3};
float BRIGHT_YELLOW[] = {1.0, 1.0, 0.5};


bool IsValidPosition(int x, int y, int width, int height);
bool CheckCollision(int carX, int carY, int carWidth, int carHeight);
class GameState {
private:
int numObstacles ;
int numOtherCars ;
    int score;
    int fuel;
    int timeLeft;
    int wallet;
    int completedPassengers;
    int completedDeliveries;
    int currentPassenger; // -1 means none
    int currentDelivery;  // -1 means none
    bool gameOver;
    bool isTaxiMode;

public:
    GameState() : 

        score(0),
        fuel(100),
        timeLeft(180),
        wallet(0),
        completedPassengers(0),
        completedDeliveries(0),
        currentPassenger(-1),
        currentDelivery(-1),
        gameOver(false),
        isTaxiMode(true) {}

    // Getters and setters

    
   int getnumOtherCars()const{return numOtherCars;}
    void setnumOtherCars(int d){ numOtherCars =d;}
    void icrementOtherCars(){numOtherCars++;}

    int getnumObstacles()const{ return numObstacles;}
    void setnumObstacles(int d){ numObstacles = d;}
    void incrementnumObstacles(){ numObstacles++;}

    int getScore() const { return score; }
    void setScore(int s) { score = s; }
    void addScore(int delta) { score += delta; }
    
    int getFuel() const { return fuel; }
    void setFuel(int f) { fuel = f; }   
    void addFuel(int delta) { fuel += delta; }
    void decrementFuel(){ fuel--;}
    
    int getTimeLeft() const { return timeLeft; }
    void setTimeLeft(int t) { timeLeft = t; }
    void decrementTime() { if(timeLeft > 0) timeLeft--; }
    
    int getWallet() const { return wallet; }
    void setWallet(int w) { wallet = w; }
    void addWallet(int delta) { wallet += delta; }
    void decrementWallet(int d){wallet -= d;}
    
    int getCompletedPassengers() const { return completedPassengers; }
    void incrementCompletedPassengers() { completedPassengers++; }
    void setCompletedPassengers(int d){ completedPassengers = d;}


    int getCompletedDeliveries() const { return completedDeliveries; }
    void incrementCompletedDeliveries() { completedDeliveries++; }
     void setCompletedDeliveries(int d){ completedDeliveries = d;}

    
    int getCurrentPassenger() const { return currentPassenger; }
    void setCurrentPassenger(int p) { currentPassenger = p; }
    
    int getCurrentDelivery() const { return currentDelivery; }
    void setCurrentDelivery(int d) { currentDelivery = d; }
    
    bool isGameOver(){ return gameOver; }
    void setGameOver(bool go) { gameOver = go; }
    
    bool getIsTaxiMode() const { return isTaxiMode; }
    void setIsTaxiMode(bool mode) { isTaxiMode = mode; }
    
    // Reset all state
    void reset() {
 xI = 3 ;  // Start position X
 yI = 800;                         // Start position Y
        score = 0;
        fuel = 100;
        timeLeft = 180;
        wallet = 0;
        completedPassengers = 0;
        completedDeliveries = 0;
        currentPassenger = -1;
        currentDelivery = -1;
        gameOver = false;
    }
};
GameState gamestate;
class GameObject {
protected:
    int x, y;
    int width, height;
    
public:
    GameObject(int x, int y, int width, int height) 
        : x(x), y(y), width(width), height(height) {}
    
    virtual void draw() = 0; // Pure virtual function
    
    int getX() const { return x; }
    int getY() const { return y; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }
    
    void setPosition(int newX, int newY) {
        x = newX;
        y = newY;
    }
    
    bool collidesWith(const GameObject& other) const {
        return !(x > other.x + other.width || 
                 x + width < other.x || 
                 y > other.y + other.height || 
                 y + height < other.y);
    }
};

// Vehicle class for all movable entities
class Vehicle : public GameObject {
protected:
    int speed;
    float color[3];
    
public:
    Vehicle(int x, int y, int w, int h, float r, float g, float b) 
        : GameObject(x, y, w, h), speed(3) {
        color[0] = r; color[1] = g; color[2] = b;
    }
    
    virtual void move(int dx, int dy) {
        x += dx * speed;
        y += dy * speed;
    }
    
    void setSpeed(int newSpeed) {
        speed = newSpeed;
    }
    
    int getSpeed() const {
        return speed;
    }
};

// Player's car
class PlayerCar : public Vehicle {
public:
   PlayerCar(int x, int y, bool isTaxiMode) 
        : Vehicle(x, y, 40, 20,isTaxiMode ? 1.0f : 0.53f,   isTaxiMode ? 1.0f : 0.90f,   isTaxiMode ? 0.0f : 0.53f) 
    {}
void draw() override {
        // Car body
        DrawRoundRect(x, y, width, height, color, 5);
        // Roof - color changes based on mode
        float roofColor[3] = {
            color[0] * 0.8f, 
            color[1] * 0.8f, 
            color[2] + 0.2f
        };
        DrawRoundRect(x+5, y+10, width-10, height/2, roofColor, 3);
        // Wheels
        DrawCircle(x+8, y+5, 6, colors[BLACK]);
        DrawCircle(x+width-8, y+5, 6, colors[BLACK]);
        
        if(gamestate.getCurrentPassenger() != -1) {
            DrawString(x-15, y-20, "PASSENGER", colors[WHITE]);
        }
    }
};


// Other cars in the game
class OtherCar : public Vehicle {
public:
    bool active;
    int direction; // 0: up, 1: right, 2: down, 3: left
    
    OtherCar() 
        : Vehicle(0, 0, 40, 20, 1, 0, 0), active(false), direction(0) {
        speed = 2; // Fixed speed
    }
    
    void init(int x, int y) {
        this->x = x;
        this->y = y;
        direction = rand() % 4;
        active = true;
    }
    
    void increaseSpeed() {
        speed += 2;
    }
    
    void move() {
    if(!active) return;
    
    // Save old position in case we need to revert
    int oldX = x;
    int oldY = y;
    
    // Move based on current direction
    switch(direction) {
        case 0: y += speed; break; // Up 
        case 1: x += speed; break; // Right
        case 2: y -= speed; break; // Down
        case 3: x -= speed; break; // Left
    }
    
    // Check if new position is valid
    bool hitSomething = !IsValidPosition(x, y, width, height);
    
    // If we hit something or went out of bounds
    if(hitSomething) {
        // Revert position
        x = oldX;
        y = oldY;
        
        // Choose a new random direction (but not the opposite)
        int newDir;
        do {
            newDir = rand() % 4;
        } while(newDir == (direction + 2) % 4); // Don't go directly backwards
        
        direction = newDir;
        
        // Try moving again in new direction
        switch(direction) {
            case 0: y += speed; break;
            case 1: x += speed; break;
            case 2: y -= speed; break;
            case 3: x -= speed; break;
        }
        
        // If still invalid, just stay put
        if(!IsValidPosition(x, y, width, height)) {
            x = oldX;
            y = oldY;
        }
    }
}

void draw() override {
    if(!active) return;
    
    // Car body
    DrawRoundRect(x, y, width, height, color, 5);
    // Roof
    DrawRoundRect(x+5, y+10, width-10, height/2, colors[RED], 3);
    // Wheels
    DrawCircle(x+8, y+5, 6, colors[BLACK]);
    DrawCircle(x+width-8, y+5, 6, colors[BLACK]);
    
    // Draw direction indicator
    switch(direction) {
        case 0: // Up
            DrawTriangle(x+width/2, y+height, x+width/2-5, y+height-10, 
                        x+width/2+5, y+height-10, colors[WHITE]);
            break;
        case 1: // Right
            DrawTriangle(x+width, y+height/2, x+width-10, y+height/2-5, 
                        x+width-10, y+height/2+5, colors[WHITE]);
            break;
        case 2: // Down
            DrawTriangle(x+width/2, y, x+width/2-5, y+10, 
                        x+width/2+5, y+10, colors[WHITE]);
            break;
        case 3: // Left
            DrawTriangle(x, y+height/2, x+10, y+height/2-5, 
                        x+10, y+height/2+5, colors[WHITE]);
            break;
    }
}
    };


// Building class
class Building : public GameObject {
private:
    float color[3];
    
public:
    Building(int x, int y) 
        : GameObject(x, y, 40, 40) {
        color[0] = 0.2 + (rand()%80)/100.0;
        color[1] = 0.2 + (rand()%80)/100.0;
        color[2] = 0.2 + (rand()%80)/100.0;
    }
    
    void draw() override {
              int gridX = x/40;
        int gridY = y/40;
        
        // Building base with shadow
        DrawRectangle(x+2, y+2, 40-4, 40-4, colors[BLACK]); // Shadow
        DrawRectangle(x, y, 40-4, 40-4, buildingColors[gridX][gridY]);
        
        // Detailed windows
        for(int wy = y+5; wy < y+40-10; wy += 12) {
            for(int wx = x+5; wx < x+40-5; wx += 8) {
                // Alternate window styles
                if((wx+wy)%20 < 10) {
                    DrawRectangle(wx, wy, 6, 8, colors[LIGHT_YELLOW]);
                } else {
                    DrawRectangle(wx, wy, 6, 8, colors[BLUE]);
                }
            }
        }
        
        // Rooftop details
        DrawLine(x, y, x+40-4, y, 2, colors[DARK_RED]); // Rooftop
    }
};


// Obstacles in the game
class Obstacle : public GameObject {
public:
    float color[3];
    bool active;
    
    Obstacle() 
        : GameObject(0, 0, 20, 20), active(false) {
        color[0] = colors[BROWN][0];
        color[1] = colors[BROWN][1];
        color[2] = colors[BROWN][2];
    }
    
    void init(int x, int y, int w, int h, float r, float g, float b) {
        this->x = x;
        this->y = y;
        width = w;
        height = h;
        color[0] = r; color[1] = g; color[2] = b;
        active = true;
    }
    
   void draw() override {
        if(!active) return;
        
        // Tree trunk
        DrawRectangle(x+7, y, 6, 20, colors[BROWN]);
        // Tree leaves
        DrawCircle(x+10, y+25, 15, colors[GREEN]);
    }
    
    bool checkCollision(int carX, int carY, int carW, int carH) {
        if(!active) return false;
        return !(carX > x + width || carX + carW < x || 
                 carY > y + height || carY + carH < y);
    }
};
// Passenger class
class Passenger {
public:
    int x, y;           // Current position
    int destX, destY;    // Destination coordinates
    bool waiting;        // Waiting to be picked up
    bool inTaxi;         // Currently in the taxi
     bool isPedestrian; // True in delivery mode, false in taxi mode
    
    Passenger() 
        : x(0), y(0), destX(0), destY(0), waiting(true), inTaxi(false) {}
   void init() {
    // Random positions on roads (not in buildings or station area)
    do {
        x = (rand() % (25-2) + 1) * 40;
        y = (rand() % (21-2) + 1) * 40;
    } while(!IsValidPosition(x, y, 1, 1) || 
           (x/40 <= 2 && y/40 <= 1)); // Avoid station area
    
    // Random destinations
    do {
        destX = (rand() % (25-2) + 1) * 40;
        destY = (rand() % (21-2) + 1) * 40;
    } while(!IsValidPosition(destX, destY, 1, 1) ||
           (destX/40 <= 2 && destY/40 <= 1)); // Avoid station area
    
    waiting = true;
    inTaxi = false;
}
    
  void draw() {
        if(waiting) {
            // Draw waiting passenger (person icon)
            DrawCircle(x, y, 7, colors[BLACK]); // Head
            DrawLine(x, y-10, x, y-25, 2, colors[BLACK]); // Body
            // Arms 
            DrawLine(x, y-12, x-7, y-12, 2, colors[BLACK]); // Left arm
            DrawLine(x, y-12, x+7, y-12, 2, colors[BLACK]); // Right arm
            DrawLine(x, y-25, x-5, y-35, 2, colors[BLACK]); // Left leg
            DrawLine(x, y-25, x+5, y-35, 2, colors[BLACK]); // Right leg
        }
        
        if(inTaxi) {
            // Draw destination marker
            // Flag pole
            DrawLine(destX, destY, destX, destY+30, 3, colors[BLACK]);
            // Flag
            DrawRectangle(destX, destY+15, 20, 15, colors[GREEN]);
            // House icon
            DrawRectangle(destX-15, destY-10, 30, 20, colors[LIGHT_BLUE]);
            DrawTriangle(destX-15, destY+10, destX+15, destY+10,
                       destX, destY+25, colors[RED]);
            DrawRectangle(destX-5, destY-20, 10, 10, colors[BROWN]);
            // Text
            DrawString(destX-12, destY-35, "DEST", colors[WHITE]);
        }
    }
};
class DeliveryPackage {
public:
    int x, y;           // Current position (pickup point)
    int destX, destY;    // Destination coordinates
    bool waiting;        // Waiting to be picked up
    bool inCar;          // Currently in the delivery car
    
    DeliveryPackage() 
        : x(0), y(0), destX(0), destY(0), waiting(true), inCar(false) {}
    
void init() {
    // Random positions on roads (not in buildings or station area)
    do {
        x = (rand() % (25-2) + 1) * 40;
        y = (rand() % (21-2) + 1) * 40;
    } while(!IsValidPosition(x, y, 1, 1) || 
           (x/40 <= 2 && y/40 <= 1)); // Avoid station area
    
    // Random destinations
    do {
        destX = (rand() % (25-2) + 1) * 40;
        destY = (rand() % (21-2) + 1) * 40;
    } while(!IsValidPosition(destX, destY, 1, 1) ||
           (destX/40 <= 2 && destY/40 <= 1)); // Avoid station area
    
    waiting = true;
    inCar = false;
}

void draw() {
        if(waiting) {
            // Draw package (brown box with tape)
            DrawRectangle(x-15, y-10, 30, 20, colors[BROWN]);
            // Tape stripes
            DrawRectangle(x-15, y, 30, 5, colors[KHAKI]);
            DrawRectangle(x-5, y-10, 5, 20, colors[KHAKI]);
        }
        
        if(inCar) {
            // Draw destination marker
            DrawRectangle(destX-15, destY-10, 30, 20, colors[LIGHT_BLUE]);
            DrawTriangle(destX-15, destY+10, destX+15, destY+10, 
                       destX, destY+25, colors[RED]);
            DrawString(destX-20, destY-25, "DELIVER", colors[WHITE]);
        }
    }
};


// Game manager class
class GameManager {
private:
    // Game entities
    Passenger passengers[4];   // max 4 passenger aige board par
    Obstacle obstacles[10];   // max obstacles 10 rakhe
    OtherCar otherCars[20];
    DeliveryPackage deliveries[3];  // sade max deliveries 3 ha is lia
    bool isTaxiMode; // true for taxi, false for delivery

    
public:


    GameManager():isTaxiMode(true) {
        // Initialize everything here
        srand(time(0));
        InitializeBoard();
        InitializePassengers();
        InitializeObstacles();
        InitializeOtherCars();
        
        // Reset player position
 xI = 3 ;  // Start position X
yI = 800;                         // Start position Y
    }
void ResetGame() {
    gamestate.reset();
    // Reset other game variables
    gamestate.setCurrentPassenger(-1);
    gamestate.setCurrentDelivery(-1);
    gamestate.setCompletedPassengers(0);
    gamestate.setCompletedDeliveries(0);
    gamestate.setWallet(0);
    gamestate.setGameOver(false);
    
    // Reinitialize game elements
    InitializePassengers();
    InitializeObstacles();
    InitializeOtherCars();
    if(!getIsTaxiMode()) {
        InitializeDeliveries();
    }
}
    void InitializeBoard() {
        
 for(int i=0; i<25; i++) {
        for(int j=0; j<21; j++) {
            // Mark leftmost columns as main roads and right most coltoo
            if(i == 0  || i == 25-1 ) {
                gameBoard[i][j] = 0; // Main road
            }
            // Add some horizontal main roads (fewer than vertical)
            else if(j == 5) {
                // Make these roads 2 cells wide
                if(i % 4 < 2) { // Alternate pattern
                    gameBoard[i][j] = 0; // Main road
                }
            }
                                      
         // sab se middle road ko bhi main road bana rha
            else if(i == 12 ) {
                gameBoard[i][j] = 0; // Main road
            }
            
            // Buildings - now we have more space for them
            else if(rand()%100 < 35 && j > 2) { // j>2 to avoid station area
                // Ensure buildings don't completely block paths
                bool canPlace = true;
                
                // Check if this would block all paths
                if(i > 2 && i < 25-3 && j > 2 && j < 21-3) {
                    int roadNeighbors = 0;
                    if(gameBoard[i-1][j] == 0) roadNeighbors++;
                    if(gameBoard[i+1][j] == 0) roadNeighbors++;
                    if(gameBoard[i][j-1] == 0) roadNeighbors++;
                    if(gameBoard[i][j+1] == 0) roadNeighbors++;
                    
                    if(roadNeighbors < 2) canPlace = false;
                }
                
                if(canPlace) {
                    gameBoard[i][j] = 1; // Building
                    GenerateBuildingColor(i, j);
                }
            }
        }
    }
  for(int i=0; i<3; i++) {
        for(int j=0; j<2; j++) {
            gameBoard[i][j] = 4; // Station
        }
    }
    
    // Add some horizontal connector roads (not main roads)
    for(int j=5; j<21; j+=5) {
        for(int i=2; i<25-2; i++) {
            // Skip middle of the road to create some variety
            if(abs(i - 25/2) > 3 && rand()%100 < 70) {
                gameBoard[i][j] = 0; // Regular road
            }
        }
    }
    
    // Place fuel stations (2-3 in total)
    int numFuelStations = 2 + rand() % 2;
    for(int fs=0; fs<numFuelStations; fs++) {
        int attempts = 0;
        bool placed = false;
        
        while(!placed && attempts < 100) {
            int i = rand() % (25-4) + 2;
            int j = rand() % (21-4) + 2;
            
            // Only place on roads and not too close to other stations
            if(gameBoard[i][j] == 0) {
                bool clearSpace = true;
                
                // Check surrounding cells
                for(int di=-2; di<=2; di++) {
                    for(int dj=-2; dj<=2; dj++) {
                        int checkX = i + di;
                        int checkY = j + dj;
                        if(checkX >=0 && checkX < 25 && 
                           checkY >=0 && checkY < 21 && 
                           gameBoard[checkX][checkY] == 3) {
                            clearSpace = false;
                            break;
                        }
                    }
                    if(!clearSpace) break;
                }
                
                if(clearSpace) {
                    gameBoard[i][j] = 3;
                    placed = true;
                }
            }
            attempts++;
        }
    }
    // ye neeche wala mene itni mehnat se banaya tha laki 35 % ne program war dia kia kare sharikin di nazar lag jate

/*
    for(int i=0; i<25; i++) {
        for(int j=0; j<21; j++) {
            // Mark 2 cells as road for each road segment
            if(i % (3*2) < 3) {
                gameBoard[i][j] = 0; // Road
            }
            // Buildings - ensure they don't block all paths
            else if(rand()%100 < 35 && j > 2 && j < 21-3) {
                // Ensure at least one path through buildings
                if(!(i > 5 && i < 25-5 && j > 5 && j < 21-5 && 
                   gameBoard[i-1][j] == 1 && gameBoard[i+1][j] == 1 &&
                   gameBoard[i][j-1] == 1 && gameBoard[i][j+1] == 1)) {
                    gameBoard[i][j] = 1; // Building
                    GenerateBuildingColor(i, j);
                }
            }
        }
    }
    
    // Ensure main roads are clear
    for(int j=0; j<21; j++) {
        gameBoard[0][j] = 0;
        gameBoard[1][j] = 0;
        gameBoard[25-2][j] = 0;
        gameBoard[25-1][j] = 0;
    }
    for(int i=0; i<25; i++) {
        gameBoard[i][0] = 0;
        gameBoard[i][1] = 0;
        gameBoard[i][21-2] = 0;
        gameBoard[i][21-1] = 0;
    }
    
    // Random fuel stations (2-3 in total)
// Random fuel stations (2-3 in total)
int numFuelStations = 2 + rand() % 2; // Random number between 2-3

for(int fs = 0; fs < numFuelStations; fs++) {
    int attempts = 0;
    bool placed = false;
    
    while(!placed && attempts < 100) {
        int i = rand() % (25-2) + 1; // Avoid edges
        int j = rand() % (21-2) + 1;
        
        // Check if this is a road cell
        if(gameBoard[i][j] == 0) {
            bool clearSpace = true;
            
            // Check surrounding cells
            for(int di = -2; di <= 2; di++) {
                for(int dj = -2; dj <= 2; dj++) {
                    int checkX = i + di;
                    int checkY = j + dj;
                    if(checkX >= 0 && checkX < 25 && 
                       checkY >= 0 && checkY < 21 && 
                       gameBoard[checkX][checkY] == 3) {
                        clearSpace = false;
                        break;
                    }
                }
                if(!clearSpace) break;
            }
            
            if(clearSpace) {
                // Place single fuel station cell
                gameBoard[i][j] = 3;
                placed = true;
            }
        }
        attempts++;
    }
}

*/




gameBoard[0][0] = 4;  // Station top-left
gameBoard[1][0] = 4;  // Station top-middle
gameBoard[2][0] = 4;  // Station top-right
gameBoard[0][1] = 4;  // Station bottom-left 
gameBoard[1][1] = 4;  // Station bottom-middle
gameBoard[2][1] = 4;  // Station bottom-right
}
    
    void GenerateBuildingColor(int i, int j) {
        buildingColors[i][j][0] = 0.2 + (rand()%80)/100.0;
        buildingColors[i][j][1] = 0.2 + (rand()%80)/100.0;
        buildingColors[i][j][2] = 0.2 + (rand()%80)/100.0;
    }
    
    void InitializePassengers() {
        for(int i=0; i<4; i++) {
            passengers[i].init();
        }
    }
    
    void InitializeObstacles() {
        numObstacles = 0;
        for(int i=0; i<10 && numObstacles < 10; i++) {
            int x = (rand() % (25-4) + 2) * 40;
            int y = (rand() % (21-4) + 2) * 40;
            if(gameBoard[x/40][y/40] == 0) { // Only on roads
                obstacles[numObstacles++].init(x, y, 20, 20, 
                    colors[BROWN][0], colors[BROWN][1], colors[BROWN][2]);
            }
        }
    }
    
void InitializeOtherCars() {
    numOtherCars = 0;
    for(int i=0; i<3 && numOtherCars < 20; i++) {
        int attempts = 0;
        bool placed = false;
        
        while(!placed && attempts < 50) {
            int x = (rand() % (25-4) + 2) * 40;
            int y = (rand() % (21-4) + 2) * 40;
            
            // Check if this is a road cell and has clear space around it
            if(gameBoard[x/40][y/40] == 0) {
                bool clearSpace = true;
                
                // Check for obstacles
                for(int o=0; o<numObstacles; o++) {
                    if(abs(x - obstacles[o].getX()) < 30 && 
                       abs(y - obstacles[o].getY()) < 30) {
                        clearSpace = false;
                        break;
                    }
                }
                
                // Check surrounding cells
                for(int dx = -1; dx <= 1; dx++) {
                    for(int dy = -1; dy <= 1; dy++) {
                        int checkX = x/40 + dx;
                        int checkY = y/40 + dy;
                        if(checkX >= 0 && checkX < 25 && 
                           checkY >= 0 && checkY < 21 && 
                           gameBoard[checkX][checkY] != 0) {
                            clearSpace = false;
                            break;
                        }
                    }
                    if(!clearSpace) break;
                }
                
                if(clearSpace) {
                    otherCars[numOtherCars++].init(x, y);
                    placed = true;
                }
            }
            attempts++;
        }
    }
}
    
    void MoveOtherCars() {
        for(int i=0; i<numOtherCars; i++) {
            otherCars[i].move();
        }
    }
        void increaseAllCarSpeeds() {
        for(int i=0; i<numOtherCars; i++) {
            otherCars[i].increaseSpeed();
        }
        cout << "All cars increased speed!" << endl;
    }
    // Methods to access game entities
    Passenger* getPassengers() {
        return passengers;
    }
    
    Obstacle* getObstacles() {
        return obstacles;
    }
    
    OtherCar* getOtherCars() {
        return otherCars;
    }
     void InitializeDeliveries() {
        for(int i=0; i<3; i++) {  // max deliveiea 3 ha mara is lia
            deliveries[i].init();
        }
    }
    
    void SwitchMode() {
        isTaxiMode = !isTaxiMode;
        InitializePassengers();
        if(!isTaxiMode) {
           
        
            InitializeDeliveries();
        }
    }
    
    bool getIsTaxiMode() const { return isTaxiMode; }
    
    DeliveryPackage* getDeliveries() {
        return deliveries;
    }
        void setIsTaxiMode(bool isTaxi) {
        isTaxiMode = isTaxi;
        if(!isTaxiMode) {
            InitializeDeliveries();
        }
    }

};
// Global game manager instance
GameManager gameManager;



class Leaderboard {
private:
    struct LeaderboardEntry {
        char playerName[16];  // Fixed size for binary writing
        int score;

        LeaderboardEntry() : score(0) { playerName[0] = '\0'; }
        LeaderboardEntry(string name, int sc) : score(sc) {
            strncpy(playerName, name.c_str(), 15);
            playerName[15] = '\0'; // Ensure null termination
        }
    };

    LeaderboardEntry entries[10];
    int numEntries;

public:
    Leaderboard() : numEntries(0) {
        // Initialize all entries to empty
        for (int i = 0; i < 10; i++) {
            entries[i] = LeaderboardEntry();
        }
        LoadFromFile();
    }

    void LoadFromFile() {
        ifstream file("highscores.txt", ios::binary);
        if (!file) {
            numEntries = 0;
            return;
        }

        file.read((char*)&numEntries, sizeof(int));
        if (numEntries > 10) numEntries = 10;
        
        for (int i = 0; i < numEntries; i++) {
            file.read((char*)&entries[i], sizeof(LeaderboardEntry));
        }
        file.close();
    }

    void saveToFile() {
        ofstream file("highscores.txt", ios::binary | ios::trunc);
        if (!file) return;

        file.write((char*)&numEntries, sizeof(int));
        for (int i = 0; i < numEntries; i++) {
            file.write((char*)&entries[i], sizeof(LeaderboardEntry));
        }
        file.close();
    }

    bool addEntry(string name, int score) {
        // Check if we should add this score
        if (numEntries < 10) {
            entries[numEntries++] = LeaderboardEntry(name, score);
        } 
        else if (score > entries[9].score) {
            entries[9] = LeaderboardEntry(name, score);
        } 
        else {
            return false; // Score not high enough
        }

        // Sort the entries
        sortEntries();
        saveToFile();
        return true;
    }

        
void display() {
        // Draw a semi-transparent background
  DrawRectangle(200, 150, 550, 450, colors[BLACK]);

// Draw border that matches exactly with the rectangle
DrawLine(200, 150, 750, 150, 2, colors[WHITE]);  // Top line
DrawLine(750, 150, 750, 600, 2, colors[WHITE]);  // Right line
DrawLine(750, 600, 200, 600, 2, colors[WHITE]);  // Bottom line
DrawLine(200, 600, 200, 150, 2, colors[WHITE]);  // Left line

// Center the title text above the box
DrawString(390, 570, "LEADERBOARD", colors[YELLOW]);
        
     if (numEntries == 0) {
    // Center "No high scores" text in the middle of the box
    DrawString(200 + 550/2 - 70, 150 + 450/2, "No high scores yet!", colors[WHITE]);
}
 else {
    // Start drawing entries 50px below the top of the box

          for (int i = 0; i < numEntries; i++) {
                string entryText = to_string(i + 1) + ")    " + entries[i].playerName +  " score :" + to_string(entries[i].score);
                DrawString(220, 500 - (i * 30), entryText, colors[WHITE]);
            }
            DrawString(200 + 550/2 - 120,190, "Press ENTER to return to menu", colors[YELLOW]);

}

        }
private:
    void sortEntries() {
        // Simple bubble sort
        for (int i = 0; i < numEntries - 1; i++) {
            for (int j = 0; j < numEntries - i - 1; j++) {
                if (entries[j].score < entries[j + 1].score) {
                    swap(entries[j], entries[j + 1]);
                }
            }
        }
    }
};

// MenuManager class implementation
class MenuManager {
private:
    string playerName;
    bool showLeaderboardFlag;
    bool isTaxiMode;
    bool gettingName;
    bool showRoleSelection;
    Leaderboard leaderboard;
    int menuSelection; // 0: Start, 1: Leaderboard, 2: EXIT
    string nameInput;

public:
    MenuManager() : showLeaderboardFlag(false), isTaxiMode(true), 
                   gettingName(false), showRoleSelection(false),
                   menuSelection(0), nameInput("") {
        leaderboard.LoadFromFile();
    }

    void showMainMenu() {
        showLeaderboardFlag = false;
        gettingName = false;
        showRoleSelection = false;
    }

    void showLeaderboard() {
        showLeaderboardFlag = true;
    }

    void updateLeaderboard(int score) {
        if ( !playerName.empty()) {
              leaderboard.addEntry(playerName, score);
        }
    }

    bool binCamp() {
        return true; // In menu mode
    }

    // Handle menu navigation
    void handleMenuInput(unsigned char key) {
        if (gettingName) {
            if (key == 13) { // Enter key
                if (!nameInput.empty()) {
                    playerName = nameInput;
                    gettingName = false;
                    showRoleSelection = true;
                }
            }
            else if (key == 8 && !nameInput.empty()) { // Backspace
                nameInput.pop_back();
            }
            else if (isalpha(key)) { // Only allow letters
                if (nameInput.length() < 15) { // Limit name length
                    nameInput += toupper(key);
                }
            }
            return;
        }

        if (showRoleSelection) {
            if (key == '1') {
                isTaxiMode = true;
                showRoleSelection = false;
            }
            else if (key == '2') {
                isTaxiMode = false;
                showRoleSelection = false;
            }
            else if (key == '3') {
                isTaxiMode = (rand() % 2 == 0);
                showRoleSelection = false;
            }
            return;
        }

        if (showLeaderboardFlag) {
            if (key == 13) { // Enter to return from leaderboard
                showLeaderboardFlag = false;
            }
            return;
        }

        switch(key) {
            case 'w': case 'W':
                menuSelection = (menuSelection - 1 + 3) % 3;
                break;
            case 's': case 'S':
                menuSelection = (menuSelection + 1) % 3;
                break;
            case 13: // Enter
                if (menuSelection == 0) { // Start game
                    gettingName = true;
                    nameInput = "";
                }
                else if (menuSelection == 1) { // Leaderboard
                    showLeaderboard();
                }
                else if (menuSelection == 2) { // Exit
                    exit(1);
                }
                break;
        }
    }

    void handleSpecialInput(int key) {
        if (gettingName || showRoleSelection || showLeaderboardFlag) return;

        switch(key) {
            case GLUT_KEY_UP:
                menuSelection = (menuSelection - 1 + 3) % 3;
                break;
            case GLUT_KEY_DOWN:
                menuSelection = (menuSelection + 1) % 3;
                break;
        }
    }
    void drawMenu() {
        // Draw black background
        DrawRectangle(0, 0, 1020, 840, colors[DARK_BLUE]);
        DrawString(350, 700, "RUSH HOUR", colors[WHITE]);
    // Left car (pointing right)
    DrawRoundRect(250, 685, 40, 20, colors[BLUE], 5); // Body
    DrawRoundRect(255, 690, 30, 10, colors[LIGHT_BLUE], 3); // Roof
    DrawCircle(258, 682, 5, colors[BLACK]); // Rear wheel
    DrawCircle(282, 682, 5, colors[BLACK]); // Front wheel
    DrawRectangle(290, 685, 5, 10, colors[YELLOW]); // Headlight
    
    // Right car (pointing left)
    DrawRoundRect(730, 685, 40, 20, colors[BLUE], 5); // Body
    DrawRoundRect(735, 690, 30, 10, colors[LIGHT_BLUE], 3); // Roof
    DrawCircle(738, 682, 5, colors[BLACK]); // Rear wheel
    DrawCircle(762, 682, 5, colors[BLACK]); // Front wheel
    DrawRectangle(730, 685, 5, 10, colors[YELLOW]); // Headlight

        if (gettingName) {
            drawNameInput();
        }
        else if (showRoleSelection) {
            drawRoleSelection();
        }
        else if (showLeaderboardFlag) {
            leaderboard.display();
        }
        else {
            drawMainMenu();
        }
    }

    void drawNameInput() {
        // Input box with glow
        float boxColor[] = {0.1, 0.1, 0.2};
        float glowColor[] = {0.0, 0.5, 0.8};
        
        for(int i = 0; i < 3; i++) {
            DrawRoundRect(300-i, 400-i, 400+i*2, 100+i*2, glowColor, 15);
        }
        DrawRoundRect(300, 400, 400, 100, boxColor, 15);
        
        DrawString(320, 450, "ENTER YOUR NAME:", colors[CYAN]);
        DrawString(320, 420, nameInput + "_", colors[WHITE]); // Blinking cursor
        DrawString(320, 380, "Press ENTER when done", colors[GRAY]);
        
        // Draw small car icon
        DrawRoundRect(320, 360, 30, 15, colors[BLUE], 5);
    }

    void drawRoleSelection() {
       
        float boxColor[] = {0.1, 0.1, 0.2};
        float glowColor[] = {0.0, 0.8, 0.8};
        
        for(int i = 0; i < 3; i++) {
            DrawRoundRect(250-i, 300-i, 500+i*2, 300+i*2, glowColor, 15);
        }
        DrawRoundRect(250, 300, 500, 300, boxColor, 15);
        
        DrawString(350, 550, "SELECT YOUR ROLE", colors[CYAN]);
        
        // Draw options with icons
        DrawString(300, 500, "1. Taxi Driver", 
                  menuSelection == 0 ? colors[YELLOW] : colors[WHITE]);
        DrawTaxiIcon(270, 480);
        
        DrawString(300, 450, "2. Delivery Driver", 
                  menuSelection == 1 ? colors[YELLOW] : colors[WHITE]);
        DrawPackageIcon(270, 430);
        
        DrawString(300, 400, "3. Random Role", 
                  menuSelection == 2 ? colors[YELLOW] : colors[WHITE]);
        DrawDiceIcon(270, 380);
        
        DrawString(300, 350, "Current: " + string(isTaxiMode ? "Taxi Driver" : "Delivery Driver"), 
                  colors[WHITE]);
    }

    void drawMainMenu() {
        // Menu options container
        float boxColor[] = {0.1, 0.1, 0.2};
        DrawRoundRect(200, 150, 600, 400, boxColor, 15);
        
        // Draw menu options with selection indicator
        for (int i = 0; i < 3; i++) {
            float yPos = 450 - i * 100;
            
            if (i == menuSelection) {
               
                DrawRoundRect(280, yPos - 30, 400, 50, colors[CYAN], 10);
                DrawString(300, yPos, 
                    i == 0 ? "START GAME" : 
                    i == 1 ? "LEADERBOARD" : 
                    "EXIT", 
                    colors[BLACK]);
                
         
                if(i == 0) DrawCarIcon(650, yPos-25);
                else if(i == 1) DrawTrophyIcon(650, yPos-25);
                else DrawExitIcon(650, yPos-25);
            }
            else {
                // Normal option
                DrawRoundRect(280, yPos - 30, 400, 50, colors[DARK_GRAY], 10);
                DrawString(300, yPos, 
                    i == 0 ? "START GAME" : 
                    i == 1 ? "LEADERBOARD" : 
                    "EXIT", 
                    colors[WHITE]);
            }
        }
        
        // Draw instructions at bottom
        DrawString(350, 100, "Use W/S or UP/DOWN to navigate", colors[GRAY]);
        DrawString(380, 70, "ENTER to select", colors[GRAY]);
        
        // Draw decorative cars at bottom
        DrawMovingCar(100, 50);
        DrawMovingCar(800, 50);
    }

    // Helper functions to draw icons
    void DrawTaxiIcon(int x, int y) {
        DrawRoundRect(x, y, 30, 15, colors[YELLOW], 5);
        DrawRoundRect(x+5, y+3, 20, 9, colors[LIGHT_YELLOW], 3);
    }
    
    void DrawPackageIcon(int x, int y) {
        DrawRectangle(x, y, 25, 15, colors[BROWN]);
        DrawRectangle(x, y+5, 25, 5, colors[KHAKI]);
    }
    
    void DrawDiceIcon(int x, int y) {
        DrawRectangle(x, y, 20, 20, colors[WHITE]);
        // Draw dots
        DrawCircle(x+5, y+5, 2, colors[BLACK]);
        DrawCircle(x+15, y+15, 2, colors[BLACK]);
    }
    
void DrawCarIcon(int x, int y) {
    // Car body
    DrawRoundRect(x, y, 40, 20, colors[BLUE], 5);
    // Roof/windshield
    DrawRoundRect(x+5, y+5, 30, 10, colors[LIGHT_BLUE], 3);
    // Wheels
    DrawCircle(x+8, y+5, 5, colors[BLACK]);
    DrawCircle(x+32, y+5, 5, colors[BLACK]);
    // Headlights
    DrawRectangle(x+35, y+8, 5, 4, colors[YELLOW]);
}
    
    void DrawTrophyIcon(int x, int y) {
        DrawTriangle(x+15, y+20, x+5, y, x+25, y, colors[GOLD]);
        DrawRectangle(x+10, y-10, 10, 10, colors[GOLD]);
    }
    
    void DrawExitIcon(int x, int y) {
        DrawLine(x, y, x+20, y+20, 3, colors[RED]);
        DrawLine(x+20, y, x, y+20, 3, colors[RED]);
    }
    
void DrawMovingCar(int x, int y) {
    static int offset = 0;
    offset = (offset + 2) % 1020; // Smooth continuous movement
    
    // Car body
    DrawRoundRect(x + offset, y, 40, 20, colors[RED], 5);
    
    // Cabin/windshield
    DrawRoundRect(x + offset + 5, y + 5, 30, 10, colors[GHOST_WHITE], 3);
    
    // Wheels
    DrawCircle(x + offset + 8, y + 5, 5, colors[BLACK]);
    DrawCircle(x + offset + 32, y + 5, 5, colors[BLACK]);
    
    // Windows
    DrawLine(x + offset + 15, y + 15, x + offset + 25, y + 15, 2, colors[BLACK]);
    
    // Headlights
    DrawRectangle(x + offset + 35, y + 8, 5, 4, colors[YELLOW]);
    
    // Taillights
    DrawRectangle(x + offset, y + 8, 5, 4, colors[DARK_RED]);
}

    void setPlayerName(const string& name) {
        playerName = name;
    }

    void setRole(bool isTaxi) {
        isTaxiMode = isTaxi;
    }

    bool getIsTaxiMode() const {
        return isTaxiMode;
    }

    bool shouldStartGame() const {
        return !gettingName && !showRoleSelection && !showLeaderboardFlag && 
               menuSelection == 0 && !playerName.empty();
    }

    bool isInMenu() const {
        return gettingName || showRoleSelection || showLeaderboardFlag || 
              (!playerName.empty() && menuSelection == 0);
    }
};

// Global variables
MenuManager menuManager;
bool inMenuMode = true;


class drawer {
private:
    int uiX;
    
public:
    drawer() : uiX(1020 - 200) {}
    
    void clearScreen() {
        glClearColor(0.15, 0.15, 0.15, 1.0);
        glClear(GL_COLOR_BUFFER_BIT);
    }
void drawGameBoard() {
    for(int i=0; i<25; i++) {
        for(int j=0; j<21; j++) {
            int x = i*40;
            int y = j*40;
            
            if(gameBoard[i][j] == 0) { // Road
                bool isMainRoad = (i % (3*2) == 0);
                drawRoad(x, y, isMainRoad, j);
            }
      
            else if(gameBoard[i][j] == 3) { // Fuel station
                drawFuelStation(x, y);
            }
            else if(gameBoard[i][j] == 4) { // Station
                // Only draw once for the top-left cell to avoid overlapping
                if(i == 0 && j == 0) {
                    drawStation(x, y);
                }
            }
        }
    }
}
    
    void drawRoad(int x, int y, bool isMainRoad, int rowIndex) {
        // Smooth road surface
        DrawRectangle(x, y, 40*2, 40, ROAD_GRAY);
        
        // Center line (thicker for main roads)
        if(isMainRoad) {
            // Double yellow center line
            DrawLine(x+40, y, x+40, y+40, 6, BRIGHT_YELLOW);
            
            // White lane markers
            for(int ly = y+10; ly < y+40; ly += 20) {
                DrawLine(x+40/2, ly, x+40/2, ly+10, 2, colors[WHITE]);
                DrawLine(x+40*3/2, ly, x+40*3/2, ly+10, 2, colors[WHITE]);
            }
        } else {
            // Single dashed white center line
            for(int ly = y+8; ly < y+40; ly += 25) {
                DrawLine(x+40, ly, x+40, ly+12, 3, colors[WHITE]);
            }
        }
    }
  
    void drawFuelStation(int x, int y) {
    // Base
    DrawRectangle(x, y, 40, 40, colors[ORANGE]);
    
    // Roof with canopy
    DrawRectangle(x, y+40-15, 40, 15, colors[RED]);
    
    // Single pump in the middle
    DrawRectangle(x+15, y+10, 10, 25, colors[WHITE]);
    
    // Price sign
    DrawRectangle(x+10, y+40, 20, 10, colors[WHITE]);
    DrawString(x+12, y+42, "$5", colors[BLACK]);
    
    // Fuel symbol (simple drop shape)
    DrawTriangle(x+5, y+15, x+10, y+5, x+15, y+15, colors[BLUE]);
    DrawRectangle(x+8, y+15, 4, 10, colors[BLUE]);
}   
    void drawMovementEffect(int x, int y) {
        if( gamestate.getFuel() > 0) {
            DrawCircle(x+40/2, y+40/2, 3, colors[YELLOW]);
        }
    }
    
 void drawUI(int score, int fuel, int timeLeft, int wallet) {
    // Score (top right)
    DrawRoundRect(1020-170, 840-50, 150, 30, colors[DARK_BLUE], 10);
    DrawString(1020-160, 840-35, "Score: " + to_string(score), colors[WHITE]);

    // Fuel (left of score)
    DrawRoundRect(1020-330, 840-50, 150, 30, colors[DARK_RED], 10);
    DrawRectangle(1020-325, 840-45, (fuel * 140)/100, 20, colors[GREEN]);
    DrawString(1020-320, 840-35, "Fuel: " + to_string(fuel), colors[WHITE]);

    // Time (left of fuel)
    DrawRoundRect(1020-490, 840-50, 150, 30, colors[DARK_GREEN], 10);
    int minutes = timeLeft / 60;
    int seconds = timeLeft % 60;
    string timeStr = to_string(minutes) + ":" + (seconds < 10 ? "0" : "") + to_string(seconds);
    DrawString(1020-480, 840-35, "Time: " + timeStr, colors[WHITE]);

    // Wallet (left of time) - with gold color
    float goldColor[] = {1.0, 0.84, 0.0}; // RGB for gold
    DrawRoundRect(1020-650, 840-50, 150, 30, goldColor, 10);
    DrawString(1020-640, 840-35, "$" + to_string(wallet), colors[BLACK]);
}
    void drawStation(int x, int y) {
    // Draw 7 vertical lines
    for(int i = 0; i < 7; i++) {
        int lineX = x + (i * (40*3) / 6); // Evenly spaced
        DrawLine(lineX, y, lineX, y+40*2, 2, colors[BLACK]);
    }
    
    // Draw 5 horizontal lines
    for(int j = 0; j < 5; j++) {
        int lineY = y + (j * (40*2) / 4); // Evenly spaced
        DrawLine(x, lineY, x+40*3, lineY, 2, colors[BLACK]);
    }
        string modeText = "Current Mode: " + string(gameManager.getIsTaxiMode() ? "TAXI" : "DELIVERY");
    DrawString(x, y + 40*2 + 20, modeText, colors[WHITE]);
}
};




void SetCanvasSize(int width, int height) {
	glMatrixMode (GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1, 1); // set the screen size to given width and height.
	glMatrixMode (GL_MODELVIEW);
	glLoadIdentity();
}


bool IsValidPosition(int x, int y, int width, int height) {
    // Convert pixel coordinates to grid cells
    int leftCell = x / 40;
    int rightCell = (x + width - 1) / 40;
    int bottomCell = y / 40;
    int topCell = (y + height - 1) / 40;

    // Boundary check
    if(x < 0 || y < 0 || 
       (x + width) > 1020 || 
       (y + height) > 840) {
        return false;
    }

    // Grid cell check
    for(int i = leftCell; i <= rightCell; i++) {
        for(int j = bottomCell; j <= topCell; j++) {
            if(i < 0 || i >= 25 || j < 0 || j >= 21) {
                return false;
            }
            if(gameBoard[i][j] == 1) { // Building
                return false;
            }
        }
    }

    // Obstacle check
    Obstacle* obstacles = gameManager.getObstacles();
    for(int i = 0; i < numObstacles; i++) {
        if(obstacles[i].active && obstacles[i].checkCollision(x, y, width, height)) {
            return false;
        }
    }

    return true;
}
bool CheckCollision(int carX, int carY, int carWidth, int carHeight) {
    bool collisionDetected = false;
    int penalty = 0;
    
    // Adjusted collision box (smaller than actual car)
    const int COLLISION_BUFFER = 8;
    int adjustedX = carX + COLLISION_BUFFER;
    int adjustedY = carY + COLLISION_BUFFER;
    int adjustedWidth = carWidth - 2*COLLISION_BUFFER;
    int adjustedHeight = carHeight - 2*COLLISION_BUFFER;

    // Check buildings
    int leftCell = adjustedX / 40;
    int rightCell = (adjustedX + adjustedWidth - 1) / 40;
    int bottomCell = adjustedY / 40;
    int topCell = (adjustedY + adjustedHeight - 1) / 40;
    
    for(int i = leftCell; i <= rightCell; i++) {
        for(int j = bottomCell; j <= topCell; j++) {
            if(i < 0 || i >= 25 || j < 0 || j >= 21) {
                penalty = gameManager.getIsTaxiMode() ? -2 : -4; // Boundary hit
                collisionDetected = true;
                break;
            }
            if(gameBoard[i][j] == 1) { // Building
                penalty = gameManager.getIsTaxiMode() ? -2 : -4;
                collisionDetected = true;
                break;
            }
        }
        if(collisionDetected) break;
    }

    // Check obstacles
    Obstacle* obstacles = gameManager.getObstacles();
    for(int i=0; i<numObstacles && !collisionDetected; i++) {
        if(obstacles[i].active && obstacles[i].checkCollision(adjustedX, adjustedY, adjustedWidth, adjustedHeight)) {
            penalty = gameManager.getIsTaxiMode() ? -2 : -4;
            collisionDetected = true;
        }
    }

    // Check other cars
    OtherCar* otherCars = gameManager.getOtherCars();
    for(int i=0; i<numOtherCars && !collisionDetected; i++) {
        if(otherCars[i].active) {
            if(!(adjustedX > otherCars[i].getX() + otherCars[i].getWidth() - COLLISION_BUFFER || 
                 adjustedX + adjustedWidth < otherCars[i].getX() + COLLISION_BUFFER || 
                 adjustedY > otherCars[i].getY() + otherCars[i].getHeight() - COLLISION_BUFFER || 
                 adjustedY + adjustedHeight < otherCars[i].getY() + COLLISION_BUFFER)) {
                penalty = gameManager.getIsTaxiMode() ? -3 : -5;
                collisionDetected = true;
                otherCars[i].direction = (otherCars[i].direction + 1 + rand()%3) % 4;
            }
        }
    }

  if(!collisionDetected) {
        Passenger* passengers = gameManager.getPassengers();
        DeliveryPackage* deliveries = gameManager.getDeliveries();
        
        if(gameManager.getIsTaxiMode()) {
            // In taxi mode, only check passenger collisions if carrying a passenger
            if(gamestate.getCurrentPassenger() != -1) {
                for(int i=0; i<4; i++) {
                    if(passengers[i].waiting) {
                        int hitRange = 20;
                        if(abs(carX + carWidth/2 - passengers[i].x) < hitRange && 
                           abs(carY + carHeight/2 - passengers[i].y) < hitRange) {
                            penalty -= 5;
                            collisionDetected = true;
                            break;
                        }
                    }
                }
            }
        } 
        else {
            // In delivery mode, passengers always cause collisions
            for(int i=0; i<4; i++) {
                if(passengers[i].waiting) {
                    int hitRange = 20;
                    if(abs(carX + carWidth/2 - passengers[i].x) < hitRange && 
                       abs(carY + carHeight/2 - passengers[i].y) < hitRange) {
                        penalty -= 8;
                        collisionDetected = true;
                        break;
                    }
                }
            }
            
            // Also check package collisions if carrying a delivery
            if(!collisionDetected && gamestate.getCurrentDelivery() != -1) {
                for(int i=0; i<3; i++) {   // max deliveries 3 ha is lia
                    if(deliveries[i].waiting && i != gamestate.getCompletedDeliveries()) {
                        int hitRange = 20;
                        if(abs(carX + carWidth/2 - deliveries[i].x) < hitRange && 
                           abs(carY + carHeight/2 - deliveries[i].y) < hitRange) {
                            penalty -= 4;
                            collisionDetected = true;
                            break;
                        }
                    }
                }
            }
        }
    }

    if(collisionDetected) {
        gamestate.addScore(penalty);
        cout << "Collision! Penalty: " << penalty << " New score: " << gamestate.getScore() << endl;
    }
    
    return collisionDetected;
}
       

    drawer gameDrawer;
    void GameDisplay() {
            if (inMenuMode) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        menuManager.drawMenu();
        glutSwapBuffers();
        return;
    }
    gameDrawer.clearScreen();
    
    // Draw game board (roads, fuel stations, etc.)
    gameDrawer.drawGameBoard();
    
    // Draw buildings
    for(int i = 0; i < 25; i++) {
        for(int j = 0; j < 21; j++) {
            if(gameBoard[i][j] == 1) { // Building
                Building b(i*40, j*40);
                b.draw();
            }
        }
    }
    
    // Draw obstacles
    Obstacle* obstacles = gameManager.getObstacles();
    for(int i = 0; i < numObstacles; i++) {
        obstacles[i].draw();
    }
    
    // Draw other cars
    OtherCar* otherCars = gameManager.getOtherCars();
    for(int i = 0; i < numOtherCars; i++) {
        otherCars[i].draw();
    }
    
    // Draw player car
     PlayerCar playerCar(xI, yI, gameManager.getIsTaxiMode());
    playerCar.draw();
    
    // Draw passengers (always visible)
    Passenger* passengers = gameManager.getPassengers();
    for(int i = 0; i < 4; i++) {
        passengers[i].draw();
    }
    
    // Draw deliveries only in delivery mode
    if(!gameManager.getIsTaxiMode()) {
        DeliveryPackage* deliveries = gameManager.getDeliveries();
        for(int i = 0; i <3; i++) {   // max deliveries 3 aye is lie
            deliveries[i].draw();
        }
    }
    


    // this thingy i am doing is for displaying message if driver move to fuel station or role changer station
    int gridX = xI / 40;
    int gridY = yI / 40;
 if(gameBoard[gridX][gridY] == 3) {
    // Position text directly above the fuel station
    int posX = gridX * 40;
    int posY = gridY * 40 + 40 + 10; // 10 pixels above station
    DrawString(posX, posY, "Press F to refuel only $10 :)", colors[DARK_CYAN]);
}
if(gameBoard[gridX][gridY] == 4){
    int posX= gridX * 40;
    int posY = gridY * 40 + 40 + 10;
    DrawString(posX , posY , "Press P to change your role u broky " , colors[DARK_CYAN]);
     string(gameManager.getIsTaxiMode() ? "Taxi" : "Delivery") + ")", colors[DARK_CYAN];
}    
    // Draw UI
    gameDrawer.drawUI(gamestate.getScore(), gamestate.getFuel(), gamestate.getTimeLeft(), gamestate.getWallet());





        // game me out hojai toh screen ajai ge ye wali
            if (gamestate.isGameOver()) {
        // Draw game over screen
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        // Semi-transparent overlay
        DrawRectangle(0, 0, 1020, 840, colors[BLACK]);
        
        // Game over message
        DrawString(1020/2 - 100, 840/2 + 50, "GAME OVER", colors[RED]);
        
        // Reason for game over
        string reason = (gamestate.getTimeLeft() <= 0 ) ? "TIME'S UP!" : "OUT OF FUEL!";
        DrawString(1020/2 - 80, 840/2, reason, colors[WHITE]);
        
        if(gamestate.getScore() >= 100){
                    DrawString(1020/2 - 100, 840/2 - 50, 
                 "You Won Final Score: " + to_string(gamestate.getScore()), colors[YELLOW]);
        
        // Instructions
        DrawString(1020/2 - 150, 840/2 - 100, 
                 "Press ENTER to return to menu", colors[WHITE]);
        
        glutSwapBuffers();
        return;
        }
        // Final score
        DrawString(1020/2 - 100, 840/2 - 50, 
                 "Final Score: " + to_string(gamestate.getScore()), colors[YELLOW]);
        
        // Instructions
        DrawString(1020/2 - 150, 840/2 - 100, 
                 "Press ENTER to return to menu", colors[WHITE]);
        
        glutSwapBuffers();
        return;
            }
    if(gamestate.getScore() < 0){
        exit(1);

    }

    glutSwapBuffers();
}

/*This function is called (automatically) whenever any non-printable key (such as up-arrow, down-arraw)
 * is pressed from the keyboard
 *
 * You will have to add the necessary code here when the arrow keys are pressed or any other key is pressed...
 *
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 *
 * */
void NonPrintableKeys(int key, int x, int y) {

    if (inMenuMode) {
        menuManager.handleSpecialInput(key);
        glutPostRedisplay();
        return;
    }

    if (gamestate.getFuel() <= 0) return;

    static int collisionRecovery = 0; // Frames until control returns
    const int RECOVERY_TIME = 10;    // ~0.25 seconds at 60fps
    const int RECOIL_DISTANCE = 10;  // Stronger bounce-back
    
    // If in recovery period, ignore movement keys
    if (collisionRecovery > 0) {
        collisionRecovery--;
        glutPostRedisplay();
        return;
    }

    int newX = xI, newY = yI;
    const int carWidth = 40;
    const int carHeight = 40 / 2;

    // Movement input
    switch (key) {
        case GLUT_KEY_LEFT:  newX -= 6; break;
        case GLUT_KEY_RIGHT: newX += 6; break;
        case GLUT_KEY_UP:    newY += 6; break;
        case GLUT_KEY_DOWN:  newY -= 6; break;
    }

    if (CheckCollision(newX, newY, carWidth, carHeight)) {
        // Apply recoil effect
        switch (key) {
            case GLUT_KEY_LEFT:  xI += RECOIL_DISTANCE; break;
            case GLUT_KEY_RIGHT: xI -= RECOIL_DISTANCE; break;
            case GLUT_KEY_UP:    yI -= RECOIL_DISTANCE; break;
            case GLUT_KEY_DOWN:  yI += RECOIL_DISTANCE; break;
        }
        collisionRecovery = RECOVERY_TIME;
    } 
    else {
        // Normal movement
        xI = newX;
        yI = newY;
    }

    // Boundary checks
    xI = max(0, min(1020 - carWidth, xI));
    yI = max(0, min(840 - carHeight, yI));
    
    // Fuel consumption
    static int moveCount = 0;
    if (++moveCount % 8 == 0) gamestate.decrementFuel();

    glutPostRedisplay();
}

/*This function is called (automatically) whenever any printable key (such as x,b, enter, etc.)
 * is pressed from the keyboard
 * This function has three argument variable key contains the ASCII of the key pressed, while x and y tells the
 * program coordinates of mouse pointer when key was pressed.
 * */
void PrintableKeys(unsigned char key, int x, int y) {

        if (inMenuMode) {
        menuManager.handleMenuInput(key);
        
        if (menuManager.shouldStartGame()) {
            inMenuMode = false;
            gameManager.setIsTaxiMode(menuManager.getIsTaxiMode());
            gameManager.ResetGame(); // This will reset all game state
            glutPostRedisplay();
        }
        return;
    }
        if (gamestate.isGameOver()) {
        if (key == 13) { // Enter key
            inMenuMode = true;
            gamestate.setGameOver(false);
            glutPostRedisplay();
        }
        return;
    }
    
    if (key == 27) { // Escape key
        exit(1);
    }

    if (key == 'b' || key == 'B') {
        cout << "b pressed" << endl;
    }
if (key == 'p' || key == 'P') {
    // Convert car position to grid cells
    int gridX = xI / 40;
    int gridY = yI / 40;
    
    // Check if at station (any of the 6 cells)
    if((gridX == 0 || gridX == 1 || gridX == 2) && (gridY == 0 || gridY == 1)) {
        gameManager.SwitchMode();
        cout << "Switched to " << (gameManager.getIsTaxiMode() ? "Taxi" : "Delivery") << " mode" << endl;
               
        // Reset any carried passenger/delivery
        gamestate.setCurrentPassenger(-1);
        gamestate.setCurrentDelivery(-1);
    }
}
 if (key == 'f' || key == 'F') {
    int gridX = xI / 40;
    int gridY = yI / 40;
    
    if(gameBoard[gridX][gridY] == 3) {
        if(gamestate.getWallet() >= 10 && gamestate.getFuel() < 100) {   // fuel ki qeemat 10 dollar ha nawaz sharif ki hakoomat ha samjha karo
            gamestate.decrementWallet(10);
         gamestate.setFuel(min(100, gamestate.getFuel() + 25));// kiunke fuel refill 25 kar rha me is lia

            glutSwapBuffers();
            cout << "Refueled! Fuel: " << gamestate.getFuel() << " Money: $" << gamestate.getWallet() << endl;
        } else if(gamestate.getWallet() < 10) {
            cout << "Not enough money! Need $" << 10 << endl;
        } else {
            cout << "Tank is already full!" << endl;
        }
    }
}
    if (key == ' ') { // Spacebar for pickup/dropoff
        
        if(gameManager.getIsTaxiMode()){
        Passenger* passengers = gameManager.getPassengers();
     

        // Pickup logic
        if (gamestate.getCurrentPassenger() == -1) {
            for (int i = 0; i < 4; i++) {
                if (passengers[i].waiting && 
                    abs(xI - passengers[i].x) < 50 && 
                    abs(yI - passengers[i].y) < 50) {
                    passengers[i].waiting = false;
                    passengers[i].inTaxi = true;
                    gamestate.setCurrentPassenger(i);
                    cout << "Picked up passenger " << i << endl;
                    break;
                    
                }
            }
        } 
        // Dropoff logic
        else if (abs(xI - passengers[gamestate.getCurrentPassenger()].destX) < 50 && 
                 abs(yI - passengers[gamestate.getCurrentPassenger()].destY) < 50) {
            // Successful dropoff
            gamestate.addScore(10);
            gamestate.incrementCompletedPassengers();
            int fare = 5 + rand() % 10; // Random fare between 5-14
                gamestate.addWallet(fare); 
            cout << "Dropped passenger! +10 points, +" << fare << " dollars" << endl;
            
            // Add new car every 2 completed passengers
            if (gamestate.getCompletedPassengers() % 2 == 0 && numOtherCars < 20) {
                 gameManager.increaseAllCarSpeeds();
                int x, y;
                do {
                    x = (rand() % (25-4) + 2) * 40;
                    y = (rand() % (21-4) + 2) * 40;
                } while (gameBoard[x/40][y/40] != 0);
                
                OtherCar* otherCars = gameManager.getOtherCars();
                otherCars[numOtherCars++].init(x, y);
                cout << "New car added to traffic!" << endl;
            }

            // Respawn passenger with new destination
            passengers[gamestate.getCurrentPassenger()].inTaxi = false;
     do {
    passengers[gamestate.getCurrentPassenger()].x = (rand() % (25-4) + 2) * 40;
    passengers[gamestate.getCurrentPassenger()].y = (rand() % (21-4) + 2) * 40;
} while (gameBoard[passengers[gamestate.getCurrentPassenger()].x/40][passengers[gamestate.getCurrentPassenger()].y/40] != 0 ||
        (passengers[gamestate.getCurrentPassenger()].x/40 <= 2 && passengers[gamestate.getCurrentPassenger()].y/40 <= 1));

do {
    passengers[gamestate.getCurrentPassenger()].destX = (rand() % (25-4) + 2) * 40;
    passengers[gamestate.getCurrentPassenger()].destY = (rand() % (21-4) + 2) * 40;
} while (gameBoard[passengers[gamestate.getCurrentPassenger()].destX/40][passengers[gamestate.getCurrentPassenger()].destY/40] != 0 ||
        (passengers[gamestate.getCurrentPassenger()].destX/40 <= 2 && passengers[gamestate.getCurrentPassenger()].destY/40 <= 1));
            passengers[gamestate.getCurrentPassenger()].waiting = true;
            gamestate.setCurrentPassenger(-1);
        }
    }

            // Dropoff logic
    else {
        // Delivery mode logic
        DeliveryPackage* deliveries = gameManager.getDeliveries();
        
        // Pickup logic
        if(gamestate.getCurrentDelivery() == -1) {
            for(int i=0; i<3; i++) { // max deliveries 3 ha na bhai is lia
                if(deliveries[i].waiting && 
                   abs(xI - deliveries[i].x) < 50 && 
                   abs(yI - deliveries[i].y) < 50) {
                    deliveries[i].waiting = false;
                    deliveries[i].inCar = true;
                    gamestate.setCurrentDelivery(i);
                    cout << "Picked up package " << i << endl;
                    break;
                }
            }
        } 
        // Dropoff logic
        else if(abs(xI - deliveries[gamestate.getCurrentDelivery()].destX) < 50 && 
                abs(yI - deliveries[gamestate.getCurrentDelivery()].destY) < 50) {
            // Successful delivery
            gamestate.addScore(20);
            gamestate.incrementCompletedDeliveries();
            int payment = 10 + rand() % 15; // Random payment between 10-24
            gamestate.addWallet(payment);
            
            cout << "Delivered package! +20 points, +" << payment << " dollars" << endl;
            
            // Add new car every 2 completed deliveries
            if(gamestate.getCompletedDeliveries() % 2 == 0 && numOtherCars < 20) {
                 gameManager.increaseAllCarSpeeds();
                int x, y;
                do {
                    x = (rand() % (25-4) + 2) * 40;
                    y = (rand() % (21-4) + 2) * 40;
                } while(gameBoard[x/40][y/40] != 0);
                
                OtherCar* otherCars = gameManager.getOtherCars();
                otherCars[numOtherCars++].init(x, y);
                cout << "New car added to traffic!" << endl;
            }

            // Respawn package with new destination
            deliveries[gamestate.getCurrentDelivery()].inCar = false;
            deliveries[gamestate.getCurrentDelivery()].init();
            gamestate.setCurrentDelivery(-1);
        }
    }

    }

    
    
    glutPostRedisplay();
}


/*
 * This function is called after every 1000.0/FPS milliseconds
 * (FPS is defined on in the beginning).
 * You can use this function to animate objects and control the
 * speed of different moving objects by varying the constant FPS.
 *
 * */
// Replace your current Timer function with this:

void Timer(int m) {
    // Update game time
    static int secondCounter = 0;
    
    // Move other cars REGARDLESS of time left
    gameManager.MoveOtherCars();
    if ((gamestate.getTimeLeft() <= 0 || gamestate.getFuel() <= 0) && !gamestate.isGameOver()) {
       gamestate.setGameOver(true);
        // Update leaderboard with current score
        menuManager.updateLeaderboard(gamestate.getScore());
        
        // Reset player name so they can enter a new one
        menuManager.setPlayerName("");
    }
    // Only decrement time when game is still running
    if(gamestate.getTimeLeft() > 0 && !gamestate.isGameOver()) {
        // Only decrement timeLeft once per second (assuming FPS = 60)
        if(++secondCounter >= FPS) {
            gamestate.decrementTime();
            secondCounter = 0;
        }
    }
    
    // Always call glutPostRedisplay to refresh the screen
    glutPostRedisplay();
    
    // IMPORTANT: Always reschedule the Timer function, even after game ends
    glutTimerFunc(1000.0/FPS, Timer, 0);
}

/*This function is called (automatically) whenever your mouse moves witin inside the game window
 *
 * You will have to add the necessary code here for finding the direction of shooting
 *
 * This function has two arguments: x & y that tells the coordinate of current position of move mouse
 *
 * */
void MousePressedAndMoved(int x, int y) {
    cout << x << " " << y << endl;
    glutPostRedisplay();
}

void MouseMoved(int x, int y) {
    glutPostRedisplay();
}

/*This function is called (automatically) whenever your mouse button is clicked witin inside the game window
 *
 * You will have to add the necessary code here for shooting, etc.
 *
 * This function has four arguments: button (Left, Middle or Right), state (button is pressed or released),
 * x & y that tells the coordinate of current position of move mouse
 *
 * */
void MouseClicked(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON) // dealing only with left button
    {
        cout << GLUT_DOWN << " " << GLUT_UP << endl;
    } 
    else if (button == GLUT_RIGHT_BUTTON) // dealing with right button
    {
        cout << "Right Button Pressed" << endl;
    }
    glutPostRedisplay();
}

/*
 * our gateway main function
 * */
int main(int argc, char*argv[]) {
    int width = 1020, height = 840; // Window size

    // Initialize the game
    // Note: GameManager constructor will handle initialization
    
    InitRandomizer(); // seed the random number generator...
    glutInit(&argc, argv); // initialize the graphics library...
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA); // we will be using color display mode
    glutInitWindowPosition(50, 50); // set the initial position of our window
    glutInitWindowSize(width, height); // set the size of our window
    glutCreateWindow("OOP Project"); // set the title of our game window
    SetCanvasSize(width, height); // set the number of pixels...

    // Register callback functions
    glutDisplayFunc(GameDisplay); // tell library which function to call for drawing Canvas.
    glutSpecialFunc(NonPrintableKeys); // tell library which function to call for non-printable ASCII characters
    glutKeyboardFunc(PrintableKeys); // tell library which function to call for printable ASCII characters
    glutTimerFunc(1000.0, Timer, 0); // Set timer function
    glutMouseFunc(MouseClicked);
    glutPassiveMotionFunc(MouseMoved); // Mouse
    glutMotionFunc(MousePressedAndMoved); // Mouse

menuManager.updateLeaderboard(gamestate.getScore());
inMenuMode = true;
    // Start the main game loop
    glutMainLoop();
    return 1;
}
#endif /* RushHour_CPP_ */