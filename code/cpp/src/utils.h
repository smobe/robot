#pragma once

namespace rf {


inline uint64_t microSinceEpoc(){
    // Get time stamp in microseconds.
    return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::
                  now().time_since_epoch()).count();
}

struct FrameHeader{

    uint32_t height, width, type;
    uint64_t frame_timestamp;
    uint64_t currenct_clock;
};

} // namespace rf
