//
// Created by xiaozhuai on 2021/12/29.
//

#include "Signals.h"

#include <utility>
#include <vector>

class SignalHandler {
public:
    SignalHandler(std::vector<int> _signals, std::function<void()> _handler)
            : signals(std::move(_signals)),
              handler(std::move(_handler)) {}

    std::vector<int> signals;
    std::function<void()> handler;
};

static std::vector<SignalHandler> signalHandlers;

static void onSignal(int sig) {
    for (const auto &handler: signalHandlers) {
        if (std::find(handler.signals.begin(), handler.signals.end(), sig) != handler.signals.end()
            && handler.handler) {
            handler.handler();
        }
    }
}

namespace Signals {

void catchSome(std::initializer_list<int> sigs, const std::function<void()> &handlerFunc) {
    signalHandlers.emplace_back(SignalHandler(sigs, handlerFunc));
    for (auto sig: sigs) {
        signal(sig, onSignal);
    }
}

}
