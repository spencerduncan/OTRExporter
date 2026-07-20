#pragma once

#include <map>
#include <vector>
#include <ship/resource/Resource.h>
#include <fast/resource/ResourceType.h>
#include "ExporterVariant.h"

// Per-variant namespace: see ExporterVariant.h (Fault A, #396).
namespace OTREXP_NS {
extern std::map<uint32_t, uint32_t> resourceVersions;
} // namespace OTREXP_NS
