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

// PARENT CLASS (Polymorphism)
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

    static vector<EnemyShape> createEnemyShapeVect( vector<EnemyShape> & enemyShapeVect ) {
        for ( int i = 0; i < NUM_ENEMY_SHAPES; i++ ) {
            EnemyShape enemy = EnemyShape::create_EnemyShape();
            enemy.isActive = true;
            enemyShapeVect.push_back( enemy );
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
    int width = 10;
    int height = 10;

    // Create bullet object and specialize attributes
    static Bullet create_Bullet() {
        Bullet bullet;
        bullet.attr.setSize( sf::Vector2f(bullet.width, bullet.height) );
        bullet.attr.setFillColor( sf::Color::Red );

        return bullet; // Returning an object (bullet)
    }

    // Loading the queue with a fixed number of bullets
    static queue<Bullet> loadBulletQueue( queue<Bullet> & bulletQueue ) {
        for ( int i = 0; i < NUM_BULLETS; i++ ){
            Bullet bullet = Bullet::create_Bullet();
            bulletQueue.push( bullet );
        }
        return bulletQueue;
    }

    // Take ojecct from bulletQueue and add it to activeBulletVect since bullet has been fired - pop a bullet from bulletQueue
    static void fireBullet( queue<Bullet> & bulletQueue, vector<Bullet> & activeBulletVect ) {
        bulletQueue.front().isActive = true;
        activeBulletVect.push_back( bulletQueue.front() );
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
        bullet.attr.setPosition( bullet_pos) ;

        return bullet;
    }
};


// HELPER FUNCTIONS
bool EnemyShapeUserCollision( sf::Vector2f player_position, sf::Vector2f rect_position ) {
    if ( (player_position.x - 40 <= rect_position.x && rect_position.x < player_position.x + 40)
    && (player_position.y - 40 <= rect_position.y && rect_position.y < player_position.y + 40) ) {
        return true;
    }
    return false;
}

bool EnemyShapeBulletCollision( sf::Vector2f enemyShapePosition, sf::Vector2f bulletPosition ) {
    if ( (bulletPosition.x <= enemyShapePosition.x + 50 && enemyShapePosition.x <= bulletPosition.x)
    && (bulletPosition.y <= enemyShapePosition.y + 50 && enemyShapePosition.y <= bulletPosition.y) ) {
        return true;
    }
    return false;
}

sf::Text setTextCharacteristics( sf::Text & textObject, int xPos, int yPos, int charSize ) {
    textObject.setPosition( xPos, yPos );
    textObject.setCharacterSize( charSize );
    textObject.setStyle(sf::Text::Bold);

    return textObject;
}

void checkEnemyShapeBoundaries( vector<EnemyShape> & enemyShapeVect, int i, sf::Vector2f enemyShapePosition ) {
    int Xpos = enemyShapeVect[i].attr.getPosition().x;
    int Ypos = enemyShapeVect[i].attr.getPosition().y;

    // Ensures enemy shapes stay in window
    if ( Xpos < 0 || Xpos > WIDTH - 50 ) enemyShapeVect[i].xVelocity *= -1; // 50 for Enemy Shape width
    if ( Ypos < 0 || Ypos > HEIGHT - 50 ) enemyShapeVect[i].yVelocity *= -1; // 50 for Enemy Shape width

    enemyShapePosition.x += enemyShapeVect[i].xVelocity;
    enemyShapePosition.y += enemyShapeVect[i].yVelocity;
    enemyShapeVect[i].attr.setPosition( enemyShapePosition );
}

Bullet checkBulletBoundaries(Bullet bullet) {
    int XBulletpos = bullet.attr.getPosition().x;
    int YBulletpos = bullet.attr.getPosition().y;

    // If bullet hits boundary of window, delete bullet
    if ( (XBulletpos < 0 || XBulletpos > WIDTH - 10) && bullet.isActive ) { // 10 for Enemy Shape width
        bullet.isActive = false;
    }
    if ( (YBulletpos < 0 || YBulletpos > HEIGHT - 10) && bullet.isActive ) { // 10 for Enemy Shape width
        bullet.isActive = false;
    }

    return bullet;
}



int main() {

    srand(time(nullptr));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Shape Destroyers");
    window.setFramerateLimit(60);

    // CREATE USER SHAPE
    sf::CircleShape player;
    player.setRadius(20); // 20 pixels
    player.setFillColor( sf::Color::Green );
    player.setOrigin(1, 1);
    player.setPosition(WIDTH / 2, HEIGHT / 1.2);
    float xOffset = 14.5;
    float yOffset = 15;

    // INITIALIZE VARIABLES
    bool keyPressed = false;
    bool gameOver = false;
    bool beginBuffer = true;
    bool outOfBullets = false;
    bool enemyShapeVectEmpty = false;
    int score = 0;
    vector<int> WASDvect = { 0, 0, 0, 0 };


    // CREATE ENEMY SHAPES AND ADD THEM TO A VECTOR
    vector<EnemyShape> enemyShapeVect;
    EnemyShape::createEnemyShapeVect( enemyShapeVect );

    // LOAD THE BULLET QUEUE WITH BULLETS
    queue<Bullet> bulletQueue;
    Bullet::loadBulletQueue( bulletQueue );

    // CREATE ACTIVE BULLET VECT
    vector<Bullet> activeBulletVect;


    // DECLARE AND LOAD FONT
    sf::Font font;
    if ( !font.loadFromFile("arial.ttf") ) {
        cout << "Couldn't Load Font" << endl;
        exit(1); // error...
    }

    // INITIATE AND SPECIALIZE TEXTS
    //     Score Text
    sf::Text scoreText("Score: " + to_string(score), font); // Cast type score int to a string --> to_string(score)
    scoreText.setFillColor( sf::Color::Green );

    //     Number of Bullets Text
    sf::Text numBullets(to_string(bulletQueue.size()), font); // Cast type bulletQueue.size() int to a string --> bulletQueue.size()
    scoreText.setFillColor( sf::Color::Green );

    //     You Win Text
    sf::Text youWin("YOU WIN!", font);
    scoreText.setFillColor( sf::Color::Green );

    //     OutOfBullets Text
    sf::Text OutOfBullets("Out Of Bullets :(", font);
    OutOfBullets.setFillColor( sf::Color::Cyan );

    //     Game Over Text
    sf::Text GameOver("GAME OVER", font);
    GameOver.setFillColor( sf::Color::Red );

    //     Begin Buffer Text
    int time = 3;
    sf::Text BeginBuffer(to_string(time), font); // Cast type time int to a string --> to_string(time)
    BeginBuffer.setFillColor( sf::Color::Blue );

    // EVENT LOOP
    while ( window.isOpen() ) {

        // CREATE EVENT FOR EVENT LOOP
        sf::Event event;
        while ( window.pollEvent( event ) ) {
            if ( event.type == sf::Event::Closed )
                window.close();
        }

        if ( enemyShapeVect.empty() ) {
            enemyShapeVectEmpty = true;
        }
        if (  bulletQueue.empty() &&  activeBulletVect.empty() ) {
            outOfBullets = true;
            gameOver = true;
        }

        // ACCESS THE USER'S POSITION
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
        if( sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !keyPressed ) {
            WASDvect[0] = 1;
        }
        else if( sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !keyPressed) {
            WASDvect[1] = 1;
        }
        else if( sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !keyPressed) {
            WASDvect[2] = 1;
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D) && !keyPressed) {
            WASDvect[3] = 1;
        }
        else{
            keyPressed = false;
        }

        // DETERMINE WHICH KEY WAS PRESSED (W, A, S, D) AND TAKE NECESSARY ACTION FOR THE BULLET
        if (event.type == sf::Event::KeyReleased) {
            if ( WASDvect[0] ) {  // Shoot bullet Upwards
                keyPressed = true;
                WASDvect[0] = 0;
                // Set bullet Object attributes
                bulletQueue.front().attr.setPosition( playerPosition.x + xOffset, playerPosition.y );
                bulletQueue.front().bulletUp = true;
            }
            else if ( WASDvect[1] ) { // Shoot bullet Left
                keyPressed = true;
                WASDvect[1] = 0;
                // Set bullet Object attributes
                bulletQueue.front().attr.setPosition( playerPosition.x, playerPosition.y + yOffset );
                bulletQueue.front().bulletLeft = true;
            }
            else if ( WASDvect[2] ) { // Shoot bullet Down
                keyPressed = true;
                WASDvect[2] = 0;
                // Set bullet Object attributes
                bulletQueue.front().attr.setPosition( playerPosition.x + xOffset, playerPosition.y + (2 * yOffset) );
                bulletQueue.front().bulletDown = true;
            }
            else if ( WASDvect[3] ) { // Shoot bullet Right
                keyPressed = true;
                WASDvect[3] = 0;
                // Set bullet Object attributes
                bulletQueue.front().attr.setPosition( playerPosition.x + (2 * xOffset), playerPosition.y + yOffset );
                bulletQueue.front().bulletRight = true;
            }
        }

        // ONLY MOVE BULLET OBJECT FROM QUEUE TO ACTIVE BULLET VECT IF THERE IS BULLETS LEFT IN THE QUEUE
        if (!bulletQueue.empty() && keyPressed) Bullet::fireBullet( bulletQueue, activeBulletVect );

        // MOVING EACH ACTIVE BULLET IN NECESSARY DIRECTION
        for (int j = 0; j < activeBulletVect.size(); j++) {
            if ( activeBulletVect[j].bulletUp ) {
                activeBulletVect[j] = Bullet::moveBullet( activeBulletVect[j], 0, -3, 1, 1 );
            }
            else if( activeBulletVect[j].bulletLeft ) {
                activeBulletVect[j] = Bullet::moveBullet( activeBulletVect[j], 3, 0, -1, 1 );
            }
            else if( activeBulletVect[j].bulletDown ) {
                activeBulletVect[j] = Bullet::moveBullet( activeBulletVect[j], 0, 3, 1, 1 );
            }
            else if( activeBulletVect[j].bulletRight ) {
                activeBulletVect[j] = Bullet::moveBullet( activeBulletVect[j], 3, 0, 1, 1 );
            }

            // Delete bullet if it goes out of the boundaries
            activeBulletVect[j] = checkBulletBoundaries( activeBulletVect[j] );
        }

        // GO THROUGH ALL ENEMY SHAPES TO MOVE THEM AND CHECK IF THEY HIT ANY OTHER OBJECTS
        for ( int i = 0; i < enemyShapeVect.size(); i++ ) {
            // Get position of Objects
            sf::Vector2f playerPosition = player.getPosition();
            sf::Vector2f enemyShapePosition = enemyShapeVect[i].attr.getPosition();

            // Check if position of enemy shape and user hit each other
            if ( EnemyShapeUserCollision(playerPosition, enemyShapePosition) ) {
                gameOver = true;
            }

            // Traverse through each active bullet for collisions with any objects
            for (int j = 0; j < activeBulletVect.size(); j++) {
                sf::Vector2f bulletPosition = activeBulletVect[j].attr.getPosition();

                // Check if position of enemy shape and a bullet hit each other
                if ( EnemyShapeBulletCollision(enemyShapePosition, bulletPosition) && activeBulletVect[j].isActive ) {
                    enemyShapeVect[i].isActive = false;
                    activeBulletVect[j].isActive = false;
                    score += 1;  // Increment Score
                }
            }

            // Check if Enemy Shape is within the Boundaries of the Window
            checkEnemyShapeBoundaries(enemyShapeVect, i, enemyShapePosition);
        }

        // ENSURE USER STAYS WITHIN WINDOW
        if ( (playerPosition.x < 0 || playerPosition.x > WIDTH - 40)
        ||  (playerPosition.y < 0 || playerPosition.y > HEIGHT - 40) ) gameOver = true; // 40 for User width

        // REMOVE NECESSARY OBJECTS
        //     Remove the Enemy Shape Objects that have been destroyed
        for ( int i = 0; i < enemyShapeVect.size(); i++ ) {
            if ( !enemyShapeVect[i].isActive ){
                swap( enemyShapeVect[i], enemyShapeVect.back() );
                enemyShapeVect.pop_back();
            }
        }

        // DELETE NON ACTIVE BULLETS IN BULLET VECT
        for (int i = 0; i < activeBulletVect.size(); i++ ) {
            if(!activeBulletVect[i].isActive){
                swap(activeBulletVect[i], activeBulletVect.back());
                activeBulletVect.pop_back();
            }
        }

        // UPDATE SCORE
        scoreText.setString("Score: " + to_string(score));

        // RENDER/DRAW OBJECTS
        //     Draw countdown screen at the beginning
        if ( beginBuffer ){
            for ( int i = 0; i < 4; i++ ) {
                window.clear();
                BeginBuffer = setTextCharacteristics(BeginBuffer, 640, 450, 100);
                window.draw(BeginBuffer);
                sleep( sf::seconds(1.0f) );
                window.display();
                time--;
                BeginBuffer.setString(to_string(time));
            }
            beginBuffer = false;
        }
        //     Draw the victory screen if the user wins
        else if ( enemyShapeVectEmpty ) {
            window.clear();
            youWin = setTextCharacteristics(youWin, 400, 450, 100);
            window.draw(youWin);
            scoreText = setTextCharacteristics(scoreText, 450, 600, 100);
            window.draw(scoreText);
            window.display();
        }
        //     Draw the Game over screen
        else if ( gameOver ) {
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
            for ( int i = 0; i < NUM_ENEMY_SHAPES; i++ ) {
                window.draw(enemyShapeVect[i].attr);
            }
            for (int j = 0; j < activeBulletVect.size(); j++) {
                if ( activeBulletVect[j].isActive ) {
                    window.draw(activeBulletVect[j].attr);
                }
            }
            window.display();
        }
    }

    return 0;
}
