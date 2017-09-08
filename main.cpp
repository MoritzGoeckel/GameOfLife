//sudo apt-get install libsfml-dev
//g++ *.cpp -o exec -lsfml-graphics -lsfml-window -lsfml-system
//./exec

#include <iostream>
#include <memory>
#include <SFML/Graphics.hpp>
#include <string>

class FieldItem
{
  public:
    int state = 0;
    int nextState = 0;
    std::unique_ptr<sf::RectangleShape> shape;
    std::unique_ptr<sf::Text> text;
    FieldItem(int number, std::unique_ptr<sf::RectangleShape> shape);
    void setState(int newState);
    bool needsDrawing();
    void setNextState(int next);
    void draw(sf::RenderWindow& window);
    void setComment(std::string comment);
    static std::unique_ptr<sf::Font> font;
    void transitionState();
};

std::unique_ptr<sf::Font> FieldItem::font = nullptr;

void FieldItem::setComment(std::string comment){
  if(this->text == nullptr){
    auto text = std::make_unique<sf::Text>(comment, *font);
    text->setCharacterSize(15);
    text->setFillColor(sf::Color::White);
    text->setOutlineColor(sf::Color::Black);
    text->setOutlineThickness(0.5);
    text->setPosition(this->shape->getPosition());
    this->text = std::move(text);
  }
  else
    this->text->setString(comment);
}

void FieldItem::draw(sf::RenderWindow& window){
  window.draw(*this->shape);

  if(this->text != nullptr)
    window.draw(*this->text);
}

void FieldItem::transitionState(){
  if(this->state != this->nextState)
    this->setState(this->nextState);
}

FieldItem::FieldItem(int state, std::unique_ptr<sf::RectangleShape> shape){
  this->shape = std::move(shape);
  this->setState(state);
}

void FieldItem::setState(int newState){
  this->state = newState;
  this->nextState = newState;

  if(this->state == 1)
    this->shape->setFillColor(sf::Color::Green);
  else
    this->shape->setFillColor(sf::Color::Black);
}

void FieldItem::setNextState(int next){
  this->nextState = next;
}

bool inline FieldItem::needsDrawing(){
  return true; //this->state != 0
}

int main()
{
  FieldItem::font = std::make_unique<sf::Font>();
  FieldItem::font->loadFromFile("VeraMono.ttf");

  const int blockSize = 30;
  const int xSize = 30, ySize = 30;
  std::array<std::array<std::unique_ptr<FieldItem>, xSize>, ySize> field;

  sf::RenderWindow window(sf::VideoMode(xSize * blockSize, ySize * blockSize),
   "Game of Life");

   for (int x = 0; x < field.size(); x++)
    for (int y = 0; y < field[x].size(); y++){
      auto rectangle = std::make_unique<sf::RectangleShape>(sf::Vector2f(blockSize, blockSize));
      rectangle->setPosition(blockSize * x, blockSize * y);
      field[x][y] = std::make_unique<FieldItem>(0, std::move(rectangle));
    }

  //Some patterns

  field[0][0]->setState(1);
  field[0][1]->setState(1);
  field[1][0]->setState(1);
  field[1][1]->setState(1);

  field[3][5]->setState(1);
  field[3][6]->setState(1);
  field[4][5]->setState(1);
  field[4][6]->setState(1);

  /*field[3 + 2][5 + 2]->setState(1);
  field[3 + 2][6 + 2]->setState(1);
  field[4 + 2][5 + 2]->setState(1);
  field[4 + 2][6 + 2]->setState(1);*/

  field[10][10]->setState(1);
  field[10][9]->setState(1);
  field[10][8]->setState(1);
  field[11][8]->setState(1);
  field[8][9]->setState(1);

  field[20][20]->setState(1);
  field[21][20]->setState(1);
  field[19][20]->setState(1);

  //Game Loop
  while (window.isOpen())
  {
    sf::Event event;
    while (window.pollEvent(event))
    {
      if (event.type == sf::Event::Closed)
        window.close();
    }

    //Tick
    //Analyse and set next states
    for (int x = 0; x < field.size(); x++)
     for (int y = 0; y < field[x].size(); y++){
       int aliveNeighbors = 0;
       for(int xOffset = -1; xOffset <= 1; xOffset++)
        for(int yOffset = -1; yOffset <= 1; yOffset++)
          if(xOffset != 0 || yOffset != 0)
            aliveNeighbors += (field[(x + xOffset) % field.size()][(y + yOffset) % field[x].size()]->state == 1 ? 1 : 0);
        field[x][y]->setComment(std::string(" ") + std::to_string(aliveNeighbors));

        if(aliveNeighbors < 2 || aliveNeighbors > 3)
          field[x][y]->setNextState(0);
        else if(aliveNeighbors == 3)
          field[x][y]->setNextState(1);
      }

    //Transit into next states
    for (int x = 0; x < field.size(); x++)
     for (int y = 0; y < field[x].size(); y++)
      field[x][y]->transitionState();

    //Draw
    window.clear();
    int drawCount = 0;

    for (int x = 0; x < field.size(); x++)
     for (int y = 0; y < field[x].size(); y++){
       if(field[x][y]->needsDrawing()){
         field[x][y]->draw(window);
         drawCount++;
       }
      }

    //std::cout << drawCount << " Tiles drawn" << std::endl;
    window.display();
  }

  return 0;
}
