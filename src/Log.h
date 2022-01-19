//
// Created by xiaozhuai on 2022/1/19.
//

#ifndef SOFT_RENDERER_LOG_H
#define SOFT_RENDERER_LOG_H

#include <string>
#include <unordered_map>
#include <chrono>

namespace __log__ {
inline int64_t now() {
    return std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::system_clock::now().time_since_epoch()
    ).count();
}

static std::unordered_map<std::string, int64_t> start_time_map;
}

#define LOG(format, ...) do { printf(format "\n", ##__VA_ARGS__); } while(0)
#define LOGP_BEG(tag) do { __log__::start_time_map[tag] = __log__::now(); } while(0)
#define LOGP_END(tag) do { if (__log__::start_time_map.find(tag) != __log__::start_time_map.end()) { LOG("%s spend %lldμs", tag, __log__::now() - __log__::start_time_map[tag]); } } while(0)

#endif //SOFT_RENDERER_LOG_H
