/*
 * GeogramUtils.h
 *
 * Shared setup of the geogram library (used by the surface repair and by
 * fTetWild).
 */

#ifndef TETRAMESHTOOLS_GEOGRAMUTILS_H_
#define TETRAMESHTOOLS_GEOGRAMUTILS_H_

#include "TetraToolsExports.h"

namespace TetraTools {

/// Initializes geogram once (thread safe): no signal handlers, assertions
/// throw std::runtime_error instead of aborting, the command line options
/// used by its algorithms are declared.
DLL_EXPORT void InitGeogram();

}  // namespace TetraTools

#endif /* TETRAMESHTOOLS_GEOGRAMUTILS_H_ */
