#include <Stepper.hpp>
#include <stdexcept>

sa::Stepper::Stepper(const ptrdiff_t _x,
                     const ptrdiff_t _y,
                     const float _speed)
  :
  source_x{ _x },
  source_y{ _y },
  destination_x{ _x },
  destination_y{ _y },
  progress{ 1.f },
  speed{ _speed }
{
}

void sa::Stepper::updateProgress(const float _dt)
{
  progress += speed * _dt;
  if (progress > 1.f)
  {
    progress = 1.f;
  }
}

bool sa::Stepper::isReadyToStep() const
{
  return (progress == 1.f);
}

void sa::Stepper::stepToLeft()
{
  if (isReadyToStep() == false)
  {
    throw std::logic_error("sa::Stepper::stepToLeft(), isReadyToStep() == false.");
  }

  source_x = destination_x;
  source_y = destination_y;

  if (destination_x > PTRDIFF_MIN)
  {
    --destination_x;
  }

  progress = 0.f;
}

void sa::Stepper::stepToRight()
{
  if (isReadyToStep() == false)
  {
    throw std::logic_error("sa::Stepper::stepToRight(), isReadyToStep() == false.");
  }

  source_x = destination_x;
  source_y = destination_y;

  if (destination_x < PTRDIFF_MAX)
  {
    ++destination_x;
  }

  progress = 0.f;
}

void sa::Stepper::stepToUp()
{
  if (isReadyToStep() == false)
  {
    throw std::logic_error("sa::Stepper::stepToUp(), isReadyToStep() == false.");
  }

  source_x = destination_x;
  source_y = destination_y;

  if (destination_y > PTRDIFF_MIN)
  {
    --destination_y;
  }

  progress = 0.f;
}

void sa::Stepper::stepToDown()
{
  if (isReadyToStep() == false)
  {
    throw std::logic_error("sa::Stepper::stepToDown(), isReadyToStep() == false.");
  }

  source_x = destination_x;
  source_y = destination_y;

  if (destination_y < PTRDIFF_MAX)
  {
    ++destination_y;
  }

  progress = 0.f;
}

void sa::Stepper::fix()
{
  if (isReadyToStep() == false)
  {
    throw std::logic_error("sa::Stepper::fix(), isReadyToStep() == false.");
  }

  source_x = destination_x;
  source_y = destination_y;
}

ptrdiff_t sa::Stepper::getSourceX() const
{
  return source_x;
}

ptrdiff_t sa::Stepper::getSourceY() const
{
  return source_y;
}

ptrdiff_t sa::Stepper::getDestinationX() const
{
  return destination_x;
}

ptrdiff_t sa::Stepper::getDestinationY() const
{
  return destination_y;
}

float sa::Stepper::getRepresentedX() const
{
  if (source_x < destination_x)
  {
    return source_x + progress;
  }
  if (source_x > destination_x)
  {
    return source_x - progress;
  }
  return static_cast<float>(destination_x);
}

float sa::Stepper::getRepresentedY() const
{
  if (source_y < destination_y)
  {
    return source_y + progress;
  }
  if (source_y > destination_y)
  {
    return source_y - progress;
  }
  return static_cast<float>(destination_y);
}
