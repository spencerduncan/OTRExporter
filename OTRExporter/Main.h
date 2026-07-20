#pragma once

#include <libultraship/bridge.h>
#include "ExporterArchive.h"
#include "ExporterVariant.h"

// These live in a per-variant namespace so the OoT and MM exporter libraries do
// not share one storage slot when both are linked into redship. See
// ExporterVariant.h for the full rationale (Fault A, #396).
namespace OTREXP_NS {
extern std::shared_ptr<ExporterArchive> archive;
extern std::map<std::string, std::vector<char>> files;
} // namespace OTREXP_NS

void AddFile(std::string fName, std::vector<char> data);
