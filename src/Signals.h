//
// Created by xiaozhuai on 2021/12/29.
//

#ifndef SOFT_RENDERER_SIGNALS_H
#define SOFT_RENDERER_SIGNALS_H

#include <functional>
#include <initializer_list>
#include <csignal>

namespace Signals {

void catchSome(std::initializer_list<int> sigs, const std::function<void()> &handler);

}

#endif //SOFT_RENDERER_SIGNALS_H
