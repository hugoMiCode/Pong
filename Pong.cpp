#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define _USE_MATH_DEFINES

void setup();

#pragma region variables
sf::RenderWindow window;
int scoreA{ 0 }, scoreB{ 0 }, position_initiale{ 100 };
sf::RectangleShape joueur1(sf::Vector2f(10, 50)), joueur2(sf::Vector2f(10, 50)), boundray(sf::Vector2f(3, 15));
double angle{ 0 }, vitesse_partie{ 1.2 };
double const acc{ 0.3 }, PI{ 3.141592 }, vitesse_max_puck{ 4.5 };
bool jeu{ false };
sf::Clock clock_move, clock_ball, contact1, contact2;
#pragma endregion variables

enum class Direction{ UNDEFINE = 0, UP, DOWN };
Direction dir1, dir2;

class Puck
{
public:
    Puck(double radus) {
        this->radus = radus;
        reset();
    }

    void boundray() {
        if (this->pos.y < 0) {
            this->dir.y = -this->dir.y;
        }
        else if (this->pos.y > window.getSize().y - this->radus * 2) {
            this->dir.y = -this->dir.y;
        }

        if (this->pos.x < 0) {
            scoreB++;
            reset();
            std::cout << "score : " << scoreA << "  " << scoreB << std::endl;
        }
        if (this->pos.x > window.getSize().x) {
            scoreA++;
            reset();
            std::cout << "score : " << scoreA << "  " << scoreB << std::endl;
        }
    }
    void palette() {
        if (this->pos.x < joueur1.getPosition().x + joueur1.getSize().x && 
            this->pos.x > joueur1.getPosition().x &&
            this->pos.y >= joueur1.getPosition().y - radus &&
            this->pos.y <= joueur1.getPosition().y + (double)joueur1.getSize().y - radus &&
            contact1.getElapsedTime().asMilliseconds() > 200)
        {
            contact1.restart();
            if (this->vitesse < vitesse_max_puck) {
                this->vitesse += acc;
                vitesse_partie += acc;
            }
            else {
                this->vitesse = vitesse_max_puck;
            }
            this->dir = direction_post_collision_joueur1();
        }
        
        if (this->pos.x + radus > joueur2.getPosition().x &&
            this->pos.x < joueur2.getPosition().x + joueur2.getSize().x &&
            this->pos.y >= joueur2.getPosition().y - radus &&
            this->pos.y <= joueur2.getPosition().y + (double)joueur1.getSize().y - radus && 
            contact2.getElapsedTime().asMilliseconds() > 200)
        {
            contact2.restart();
            if (this->vitesse < vitesse_max_puck) {
                this->vitesse += acc;
                vitesse_partie += acc;
            }
            else {
                this->vitesse = vitesse_max_puck;
            }
            
            this->dir = direction_post_collision_joueur2();
        }
    }
    sf::Vector2f direction_post_collision_joueur1() {
        double dist = (double)joueur1.getSize().y + (double)joueur1.getPosition().y - this->pos.y + this->radus;
        double angle{ ((dist * 90) / joueur1.getSize().y) + 315 };

        angle *= PI / 180;
        return sf::Vector2f(cos(angle), -sin(angle));
    }   
    sf::Vector2f direction_post_collision_joueur2() {
        double dist = (double)joueur2.getSize().y + (double)joueur2.getPosition().y - this->pos.y + this->radus;
        double angle{ ((dist * 90) / joueur2.getSize().y) + 135 };

        angle *= PI / 180;
        return sf::Vector2f(cos(angle), sin(angle));
    }
    void show() {
        sf::RectangleShape puck(sf::Vector2f(radus * 2, radus * 2));
        sf::CircleShape ball(this->radus);
        puck.setPosition(sf::Vector2f(pos.x - this->radus / 2, pos.y - this->radus / 2));
        ball.setPosition(sf::Vector2f(pos.x - this->radus, pos.y - this->radus));
        window.draw(ball);
    }
    void move() {
        if (clock_ball.getElapsedTime().asMilliseconds() > 5)
        {
            this->pos = sf::Vector2f(this->pos.x + this->dir.x * this->vitesse, this->pos.y + this->dir.y * this->vitesse);
            boundray();
            palette();
            clock_ball.restart();
        }
    }
    void reset() {
        sf::Clock clock_reset;
        sf::Font reset_font;
        if (!reset_font.loadFromFile("SIXTY.ttf")) {
            std::cout << "Erreur de chargemenr de la police SIXTY." << std::endl;
        }
        sf::Text timer;
        timer.setFont(reset_font);
        timer.setCharacterSize(100);
        timer.setPosition(window.getSize().x / 2 - 50, window.getSize().y / 2 - 50);

        while (clock_reset.getElapsedTime().asMilliseconds() < 2000) {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                    window.close();
            }
            window.clear(sf::Color(30, 144, 55));
            if (clock_reset.getElapsedTime().asMilliseconds() < 1000) {
                timer.setString("2");
                window.draw(timer);
                window.display();
            } else if (clock_reset.getElapsedTime().asMilliseconds() < 2000) {
                timer.setString("1");
                window.draw(timer);
                window.display();
            }
        }
        clock_reset.restart();


