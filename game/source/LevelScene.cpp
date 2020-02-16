#include <LevelScene.hpp>

sa::LevelScene::LevelScene(sa::ResourceLibrary& _resource_library)
  :
  Scene{ _resource_library },
  field{ 12u, 9u },
  player{ 5u, 5u }
{
}

void sa::LevelScene::react(const sf::Event& _event)
{
  // For debug.
  if (_event.type == sf::Event::EventType::KeyPressed)
  {
    if (_event.key.code == sf::Keyboard::Space)
    {
      srand(clock());
      sf::Color color;
      switch (rand() % 4)
      {
        case (0):
        {
          color = sf::Color::Red;
          break;
        }
        case (1):
        {
          color = sf::Color::Blue;
          break;
        }
        case (2):
        {
          color = sf::Color::Yellow;
          break;
        }
        case (3):
        {
          color = sf::Color::Green;
          break;
        }
      }
      Box box(rand() % field.getWidth(), 0, color);
      boxes.push_front(box);
    }
  }
}

void sa::LevelScene::process(const float _dt)
{ 
  processBoxes(_dt);
  processPlayer(_dt);
}

void sa::LevelScene::draw(const sa::Drawer& _drawer) const
{
  drawBoxes(_drawer);
  drawPlayer(_drawer);
}

sa::Scene::Uptr sa::LevelScene::getNextScene()
{
  return nullptr;
}

void sa::LevelScene::processBoxes(const float _dt)
{
  for (auto& box : boxes)
  {
    box.process(_dt*2);
    boxTriesToStepToDown(box);
  }
  analyseBottomLine();
  analyseSectors();
  deleteMarkedBoxes();
}

void sa::LevelScene::boxTriesToStepToDown(Box& _box)
{
  if (_box.isReadyToStep() == true)
  {
    field.getCell(_box.getSourceX(), _box.getSourceY()).setOccupingBox(nullptr);

    _box.fix();

    field.getCell(_box.getSourceX(), _box.getSourceY()).setOccupingBox(&_box);

    const size_t new_x = _box.getSourceX();
    const size_t new_y = _box.getSourceY() + 1u;

    if (field.isCellValid(new_x, new_y) == true)
    {
      if (field.getCell(new_x, new_y).getOccupyingBox() == nullptr)
      {
        _box.stepToDown();
      }
    }
    field.getCell(_box.getDestinationX(), _box.getDestinationY()).setOccupingBox(&_box);
  }
}

void sa::LevelScene::analyseBottomLine()
{
  bool bottom_line_is_filled = true;

  const size_t y = field.getHeight() - 1u;

  for (size_t x = 0u; x < field.getWidth(); ++x)
  {
    const auto& cell = field.getCell(x, y);
    if ((cell.getOccupyingBox() == nullptr) || (cell.getOccupyingBox()->isReadyToStep() == false))
    {
      bottom_line_is_filled = false;
      break;
    }
  }

  if (bottom_line_is_filled == true)
  {
    std::list<Box*> marked_boxes;
    for (size_t x = 0u; x < field.getWidth(); ++x)
    {
      field.getCell(x, y).getOccupyingBox()->toMark();
    }
  }
}

void sa::LevelScene::analyseSectors()
{
  for (size_t x = 0u; x < field.getWidth(); ++x)
  {
    for (size_t y = 0u; y < field.getHeight(); ++y)
    {
      Box* const central_box = field.getCell(x, y).getOccupyingBox();
      if ((central_box != nullptr) && (central_box->isReadyToStep() == true))
      {
        std::list<Box*> selected_boxes;
        selected_boxes.push_front(central_box);

        auto analyse_cell = [&](const size_t _x, const size_t _y)
        {
          if (field.isCellValid(_x, _y) == true)
          {
            Box* const box = field.getCell(_x, _y).getOccupyingBox();
            if (box != nullptr)
            {
              if (box->isReadyToStep() == true)
              {
                if (box->getColor() == central_box->getColor())
                {
                  selected_boxes.push_back(box);
                }
              }
            }
          }
        };

        analyse_cell(x - 1u, y);
        analyse_cell(x + 1u, y);
        analyse_cell(x, y - 1u);
        analyse_cell(x, y + 1u);

        if (selected_boxes.size() >= 3u)
        {
          for (auto selected_box : selected_boxes)
          {
            selected_box->toMark();
          }
        }
      }
    }
  }
}

