#include <SFML/Graphics.hpp>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <iostream>
#include <queue>
#include <cstdlib>

using namespace std;

const int WIDTH = 1280;
const int HEIGHT = 1000;
const int NUM_ENEMY_SHAPES = 5;
const int NUM_BULLETS = 2;

class GameObject {
public:
    void move_object(){

    }
};

class EnemyShape: public GameObject {
public:
    sf::RectangleShape attr;
    float xEnemyShapeVelocity;
    float yEnemyShapeVelocity;
    bool destroyed = false;
    int enemyShapeNum = 0;

    static EnemyShape create_EnemyShape(){
        sf::Vector2f rectanglePosition(rand() % WIDTH, rand() % HEIGHT);

        EnemyShape enemyShape;
        enemyShape.attr.setPosition(rectanglePosition);
        enemyShape.attr.setSize(sf::Vector2f(50, 50));
        enemyShape.attr.setFillColor(sf::Color::Yellow);

        // Determines speed for x & y axis

//        cout << IntArray[randIndex] << endl;
        int IntArray [2] = {1, -1};
        int XrandIndex = rand() % 2;
        int YrandIndex = rand() % 2;

        enemyShape.xEnemyShapeVelocity = IntArray[XrandIndex];
        enemyShape.yEnemyShapeVelocity = IntArray[YrandIndex];

        return enemyShape;
    }

    static vector<EnemyShape> createEnemyShapeVect(vector<EnemyShape> & enemyShapeVect){
        for (int i = 0; i < NUM_ENEMY_SHAPES; i++){
            EnemyShape enemy = EnemyShape::create_EnemyShape();
            enemy.enemyShapeNum = i + 1;
            enemyShapeVect.push_back(enemy);
        }

        return enemyShapeVect;
    }

    void move_object() {

    }
};

class Bullet: public GameObject {
public:
    sf::RectangleShape attr;
    float xBulletVelocity;
    float yBulletVelocity;
    bool isActive = false;

    static Bullet create_Bullet() {
        Bullet bullet;
        bullet.attr.setSize(sf::Vector2f(10, 10));
        bullet.attr.setFillColor(sf::Color::Red);
//        cout << "Printing player position X: " << player.getPosition().x << " & y: " <<  player.getPosition().y << endl;
//        bullet.attr.setOrigin(1,1); // TODO: Fix bullet origin so its not on side of the screen

        cout << "Bullet Created" << endl;
        return bullet; // Returning an object (bullet)
    }

    static queue<Bullet> createBulletQueue(queue<Bullet> & bullet_queue) {
        for (int i = 0; i < NUM_BULLETS; i++){
            Bullet bullet = Bullet::create_Bullet();
            bullet_queue.push(bullet);
        }

        return bullet_queue;
    }

    static void fireBullet(queue<Bullet> & queue, int x, int y, int p, int n){
        queue.front().xBulletVelocity = x;
        queue.front().yBulletVelocity = y;
        sf::Vector2f bullet_pos = queue.front().attr.getPosition();

        queue.front().xBulletVelocity *= p;
        queue.front().xBulletVelocity *= n;

        bullet_pos.x += queue.front().xBulletVelocity;
        bullet_pos.y += queue.front().yBulletVelocity;
        queue.front().attr.setPosition(bullet_pos);
    }
};

bool EnemyShapeUserCollision(sf::Vector2f player_position, sf::Vector2f rect_position){
    if ((player_position.x - 40 <= rect_position.x && rect_position.x < player_position.x + 40) && (player_position.y - 40 <= rect_position.y && rect_position.y < player_position.y + 40)){
        return true;
    }
    return false;
}