        this->pos = sf::Vector2f(window.getSize().x / 2, window.getSize().y / 2);
        if (angle < 90) {
            angle += 315;
        }
        else if (angle > 90) {
            angle += 45;
        }

        angle *= PI / 180;
        this->dir = sf::Vector2f(cos(angle), sin(angle));
        this->vitesse = 1.5;
        vitesse_partie = 1.5;
        setup();
    }
    
private:
    double radus, vitesse;
    sf::Vector2f pos, dir;
};

#pragma region prototypes
void gestion_clavier();
void logic(Puck&);
void show(Puck&);
#pragma endregion prototypes

int main()
{
    std::cout << "Appuyez sur ESPACE quand vous etes pret." << std::endl;
    while (!sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {

        angle = rand() % (0, 180);
    }
    window.create(sf::VideoMode(1080, 720), "Pong");
    setup();
    Puck ball(5);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        { if (event.type == sf::Event::Closed)
                window.close();
        }
        angle = rand() % (0, 180);
        window.clear(sf::Color(30, 144, 55));

        gestion_clavier();
        logic(ball);
        show(ball);
        window.display();
        //window.setSize(sf::Vector2u(1080, 720));
    }

    return 0;
}

void logic(Puck& ball)
{
    if (clock_move.getElapsedTime().asMilliseconds() > 4) {
        if (dir1 == Direction::UP) {
            joueur1.move(0, -vitesse_partie);
        }
        else if (dir1 == Direction::DOWN) {
            joueur1.move(0, vitesse_partie);
        }

        if (dir2 == Direction::UP) {
            joueur2.move(0, -vitesse_partie);
        }
        else if (dir2 == Direction::DOWN) {
            joueur2.move(0, vitesse_partie);
        }
        clock_move.restart();
    }

    if (joueur1.getPosition().y < 2) {
        joueur1.setPosition(joueur1.getPosition().x, 2);
    } else if (joueur1.getPosition().y > window.getSize().y - 2 - joueur1.getSize().y) {
        joueur1.setPosition(joueur1.getPosition().x, window.getSize().y - 2 - joueur1.getSize().y);
    }

    if (joueur2.getPosition().y < 2) {
        joueur2.setPosition(joueur2.getPosition().x, 2);
    } else if (joueur2.getPosition().y > window.getSize().y - 2 - joueur2.getSize().y) {
        joueur2.setPosition(joueur2.getPosition().x, window.getSize().y - 2 - joueur2.getSize().y);
    }

    ball.move();
}

void gestion_clavier()
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Z)) {
        dir1 = Direction::UP;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        dir1 = Direction::DOWN;
    }
    else {
        dir1 = Direction::UNDEFINE;
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        dir2 = Direction::UP;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        dir2 = Direction::DOWN;
    }
    else {
        dir2 = Direction::UNDEFINE;
    }
}

void setup()
{
    joueur1.setPosition(position_initiale, window.getSize().y / 2 - joueur1.getSize().y / 2);
    joueur2.setPosition(window.getSize().x - position_initiale, window.getSize().y / 2 - joueur2.getSize().y / 2);

    
    dir1 = Direction::UNDEFINE ;
    dir2 = Direction::UNDEFINE ;
}

void show(Puck& ball)
{
    sf::Font font_score;
    if (!font_score.loadFromFile("SIXTY.ttf")) {
        std::cout << "Erreur de chargement de SIXTY." << std::endl;
    }
    sf::Text score1;
    score1.setFont(font_score);
    score1.setString(std::to_string(scoreA));
    score1.setCharacterSize(50);
    score1.setFillColor(sf::Color::White);
    score1.setPosition(300, 50);
    sf::Text score2;
    score2.setFont(font_score);
    score2.setString(std::to_string(scoreB));
    score2.setCharacterSize(50);
    score2.setFillColor(sf::Color::White);
    score2.setPosition(window.getSize().x - 300, 50);

    window.draw(score1);
    window.draw(score2);
    window.draw(joueur1);
    window.draw(joueur2);
    ball.show();
    

    for (unsigned int i{ 0 }; i < window.getSize().y; i += 25) {
        boundray.setPosition(window.getSize().x / 2 - 2, i);
        window.draw(boundray);
    }
}
