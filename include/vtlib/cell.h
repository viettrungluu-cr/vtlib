#ifndef VTLIB_INCLUDE_VTLIB_CELL_H_
#define VTLIB_INCLUDE_VTLIB_CELL_H_

#include <vtlib/character.h>
#include <vtlib/color.h>

namespace vtlib {

//FIXME
class Cell {
 public:
//FIXME

 private:
  Character character_;
  Color fg_;
  Color bg_;
};

}  // namespace vtlib

#endif  // VTLIB_INCLUDE_VTLIB_CELL_H_
