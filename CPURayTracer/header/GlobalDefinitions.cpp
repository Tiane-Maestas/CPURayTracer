#include "RenderingPipelineConfig.h"

// RenderingStatistics
std::atomic<uint64_t> RenderingStatistics::RaysCasted = 0;
std::atomic<uint64_t> RenderingStatistics::NumTriTests = 0;
std::atomic<uint64_t> RenderingStatistics::NumSphTests = 0;
std::atomic<uint64_t> RenderingStatistics::NumElpTests = 0;

// ProgressReport
std::atomic<uint64_t> ProgressReport::TotalBlocks = 1;
std::atomic<uint64_t> ProgressReport::RenderedBlocks = 0;