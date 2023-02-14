/*
Creator: Michael M. Smith

Description: This game is called shape destroyer and the objective is to destroy all the enemy shapes without
running out of bullets. The User moves around and shoots bullets at the enemy shapes and needs to hit them in order
to destroy the enemy shape. The User is an independent object, but the Enemy Shapes and Bullets are objects inherited
from the parent class, "GameObject".
 */


#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <queue>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 1000;
const int NUM_ENEMY_SHAPES = 10;
const int NUM_BULLETS = 15;

// PARENT CLASS
class GameObject {
public:
    sf::RectangleShape attr;
    float xVelocity;
    float yVelocity;
    bool isActive = false;
};

// INHERITS "GAME OBJECT" CLASS
class EnemyShape: public GameObject {
public:
    int enemyShapeNum = 0;

    static EnemyShape create_EnemyShape(){
        sf::Vector2f rectanglePosition((rand() % (WIDTH - 50)) + 25, rand() % ((HEIGHT *  3/4)  + 10));

        EnemyShape enemyShape;
        enemyShape.attr.setPosition(rectanglePosition);
        enemyShape.attr.setSize(sf::Vector2f(50, 50));
        enemyShape.attr.setFillColor(sf::Color::Yellow);

        // Determines speed for x & y axis
        int IntArray [2] = {1, -1};
        int xRandIndex = rand() % 2;
        int yRandIndex = rand() % 2;

        enemyShape.xVelocity = IntArray[xRandIndex];
        enemyShape.yVelocity = IntArray[yRandIndex];

        return enemyShape;
    }

    static vector<EnemyShape> createEnemyShapeVect(vector<EnemyShape> & enemyShapeVect){
        for ( int i = 0; i < NUM_ENEMY_SHAPES; i++ ){
            EnemyShape enemy = EnemyShape::create_EnemyShape();
            enemy.enemyShapeNum = i + 1;
            enemy.isActive = true;
            enemyShapeVect.push_back(enemy);
        }
        return enemyShapeVect;
    }
};

// INHERITS "GAME OBJECT" CLASS
class Bullet: public GameObject {
public:

    bool bulletUp = false;
    bool bulletDown = false;
    bool bulletLeft = false;
    bool bulletRight = false;

    static Bullet create_Bullet() {
        Bullet bullet;
        bullet.attr.setSize(sf::Vector2f(10, 10));
        bullet.attr.setFillColor(sf::Color::Red);
//        cout << "Printing player position X: " << player.getPosition().x << " & y: " <<  player.getPosition().y << endl;

        cout << "Bullet Created" << endl;
        return bullet; // Returning an object (bullet)
    }

    static queue<Bullet> loadBulletQueue(queue<Bullet> & bulletQueue) {
        for ( int i = 0; i < NUM_BULLETS; i++ ){
            Bullet bullet = Bullet::create_Bullet();
            bulletQueue.push(bullet);
        }
        return bulletQueue;
    }

    static void fireBullet(queue<Bullet> & bulletQueue, vector<Bullet> & bulletVector){

        bulletQueue.front().isActive = true;
        bulletVector.push_back(bulletQueue.front());
        bulletQueue.pop();
    }

    static Bullet moveBullet(Bullet bullet, int x, int y, int p, int n) {
        bullet.xVelocity = x;
        bullet.yVelocity = y;
        sf::Vector2f bullet_pos = bullet.attr.getPosition();

        // Directs the bullet in correct direction based on input
        bullet.xVelocity *= p;
        bullet.yVelocity *= n;

        bullet_pos.x += bullet.xVelocity;
        bullet_pos.y += bullet.yVelocity;
        bullet.attr.setPosition(bullet_pos);

        return bullet;
    }
};


// HELPER FUNCTIONS
bool EnemyShapeUserCollision(sf::Vector2f player_position, sf::Vector2f rect_position){
    if ( (player_position.x - 40 <= rect_position.x && rect_position.x < player_position.x + 40)
    && (player_position.y - 40 <= rect_position.y && rect_position.y < player_position.y + 40) ){
        return true;
    }
    return false;
}

bool EnemyShapeBulletCollision( sf::Vector2f enemyShapePosition,  sf::Vector2f bulletPosition){
    if ( (bulletPosition.x <= enemyShapePosition.x + 50 && enemyShapePosition.x <= bulletPosition.x)
    && (bulletPosition.y <= enemyShapePosition.y + 50 && enemyShapePosition.y <= bulletPosition.y) ){
        return true;
    }
    return false;
}

