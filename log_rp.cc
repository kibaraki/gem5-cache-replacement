#include "mem/cache/replacement_policies/log_rp.hh"
#include "params/LogRP.hh"
//#include "mem/cache/tags/cache_tags.hh"
#include "sim/sim_object.hh"

#include <algorithm>
#include <cassert>
#include <limits>
#include <cmath>  // for std::exp()

namespace gem5 {
namespace replacement_policy {

LogRP::LogRP(const Params &p)
    : Base(p),
      weight(0.01),
      bias(0.0)
{}

void
LogRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data) {
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    data->lastTouchTick = Tick(0);
    data->lastReuse     = Tick(0);
}

void
LogRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const {
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    Tick now      = curTick();
    Tick current  = now - data->lastTouchTick;
    Tick previous = data->lastReuse;

    // NEW
    bool label = (current <= reuseThreshold);


    // Logistic regression prediction
    double linear = weight * double(previous) + bias;
    double pred   = 1.0 / (1.0 + std::exp(-linear));
    
    // NEW double grad   = pred * (1.0 - pred);

    // Compute error between actual reuse interval and prediction
    // NEW double err  = double(current) - pred;
    double err = double(label) - pred;

    // Gradient descent update
    weight += learningRate * err * double(previous); // removed grad
    bias   += learningRate * err; // * grad;

    // Update history
    data->lastReuse     = current;
    data->lastTouchTick = now;
}

void
LogRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const {
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    Tick now = curTick();
    data->lastTouchTick = now;
    data->lastReuse     = Tick(0);
}

ReplaceableEntry*
LogRP::getVictim(const ReplacementCandidates& candidates) const {
    assert(!candidates.empty());
    ReplaceableEntry* victim   = candidates[0];

    // also change back to -infinity if doesn't work
    double worstPred           = std::numeric_limits<double>::infinity();
    Tick now                   = curTick();

    for (auto &cand : candidates) {
        auto data = std::static_pointer_cast<MyReplData>(cand->replacementData);
        // Logistic regression prediction
        double linear = weight * double(data->lastReuse) + bias;
        double pred   = 1.0 / (1.0 + std::exp(-linear));

        // testing if should be <, if not, change back to pred > worstPred
        if (pred < worstPred) {
            worstPred = pred;
            victim   = cand;
        }
    }
    return victim;
}

std::shared_ptr<ReplacementData>
LogRP::instantiateEntry() {
    return std::make_shared<MyReplData>();
}

} // namespace replacement_policy
} // namespace gem5