void sa::LevelScene::deleteMarkedBoxes()
{
  for (auto iterator = boxes.begin(); iterator != boxes.end();)
  {
    if (iterator->isMarked() == true)
    {
      field.getCell(iterator->getSourceX(), iterator->getSourceY()).setOccupingBox(nullptr);
      field.getCell(iterator->getDestinationX(), iterator->getDestinationY()).setOccupingBox(nullptr);
      iterator = boxes.erase(iterator);
    } else {
      ++iterator;
    }
  }
}

void sa::LevelScene::drawBoxes(const Drawer& _drawer) const
{
  sf::Texture texture = resource_library.getTexture("Box.bmp");

  sf::Sprite sprite;
  sprite.setTexture(texture, true);

  //const float x_scale = 64.f / static_cast<float>(texture.getSize().x);
  //const float y_scale = 64.f / static_cast<float>(texture.getSize().y);

  //sprite.setScale(x_scale, y_scale);

  for (const auto& box : boxes)
  {
    sprite.setColor(box.getColor());
    sprite.setPosition(box.getRepresentedX() * 64.f, box.getRepresentedY() * 64.f);

    _drawer.draw(sprite);
  }
}

void sa::LevelScene::processPlayer(const float _dt)
{
  if (player.isReadyToStep() == true)
  {
    player.fix();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) == true)
    {
      if (playerTriesToStepToLeft() == true)
      {
        return;
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) == true)
    {
      if (playerTriesToStepToRight() == true)
      {
        return;
      }
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) == true)
    {
      if (playerTriesToStepToUp() == true)
      {
        return;
      }
    }

    if (playerTriesToStepToDown() == true)
    {
      return;
    }
  }
  player.process(_dt*2);
}