sf::Text setTextCharacteristics(sf::Text & textObject, int xPos, int yPos, int charSize) {
    textObject.setPosition(xPos, yPos);
    textObject.setCharacterSize(charSize);
    textObject.setStyle(sf::Text::Bold);

    return textObject;
}

void checkEnemyShapeBoundaries(vector<EnemyShape> & enemyShapeVect, int i, sf::Vector2f enemyShapePosition){
    int Xpos = enemyShapeVect[i].attr.getPosition().x;
    int Ypos = enemyShapeVect[i].attr.getPosition().y;

    // Ensures enemy shapes stay in window
    if ( Xpos < 0 || Xpos > WIDTH - 50 ) enemyShapeVect[i].xVelocity *= -1; // 50 for Enemy Shape width
    if ( Ypos < 0 || Ypos > HEIGHT - 50 ) enemyShapeVect[i].yVelocity *= -1; // 50 for Enemy Shape width

    enemyShapePosition.x += enemyShapeVect[i].xVelocity;
    enemyShapePosition.y += enemyShapeVect[i].yVelocity;
    enemyShapeVect[i].attr.setPosition(enemyShapePosition);
}

Bullet checkBulletBoundaries(Bullet bullet){
    int XBulletpos = bullet.attr.getPosition().x;
    int YBulletpos = bullet.attr.getPosition().y;

    // If bullet hits boundary of window, delete bullet
    if ( (XBulletpos < 0 || XBulletpos > WIDTH - 10) && bullet.isActive ){ // 10 for Enemy Shape width
        bullet.isActive = false;
    }
    if ( (YBulletpos < 0 || YBulletpos > HEIGHT - 10) && bullet.isActive ){ // 10 for Enemy Shape width
        bullet.isActive = false;
    }

    return bullet;
}



