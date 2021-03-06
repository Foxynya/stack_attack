#pragma once

#include <memory>
#include <Cell.hpp>

namespace sa
{
  class Field
  {
  public:

    Field(const size_t _width, const size_t _height);

    bool isCellValid(const size_t _x, const size_t _y) const;

    Cell& getCell(const size_t _x, const size_t _y);
    
    void clear();

    size_t getWidth() const;
    size_t getHeight() const;

  private:

    const size_t width;
    const size_t height;
    
    std::unique_ptr<std::unique_ptr<Cell[]>[]> cells;

  };
}