bool sa::LevelScene::playerTriesToStepToLeft()
{
  const size_t x = player.getSourceX();
  const size_t y = player.getSourceY();

  bool player_stands_onto_something = false;

  if (field.isCellValid(x, y + 1u) == true)
  {
    if (field.getCell(x, y + 1u).getOccupyingBox() != nullptr)
    {
      player_stands_onto_something = true;
    }
  } else {
    player_stands_onto_something = true;
  }

  if ((field.isCellValid(x - 1u, y) == true) && (field.isCellValid(x - 1u, y - 1u) == true))
  {
    if (player_stands_onto_something == true)
    {
      if (field.getCell(x - 1u, y - 1u).getOccupyingBox() == nullptr)
      {
        if (field.getCell(x - 1u, y).getOccupyingBox() == nullptr)
        {
          player.stepToLeft();
          return true;
        } else {
          if (field.isCellValid(x - 2u, y) == true)
          {
            if (field.getCell(x - 2u, y).getOccupyingBox() == nullptr)
            {
              Box* const box = field.getCell(x - 1u, y).getOccupyingBox();
              if (box->isReadyToStep() == true)
              {
                box->stepToLeft();
                field.getCell(box->getDestinationX(), box->getDestinationY()).setOccupingBox(box);
                player.stepToLeft();
                return true;
              }
            }
          }
        }
      }
    } else {
      if (field.isCellValid(x - 1u, y + 1u) == true)
      {
        if (field.getCell(x - 1u, y + 1u).getOccupyingBox() != nullptr)
        {
          if (field.getCell(x - 1u, y - 1u).getOccupyingBox() == nullptr)
          {
            if (field.getCell(x - 1u, y).getOccupyingBox() == nullptr)
            {
              player.stepToLeft();
              return true;
            } else {
              if (field.isCellValid(x - 2u, y) == true)
              {
                if (field.getCell(x - 2u, y).getOccupyingBox() == nullptr)
                {
                  Box* const box = field.getCell(x - 1u, y).getOccupyingBox();
                  if (box->isReadyToStep() == true)
                  {
                    box->stepToLeft();
                    field.getCell(box->getDestinationX(), box->getDestinationY()).setOccupingBox(box);
                    player.stepToLeft();
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

bool sa::LevelScene::playerTriesToStepToRight()
{
  const size_t x = player.getSourceX();
  const size_t y = player.getSourceY();

  bool player_stands_onto_something = false;

  if (field.isCellValid(x, y + 1u) == true)
  {
    if (field.getCell(x, y + 1u).getOccupyingBox() != nullptr)
    {
      player_stands_onto_something = true;
    }
  } else {
    player_stands_onto_something = true;
  }

  if ((field.isCellValid(x + 1u, y) == true) && (field.isCellValid(x + 1u, y - 1u) == true))
  {
    if (player_stands_onto_something == true)
    {
      if (field.getCell(x + 1u, y - 1u).getOccupyingBox() == nullptr)
      {
        if (field.getCell(x + 1u, y).getOccupyingBox() == nullptr)
        {
          player.stepToRight();
          return true;
        } else {
          if (field.isCellValid(x + 2u, y) == true)
          {
            if (field.getCell(x + 2u, y).getOccupyingBox() == nullptr)
            {
              Box* const box = field.getCell(x + 1u, y).getOccupyingBox();
              if (box->isReadyToStep() == true)
              {
                box->stepToRight();
                field.getCell(box->getDestinationX(), box->getDestinationY()).setOccupingBox(box);
                player.stepToRight();
                return true;
              }
            }
          }
        }
      }
    } else {
      if (field.isCellValid(x + 1u, y + 1u) == true)
      {
        if (field.getCell(x + 1u, y + 1u).getOccupyingBox() != nullptr)
        {
          if (field.getCell(x + 1u, y - 1u).getOccupyingBox() == nullptr)
          {
            if (field.getCell(x + 1u, y).getOccupyingBox() == nullptr)
            {
              player.stepToRight();
              return true;
            } else {
              if (field.isCellValid(x + 2u, y) == true)
              {
                if (field.getCell(x + 2u, y).getOccupyingBox() == nullptr)
                {
                  Box* const box = field.getCell(x + 1u, y).getOccupyingBox();
                  if (box->isReadyToStep() == true)
                  {
                    box->stepToRight();
                    field.getCell(box->getDestinationX(), box->getDestinationY()).setOccupingBox(box);
                    player.stepToRight();
                    return true;
                  }
                }
              }
            }
          }
        }
      }
    }
  }
  return false;
}

bool sa::LevelScene::playerTriesToStepToUp()
{
  const size_t x = player.getSourceX();
  const size_t y = player.getSourceY();

  bool player_stands_onto_something = false;

  if (field.isCellValid(x, y + 1u) == true)
  {
    if (field.getCell(x, y + 1u).getOccupyingBox() != nullptr)
    {
      player_stands_onto_something = true;
    }
  } else {
    player_stands_onto_something = true;
  }

  if (player_stands_onto_something == true)
  {
    if ((field.isCellValid(x, y) == true) && (field.isCellValid(x, y - 1u) == true))
    {
      if ((field.getCell(x, y).getOccupyingBox() == nullptr) && (field.getCell(x, y - 1u).getOccupyingBox() == nullptr))
      {
        player.stepToUp();
        return true;
      }
    }
  }

  return false;
}

bool sa::LevelScene::playerTriesToStepToDown()
{
  const size_t x = player.getSourceX();
  const size_t y = player.getSourceY();

  if (field.isCellValid(x, y + 1u) == true)
  {
    if (field.getCell(x, y + 1u).getOccupyingBox() == nullptr)
    {
      player.stepToDown();
      return true;
    }
  }
  return false;
}

void sa::LevelScene::drawPlayer(const Drawer& _drawer) const
{
  sf::Texture texture = resource_library.getTexture("Unknown.bmp");

  sf::Sprite sprite;
  sprite.setTexture(texture, true);
  sprite.setScale(16, 16);

  sprite.setPosition(player.getRepresentedX() * 64.f, player.getRepresentedY() * 64.f);
  _drawer.draw(sprite);
  sprite.setPosition(player.getRepresentedX() * 64.f, (player.getRepresentedY() - 1) * 64.f);
  _drawer.draw(sprite);
}
