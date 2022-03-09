#include <iostream>                             // for standard input/output
#include <windows.h>
using namespace std;                            // using the standard namespace

#include <SFML/Graphics.hpp>                    // include the SFML Graphics Library
#include <SFML/Audio.hpp>
#include <cmath>

using namespace sf;                             // using the sf namespace

void makeLines(float scale, RenderWindow &window, vector<RectangleShape> &lines) {
    lines.clear();

    for (float i = 0; i <= window.getSize().x; i+=scale) {
        float height = 0;

        if (i/(100*scale) == round(i/(100*scale))) {
            height = window.getSize().y/2.0;
        } else if (i/(50*scale) == round(i/(50*scale))) {
            height = window.getSize().y/4.0 * 1.5;
        } else if (i/(5*scale) == round(i/(5*scale))) {
            height = window.getSize().y/4.0;
        }

        if (height != 0) {
            lines.emplace_back(Vector2f(2, height));
            lines[lines.size() - 1].setFillColor(Color::Black);
            lines[lines.size() - 1].setPosition(i - 1, 0);
        }
    }
}

void setMeasurement(Text &measurement, Text &unit, float scale, vector<RectangleShape> &lines, RenderWindow &window) {
    measurement.setString(to_string((int) round(window.getSize().x / scale)));

    if (measurement.getGlobalBounds().width + 10 + 7 + unit.getGlobalBounds().width < window.getSize().x/2) {
        measurement.setOrigin(measurement.getGlobalBounds().width / 2.0, measurement.getGlobalBounds().height / 2.0);
        measurement.setPosition(window.getSize().x / 2.0, 3 * window.getSize().y / 4.0);
        unit.setPosition(measurement.getPosition().x + measurement.getGlobalBounds().width / 2.0 + 7, measurement.getPosition().y + measurement.getGlobalBounds().height / 2.0 - unit.getGlobalBounds().height);
    } else {
        measurement.setOrigin(0, measurement.getGlobalBounds().height / 2.0);
        measurement.setPosition(5, 3 * window.getSize().y / 4.0);
        unit.setPosition(measurement.getPosition().x + measurement.getGlobalBounds().width + 7, measurement.getPosition().y + measurement.getGlobalBounds().height / 2.0 - unit.getGlobalBounds().height);
    }

    makeLines(scale, window, lines);
}