bool EnemyShapeBulletCollision( sf::Vector2f enemyShapePosition,  sf::Vector2f bulletPosition){
    if ((bulletPosition.x <= enemyShapePosition.x + 50 && enemyShapePosition.x <= bulletPosition.x) && (bulletPosition.y <= enemyShapePosition.y + 50 && enemyShapePosition.y <= bulletPosition.y)){
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
    // Makes sure enemy squares stay in screen
    if (Xpos < 0 || Xpos > WIDTH - 50) enemyShapeVect[i].xEnemyShapeVelocity *= -1; // 50 for Enemy Shape width
    if (Ypos < 0 || Ypos > HEIGHT - 50) enemyShapeVect[i].yEnemyShapeVelocity *= -1; // 50 for Enemy Shape width

    enemyShapePosition.x += enemyShapeVect[i].xEnemyShapeVelocity;
    enemyShapePosition.y += enemyShapeVect[i].yEnemyShapeVelocity;
    enemyShapeVect[i].attr.setPosition(enemyShapePosition);
}

bool checkBulletBoundaries(queue<Bullet> & bullet_queue, bool PopBullet){
    int XBulletpos = bullet_queue.front().attr.getPosition().x;
    int YBulletpos = bullet_queue.front().attr.getPosition().y;

    // If bullet hits boundary of window, delete bullet
    if ((XBulletpos < 0 || XBulletpos > WIDTH - 10) && bullet_queue.front().isActive) { // 10 for Enemy Shape width
        PopBullet = true;
        bullet_queue.front().isActive = false;
    }
    if ((YBulletpos < 0 || YBulletpos > HEIGHT - 10) && bullet_queue.front().isActive) { // 10 for Enemy Shape width
        PopBullet = true;
        bullet_queue.front().isActive = false;
    }

    return PopBullet;
}

int main() {

    srand(time(NULL));

    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGHT), "Shape Destroyers");
    window.setFramerateLimit(60);

    sf::CircleShape player;
    player.setRadius(20);
    player.setFillColor(sf::Color::Green);
    player.setOrigin(1, 1);
    player.setPosition(WIDTH / 2, HEIGHT / 2);



    // Initialize Variables
    bool W = false;
    bool A = false;
    bool S = false;
    bool D = false;
    bool gameOver = false;
    bool beginBuffer = true;
    bool PopBullet = false;
    bool PopEnemyShape = false;
    bool enemyShapeBulletCollision = false;
    bool bulletFired = false;
    bool outOfBullets = false;
    int score = 0;

    // Declare and load a font
    sf::Font font;
    if (!font.loadFromFile("arial.ttf")) exit(1); // error...

    // INITIATE AND SPECIALIZE TEXTS
    //     Score Text
    sf::Text scoreText("Score: " + to_string(score), font); // Cast type score int to a string --> to_string(score)
    scoreText.setFillColor(sf::Color::Green);

    //     OutOfBullets Text
    sf::Text OutOfBullets("Out Of Bullets :(", font); // Cast type score int to a string --> to_string(score)
    OutOfBullets.setFillColor(sf::Color::Cyan);

    //     Game Over Text
    sf::Text GameOver("GAME OVER", font); // Cast type score int to a string --> to_string(score)
    GameOver.setFillColor(sf::Color::Red);

    //     Begin Buffer Text
    int time = 3;
    sf::Text BeginBuffer(to_string(time), font); // Cast type score int to a string --> to_string(score)
    BeginBuffer.setFillColor(sf::Color::Blue);


    // Creates Enemy Shapes and adds them to enemyShapeVect by calling helper function
    vector<EnemyShape> enemyShapeVect;
    EnemyShape::createEnemyShapeVect(enemyShapeVect);

    // Creates bullets and adds them to the bulletVect