int main() {

    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Shape Destroyers");
    window.setFramerateLimit(60);

    // CREATE USER SHAPE
    sf::CircleShape player;
    player.setRadius(20);
    player.setFillColor(sf::Color::Green);
    player.setOrigin(1, 1);
    player.setPosition(WIDTH / 2, HEIGHT / 1.2);

    // INITIALIZE VARIABLES
    bool keyPressed = false;
    bool gameOver = false;
    bool beginBuffer = true;
    bool outOfBullets = false;
    bool enemyShapeVectEmpty = false;
    int score = 0;

    vector<int> WASDvect = {0, 0, 0, 0};

    // CREATE ENEMY SHAPES AND ADD THEM TO A VECTOR
    vector<EnemyShape> enemyShapeVect;
    EnemyShape::createEnemyShapeVect(enemyShapeVect);

    // LOAD THE BULLET QUEUE WITH BULLETS
    queue<Bullet> bulletQueue;
    Bullet::loadBulletQueue(bulletQueue);

    // DECLARE AND LOAD FONT
    sf::Font font;
    if ( !font.loadFromFile("arial.ttf") ) exit(1); // error...

    // INITIATE AND SPECIALIZE TEXTS
    //     Score Text
    sf::Text scoreText("Score: " + to_string(score), font); // Cast type score int to a string --> to_string(score)
    scoreText.setFillColor(sf::Color::Green);

    //     Number of Bullets Text
    sf::Text numBullets(to_string(bulletQueue.size()), font); // Cast type bulletQueue.size() int to a string --> bulletQueue.size()
    scoreText.setFillColor(sf::Color::Green);

    //     You Win Text
    sf::Text youWin("YOU WIN!", font);
    scoreText.setFillColor(sf::Color::Green);

    //     OutOfBullets Text
    sf::Text OutOfBullets("Out Of Bullets :(", font);
    OutOfBullets.setFillColor(sf::Color::Cyan);

    //     Game Over Text
    sf::Text GameOver("GAME OVER", font);
    GameOver.setFillColor(sf::Color::Red);

    //     Begin Buffer Text
    int time = 3;
    sf::Text BeginBuffer(to_string(time), font); // Cast type time int to a string --> to_string(time)
    BeginBuffer.setFillColor(sf::Color::Blue);


    // Creates bullets and adds them to the bulletVect
    vector<Bullet> bulletVect;
    for ( int i = 0; i < NUM_BULLETS; i++ ){
        Bullet bullet = Bullet::create_Bullet();
        bulletVect.push_back(bullet);
    }


    while ( window.isOpen() ){

        // CREATE EVENT FOR EVENT LOOP
        sf::Event event;
        while ( window.pollEvent(event) ){
            if ( event.type == sf::Event::Closed )
                window.close();
        }

        if (enemyShapeVect.empty() ){
            enemyShapeVectEmpty = true;
        }
        if (  bulletQueue.empty() &&  bulletVect.empty() ){ // TODO: Fix this
            outOfBullets = true;
            gameOver = true;
        }

        // Get position of enemy shape and user
        sf::Vector2f playerPosition = player.getPosition();

        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Escape) ) // Press Escape to Close Window
            window.close();
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Left) )
            player.move(-5.0f, 0.0f);
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Right) )
            player.move(5.0f, 0.0f);
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Up) )
            player.move(0.0f, -5.0f);
        if ( sf::Keyboard::isKeyPressed(sf::Keyboard::Down) )
            player.move(0.0f, 5.0f);
        if( sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !keyPressed ){
            WASDvect[0] = 1;
        }
        else if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !keyPressed){
            WASDvect[1] = 1;
        }
        else if( sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !keyPressed){
            WASDvect[2] = 1;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !keyPressed){
            WASDvect[3] = 1;
        }
        else{
            keyPressed = false;
        }

        if (event.type == sf::Event::KeyReleased){
            if ( WASDvect[0] ) {  // Shoot bullet Upwards
                keyPressed = true;

                bulletQueue.front().attr.setPosition(playerPosition.x + 14.5, playerPosition.y);
                bulletQueue.front().bulletUp = true;
                cout << "Adding bulletQueue.front() to bulletVect for W pressed" << endl;

                WASDvect[0] = 0;
//                W = false;

            }
            else if ( WASDvect[1] ) { // Shoot bullet Left
                keyPressed = true;

                bulletQueue.front().attr.setPosition(playerPosition.x, playerPosition.y + 15);
                bulletQueue.front().bulletLeft = true;
                cout << "Adding bulletQueue.front() to bulletVect for A pressed" << endl;
                WASDvect[1] = 0;
            }
            else if ( WASDvect[2] ) { // Shoot bullet Down
                keyPressed = true;

                bulletQueue.front().attr.setPosition(playerPosition.x + 14.5, playerPosition.y + 29);
                bulletQueue.front().bulletDown = true;
                cout << "Adding bulletQueue.front() to bulletVect for S pressed" << endl;
                WASDvect[2] = 0;
            }
            else if ( WASDvect[3] ) { // Shoot bullet Right
                keyPressed = true;

                bulletQueue.front().attr.setPosition(playerPosition.x + 29, playerPosition.y + 15);
                bulletQueue.front().bulletRight = true;
                cout << "Adding bulletQueue.front() to bulletVect for D pressed" << endl;
                WASDvect[3] = 0;
            }
        }


        if (!bulletQueue.empty() && keyPressed) Bullet::fireBullet(bulletQueue, bulletVect);

        // MOVING BULLET
        for (int j = 0; j < bulletVect.size(); j++) {
            int XBulletpos = bulletVect[j].attr.getPosition().x;
            int YBulletpos = bulletVect[j].attr.getPosition().y;

            if (bulletVect[j].bulletUp){
                bulletVect[j] = Bullet::moveBullet(bulletVect[j], 0, -3, 1, 1);
            }
            else if(bulletVect[j].bulletLeft){
                bulletVect[j] = Bullet::moveBullet(bulletVect[j], 3, 0, -1, 1);
            }
            else if(bulletVect[j].bulletDown){
                bulletVect[j] = Bullet::moveBullet(bulletVect[j], 0, 3, 1, 1);
            }
            else if(bulletVect[j].bulletRight){
                bulletVect[j] = Bullet::moveBullet(bulletVect[j], 3, 0, 1, 1);
            }

            // Delete bullet if it goes out of the boundaries
            bulletVect[j] = checkBulletBoundaries(bulletVect[j]);
        }



        for ( int i = 0; i < enemyShapeVect.size(); i++ ){
            // Get position of Objects
            sf::Vector2f playerPosition = player.getPosition();
            sf::Vector2f enemyShapePosition = enemyShapeVect[i].attr.getPosition();

            // Check if position of enemy shape and user hit each other
            if ( EnemyShapeUserCollision(playerPosition, enemyShapePosition) ){
                gameOver = true;
            }

            // Traverse through each active bullet for collisions with any objects
            for (int j = 0; j < bulletVect.size(); j++){
                sf::Vector2f bulletPosition = bulletVect[j].attr.getPosition();

                // Check if position of enemy shape and a bullet hit each other
                if ( EnemyShapeBulletCollision(enemyShapePosition, bulletPosition) && bulletVect[j].isActive ){
//                cout << "Enemy Shape Collision...enemy object # = " << enemyShapeVect[i].enemyShapeNum << endl;
                    enemyShapeVect[i].isActive = false;
                    bulletVect[j].isActive = false;
                    score += 1;  // Increment Score
                }
            }

            // Check if Enemy Shape is within the Boundaries of the Window
            checkEnemyShapeBoundaries(enemyShapeVect, i, enemyShapePosition);
        }


        // Check if user hits boundary of window
        if ( (playerPosition.x < 0 || playerPosition.x > WIDTH - 40)
        ||  (playerPosition.y < 0 || playerPosition.y > HEIGHT - 40) ) gameOver = true; // 40 for User width

        // REMOVE NECESSARY OBJECTS
        //     Remove the Enemy Shape Objects that have been destroyed
        // TODO: Check if this can be made faster than 0(n) time complexity
        for ( int i = 0; i < enemyShapeVect.size(); i++ ){
            if ( !enemyShapeVect[i].isActive ){
                swap(enemyShapeVect[i], enemyShapeVect.back());
                cout << "Enemy Shape about to be Deleted:...enemy object # = " << enemyShapeVect.back().enemyShapeNum << endl;
                enemyShapeVect.pop_back();
            }
        }


        // DELETE NON ACTIVE BULLETS IN BULLET VECT
        for (int i = 0; i < bulletVect.size(); i++ ){
            if(!bulletVect[i].isActive){
                swap(bulletVect[i], bulletVect.back());
                bulletVect.pop_back();
            }
        }

        // UPDATE SCORE
        scoreText.setString("Score: " + to_string(score));

        // RENDER/DRAW OBJECTS
        //     Draw countdown screen at the beginning
        if ( beginBuffer ){
            for ( int i = 0; i < 4; i++ ){
                window.clear();
                BeginBuffer = setTextCharacteristics(BeginBuffer, 640, 450, 100);
                window.draw(BeginBuffer);
                sleep(sf::seconds(1.0f));
                window.display();
                time--;
                BeginBuffer.setString(to_string(time));
            }
            beginBuffer = false;
        }
        //     Draw the victory screen if the user wins
        else if ( enemyShapeVectEmpty ){
            window.clear();
            youWin = setTextCharacteristics(youWin, 400, 450, 100);
            window.draw(youWin);
            scoreText = setTextCharacteristics(scoreText, 450, 600, 100);
            window.draw(scoreText);
            window.display();
        }
        //     Draw the Game over screen
        else if ( gameOver ){
            window.clear();
            GameOver = setTextCharacteristics(GameOver, WIDTH / 3.8, HEIGHT / 2.5, 100);
            window.draw(GameOver);
            scoreText = setTextCharacteristics(scoreText, 450, 700, 100);
            window.draw(scoreText);
            if ( outOfBullets ){
                OutOfBullets = setTextCharacteristics(OutOfBullets, 300, 550, 100);
                window.draw(OutOfBullets);
            }
            window.display();
        }
        //     Draw Main Screen for the Game
        else {
            window.clear();
            scoreText = setTextCharacteristics(scoreText, 50, 25, 50);
            window.draw(scoreText);
            numBullets.setString("# Of Bullets Left: " + to_string(bulletQueue.size()));
            numBullets = setTextCharacteristics(numBullets, 50, 925, 25);
            window.draw(numBullets);
            window.draw(player);
            for ( int i = 0; i < NUM_ENEMY_SHAPES; i++ ){
                window.draw(enemyShapeVect[i].attr);
            }
            for (int j = 0; j < bulletVect.size(); j++){
                if ( bulletVect[j].isActive ){
                    window.draw(bulletVect[j].attr);
                }
            }
            window.display();
        }
    }

    return 0;
}


// TODO:
//  - sometimes enemy shapes move horizontally across side of screen
//      - I think it's bc rand sometimes is 0
//  - Fix enemy shapes spawning near user at the beginning
//  - Fix all fixed numbers and put them as variables
//  - Delete Enemy Shape Object Num before I send it
//  - Maybe don't use pass by reference - is it harder to delete objects/memory?
//  - Is it possible to make anywhere more efficient? Better time complexity?

