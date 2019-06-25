#ifndef _MYCANVAS_
#define _MYCANVAS_
#include "qsfmlcanvas.hpp"
#include <iostream>
#include "graphics.hpp"

class MyCanvas : public QSFMLCanvas
{
    Q_OBJECT
public :

    MyCanvas(QWidget* Parent, const QPoint& Position, const QSize& Size) :
        QSFMLCanvas(Parent, Position, Size),
        myShapes(144, std::vector<sf::RectangleShape>(160))
    {
    }

    virtual ~MyCanvas(){}

    void renderScreen(std::vector<std::vector<RGB>> screen) {

        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                sf::Color color(screen[y][x]._red,
                                screen[y][x]._green,
                                screen[y][x]._blue);
                myShapes[y][x].setPosition(x, y);
                myShapes[y][x].setFillColor(color);
            }
        }
        repaint();
    }

private :

    void OnInit()
    {
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                myShapes[y][x].setPosition(x, y);
                myShapes[y][x].setSize({1, 1});
                myShapes[y][x].setFillColor(sf::Color(0, 0, 128));
            }
        }
    }

    void OnUpdate()
    {
        // Clear screen
        clear(sf::Color(0, 128, 0));

        // Draw it
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                draw(myShapes[y][x]);
            }
        }
    }

    sf::Image myImage;
    sf::Sprite mySprite;
    sf::Texture myTexture;
    std::vector<std::vector<sf::RectangleShape>> myShapes;
    const int tileSize = 1;
};

#endif
