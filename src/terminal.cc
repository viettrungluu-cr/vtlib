#include <vtlib/terminal.h>

#include "src/terminal_impl.h"

namespace vtlib {

// static
std::unique_ptr<Terminal> Terminal::Create(const Options& options) {
  // TODO(C++14): No make_unique in C++11.
  return std::unique_ptr<Terminal>(new TerminalImpl(options));
}

}  // namespace vtlib