//    vector<Bullet> bulletVect;
//    for (int i = 0; i < NUM_BULLETS; i++){
//        Bullet bullet = Bullet::create_Bullet();
//        bulletVect.push_back(bullet);
//    }
    // Create bullet_queue
    queue<Bullet> bullet_queue;
    Bullet::createBulletQueue(bullet_queue);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        if (enemyShapeVect.empty()){
            cout << "enemyShapeVect is empty" << endl;
        }
        if (bullet_queue.empty()){
            cout << "bullet_queue is empty" << endl;
            outOfBullets = true;

        }


        // Get position of enemy shape and user
        sf::Vector2f playerPosition = player.getPosition();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) // Press Escape to Close Window
            window.close();
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
            player.move(-5.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
            player.move(5.0f, 0.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
            player.move(0.0f, -5.0f);
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
            player.move(0.0f, 5.0f);
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W)){ // Shoot bullet Upwards
            W = true;
            S = false;
            A = false;
            D = false;
            bullet_queue.front().isActive = true;

            bullet_queue.front().attr.setPosition(playerPosition.x + 14.5, playerPosition.y);
//            cout << "printing player position in W place: " << playerPosition.x << " & " << playerPosition.y << endl;
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::A)){ // Shoot bullet Left
            A = true;
            S = false;
            W = false;
            D = false;
            bullet_queue.front().isActive = true;
            bullet_queue.front().attr.setPosition(playerPosition.x, playerPosition.y + 15);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::S)){ // Shoot bullet Down
            S = true;
            W = false;
            A = false;
            D = false;
            bullet_queue.front().isActive = true;
            bullet_queue.front().attr.setPosition(playerPosition.x + 14.5, playerPosition.y + 29);
        }
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)){ // Shoot bullet Right
            D = true;
            W = false;
            S = false;
            A = false;
            bullet_queue.front().isActive = true;
            bullet_queue.front().attr.setPosition(playerPosition.x + 29, playerPosition.y + 15);
        }

        if (W) Bullet::fireBullet(bullet_queue, 0, -3, 1, 1);
        if (S) Bullet::fireBullet(bullet_queue, 0, 3, 1, 1);
        if (A) Bullet::fireBullet(bullet_queue, 3, 0, -1, 1);
        if (D) Bullet::fireBullet(bullet_queue, 3, 0, 1, 1);
        if ( W || S || A || D ) bulletFired = true;


        for(int i = 0; i < enemyShapeVect.size(); i++){
//            cout << "Printing enemyShapeVect.size(): " << enemyShapeVect.size() << endl;
            // Get position of enemy shape and user
            sf::Vector2f playerPosition = player.getPosition();
            sf::Vector2f enemyShapePosition = enemyShapeVect[i].attr.getPosition();
            sf::Vector2f bulletPosition = bullet_queue.front().attr.getPosition();

            // Check if enemy shape and user hit each other by being in each others buffer of range for x & y
            if (EnemyShapeUserCollision(playerPosition, enemyShapePosition)){
                gameOver = true;
            }

            if (EnemyShapeBulletCollision(enemyShapePosition, bulletPosition) & bullet_queue.front().isActive){
                cout << "Enemy Shape Collision...enemy object # = " << enemyShapeVect[i].enemyShapeNum << endl;
                PopBullet = true;
                PopEnemyShape = true;
                enemyShapeVect[i].destroyed = true;

                score += 1;
            }

            // Check Boundaries of Objects in Window
            // Physics (Moving Enemy Squares) - Go through all squares
            checkEnemyShapeBoundaries(enemyShapeVect, i, enemyShapePosition);
//            int Xpos = enemyShapeVect[i].attr.getPosition().x;
//            int Ypos = enemyShapeVect[i].attr.getPosition().y;
//                // Makes sure enemy squares stay in screen
//            if (Xpos < 0 || Xpos > WIDTH - 50) enemyShapeVect[i].xEnemyShapeVelocity *= -1; // 50 for Enemy Shape width
//            if (Ypos < 0 || Ypos > HEIGHT - 50) enemyShapeVect[i].yEnemyShapeVelocity *= -1; // 50 for Enemy Shape width
//
//            enemyShapePosition.x += enemyShapeVect[i].xEnemyShapeVelocity;
//            enemyShapePosition.y += enemyShapeVect[i].yEnemyShapeVelocity;
//            enemyShapeVect[i].attr.setPosition(enemyShapePosition);
        }

        // Check Bullet Boundaries
        PopBullet = checkBulletBoundaries(bullet_queue, PopBullet);

        // Check if user hits boundary of window
        if (playerPosition.x < 0 || playerPosition.x > WIDTH - 40) gameOver = true; // 10 for Enemy Shape width
        if (playerPosition.y < 0 || playerPosition.y > HEIGHT - 40) gameOver = true; // 10 for Enemy Shape width

        // REMOVE OBJECTS
        //     Remove the Enemy Shape Objects that have been destroyed
        for (int i = 0; i < enemyShapeVect.size(); i++){
            if (enemyShapeVect[i].destroyed){
                swap(enemyShapeVect[i], enemyShapeVect.back());
                cout << "Enemy Shape about to be Deleted:...enemy object # = " << enemyShapeVect.back().enemyShapeNum << endl;
                enemyShapeVect.pop_back();
                PopEnemyShape = false;
            }
        }

        //     Remove bullets that have been destroyed
        if(PopBullet){
            PopBullet = false;
            bullet_queue.pop();
        }

        // Pop Enemy Shape off enemyShapeVect if needed
