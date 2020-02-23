#include <Delivery.hpp>
#include <stdexcept>

sa::Delivery::Delivery(const DeliveryDirection _direction,
                       const float _speed,
                       const sf::Color _color,
                       const ptrdiff_t _target)
  :
  direction{ _direction },
  speed{ _speed },
  color{ _color },
  target{ _target },
  progress{ MIN_PROGRESS },
  has{ true }
{
}

void sa::Delivery::process(const float _dt)
{
  progress += speed * _dt;
  if (progress > MAX_PROGRESS)
  {
    progress = MAX_PROGRESS;
  }
}

sa::DeliveryDirection sa::Delivery::getDirection() const
{
  return direction;
}

sf::Color sa::Delivery::getColor() const
{
  return color;
}

ptrdiff_t sa::Delivery::getTarget() const
{
  return target;
}

float sa::Delivery::getProgress() const
{
  return progress;
}

bool sa::Delivery::hasBox() const
{
  return has;
}

void sa::Delivery::throwBox()
{
  if (has == true)
  {
    has = false;
  } else {
    throw std::invalid_argument("sa::Delivery::throwBox(), has == false.");
  }
}