int main() {
    ShowWindow(FindWindowA("ConsoleWindowClass", NULL), false);

    // create a RenderWindow object
    RenderWindow window( VideoMode(500, 64), "Pixel Ruler", Style::None);

    bool dragging = false;
    bool sizing = false;

    float scale = 1;

    //********************************************
    //  PLACE ANY FILE INPUT PROCESSING BELOW HERE
    //********************************************
    RectangleShape dragBox(Vector2f(8, 8));
    dragBox.setPosition(window.getSize().x - dragBox.getSize().x - 1, window.getSize().y - dragBox.getSize().y - 1);
    dragBox.setOutlineColor(Color::Black);
    dragBox.setOutlineThickness(1);
    RectangleShape sizeBox(Vector2f(8, 8));
    sizeBox.setPosition(window.getSize().x - dragBox.getSize().x - 1, window.getSize().y - 2 * dragBox.getSize().y - 1);
    sizeBox.setOutlineColor(Color::Black);
    sizeBox.setOutlineThickness(1);

    Font arial;
    arial.loadFromFile("./data/arial.ttf");
    Text measurement(to_string(window.getSize().x), arial);
    measurement.setFillColor(Color::Black);
    measurement.setCharacterSize(16);
    measurement.setOrigin(measurement.getGlobalBounds().width/2.0, measurement.getGlobalBounds().height/2.0);
    measurement.setPosition(window.getSize().x/2.0, 3*window.getSize().y/4.0);
    Text unit("px", arial);
    unit.setFillColor(Color::Black);
    unit.setCharacterSize(12);
    unit.setPosition(measurement.getPosition().x + measurement.getGlobalBounds().width/2.0 + 7, measurement.getPosition().y + measurement.getGlobalBounds().height/2.0 - unit.getGlobalBounds().height);

    vector<RectangleShape> lines;
    makeLines(scale, window, lines);

    Cursor arrow;
    arrow.loadFromSystem(Cursor::Arrow);
    Cursor stretch;
    stretch.loadFromSystem(Cursor::SizeHorizontal);
    Cursor reposition;
    reposition.loadFromSystem(Cursor::SizeAll);
    //********************************************
    //  PLACE ANY FILE INPUT PROCESSING ABOVE HERE
    //********************************************

    // while our window is open, keep it open
    // this is our draw loop
    while( window.isOpen() ) {
        window.clear( Color(186, 140, 99) );           // clear the contents of the old frame
                                                // by setting the window to black

        //****************************************
        //  ADD ALL OF OUR DRAWING BELOW HERE
        //****************************************
        if (dragging) {
            window.setMouseCursor(reposition);
            window.setPosition(Vector2i(sf::Mouse::getPosition().x - dragBox.getPosition().x - dragBox.getSize().x/2, sf::Mouse::getPosition().y - dragBox.getPosition().y - dragBox.getSize().y/2));
        } else if (sizing) {
            window.setMouseCursor(stretch);
            window.setSize(Vector2<unsigned int>(sf::Mouse::getPosition().x - window.getPosition().x + sizeBox.getSize().x/2, window.getSize().y));
            sf::FloatRect view(0, 0, window.getSize().x, window.getSize().y);
            window.setView(View(view));
            dragBox.setPosition(window.getSize().x - dragBox.getSize().x - 1, window.getSize().y - dragBox.getSize().y - 1);
            sizeBox.setPosition(window.getSize().x - dragBox.getSize().x - 1, window.getSize().y - 2 * dragBox.getSize().y - 1);

            setMeasurement(measurement, unit, scale, lines, window);
        } else {
            sf::Vector2f mousePosF(static_cast<float>( sf::Mouse::getPosition(window).x ), static_cast<float>( sf::Mouse::getPosition(window).y ));
            if (dragBox.getGlobalBounds().contains(mousePosF)) {
                window.setMouseCursor(reposition);
            } else if (sizeBox.getGlobalBounds().contains(mousePosF)) {
                window.setMouseCursor(stretch);
            } else {
                window.setMouseCursor(arrow);
            }
        }

        window.draw(dragBox);
        window.draw(sizeBox);
        window.draw(measurement);
        window.draw(unit);

        for (auto l : lines) {
            window.draw(l);
        }
        //****************************************
        //  ADD ALL OF OUR DRAWING ABOVE HERE
        //****************************************
        window.display();                       // display the window

        //****************************************
        // HANDLE EVENTS BELOW HERE
        //****************************************
        Event event;
        while( window.pollEvent(event) ) {      // ask the window if any events occurred
            if( event.type == Event::Closed ) { // if event type is a closed event
                // i.e. they clicked the X on the window
                window.close();                 // then close our window
            } else if (event.type == Event::MouseButtonPressed) {
                sf::Vector2f mousePosF(static_cast<float>( sf::Mouse::getPosition(window).x ), static_cast<float>( sf::Mouse::getPosition(window).y ));
                if (dragBox.getGlobalBounds().contains(mousePosF)) {
                    dragging = true;
                } else if (sizeBox.getGlobalBounds().contains(mousePosF)) {
                    sizing = true;
                }
            } else if (event.type == Event::MouseButtonReleased) {
                dragging = false;
                sizing = false;
            } else if (event.type == Event::MouseWheelScrolled) {
                if (Mouse::getPosition(window).x > 0 && Mouse::getPosition(window).y > 0 && Mouse::getPosition(window).x <= window.getSize().x && Mouse::getPosition(window).y <= window.getSize().y) {
                    if (event.mouseWheelScroll.delta > 0) {
                        scale += 0.5;
                    } else if (scale > 0.5) {
                        scale -= 0.5;
                    }

                    if (scale != 1) {
                        string unitLabel = to_string(scale);
                        if (round(scale) == scale) {
                            unitLabel = unitLabel.substr(0,unitLabel.find('.'));
                        } else {
                            unitLabel = unitLabel.substr(0,unitLabel.find('0', unitLabel.find('.')));
                        }

                        unit.setString("px\tx" + unitLabel);
                    } else
                        unit.setString("px");

                    setMeasurement(measurement, unit, scale, lines, window);
                }
            }
        }
    }

    return EXIT_SUCCESS;                        // report our program exited successfully
}