//        if(PopEnemyShape && enemyShapeVect.back().destroyed){
//            PopEnemyShape = false;
//            cout << "Enemy Shape about to be Deleted:...enemy object # = " << enemyShapeVect.back().enemyShapeNum << endl;
////            cout << "Enemy Shape about to be Deleted: " << enemyShapeVect.back().attr.getPosition().x << " & y " << enemyShapeVect.back().attr.getPosition().y << endl;
//            enemyShapeVect.pop_back();
//            cout << "Enemy Shape Deleted" << endl;
//            cout << "Enemy Shape Vect back() popped..size is: " << enemyShapeVect.size() << endl;
//        }

        // Update Score if Shape gets hit
        scoreText.setString("Score: " + to_string(score));

        // Render Objects
        if (beginBuffer){
            for(int i = 0; i < 4; i++){
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
        else if(outOfBullets){
            window.clear();
            GameOver = setTextCharacteristics(GameOver, WIDTH / 3.8, HEIGHT / 2.5, 100);
            window.draw(GameOver);
            OutOfBullets = setTextCharacteristics(OutOfBullets, 300, 550, 100);
            window.draw(OutOfBullets);
            scoreText = setTextCharacteristics(scoreText, 450, 700, 100);
            window.draw(scoreText);
            window.display();

        }
        else if (!gameOver){
            window.clear();
            scoreText = setTextCharacteristics(scoreText, 50, 25, 50);
            window.draw(scoreText);
            window.draw(player);
            for (int i = 0; i < NUM_ENEMY_SHAPES; i++){
                window.draw(enemyShapeVect[i].attr);
            }
            if (bullet_queue.front().isActive){
                window.draw(bullet_queue.front().attr);
            }
            window.display();
        }
        else {
            window.clear();
            GameOver = setTextCharacteristics(GameOver, WIDTH / 3.8, HEIGHT / 2.5, 100);
            window.draw(GameOver);
            scoreText = setTextCharacteristics(scoreText, 450, 600, 100);
            window.draw(scoreText);
            window.display();
        }

    }

    return 0;
}


// TODO:
//  - sometimes enemy shapes move horizontally across side of screen
//      - I think it's bc rand sometimes is 0
//  - Clean up text to one variable
//  - Make GRBIT like Pete said
//  - Fix all fixed numbers and put them as variables
//  - Add more shapes when all shapes have been hit --> add a time limit of how long the user is playing


// - //  - fix the correct shape deleting - sometimes it has multiple shapes deleting as well


// Press W, add bullet to queue - loop that updates bullet, checks which bullet is dead
// Add alive bullets to new vector
// While vector of bullets is not empty
// Loop through vector of bullets to check if bullet is dead
