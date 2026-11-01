/*
 * GeogramUtils.cpp
 */

#include "TetraMeshTools/GeogramUtils.h"

#include <geogram/basic/assert.h>
#include <geogram/basic/command_line.h>
#include <geogram/basic/command_line_args.h>
#include <geogram/basic/common.h>

#include <cstdlib>
#include <mutex>

void TetraTools::InitGeogram() {
  static std::once_flag once;
  std::call_once(once, []() {
    // read by GEO::initialize()
#ifdef _WIN32
    _putenv_s("GEO_NO_SIGNAL_HANDLER", "1");
#else
    setenv("GEO_NO_SIGNAL_HANDLER", "1", 1);
#endif
    GEO::initialize(GEO::GEOGRAM_INSTALL_NONE);
    GEO::set_assert_mode(GEO::ASSERT_THROW);
    GEO::CmdLine::import_arg_group("standard");
    GEO::CmdLine::import_arg_group("algo");
    GEO::CmdLine::import_arg_group("remesh");
    GEO::CmdLine::import_arg_group("pre");
    GEO::CmdLine::import_arg_group("post");
  });
}
