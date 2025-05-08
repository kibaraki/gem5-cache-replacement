#include "mem/cache/replacement_policies/lin_rp.hh"
#include "params/LinRP.hh"
//#include "mem/cache/tags/cache_tags.hh"
#include "sim/sim_object.hh"

#include <algorithm>
#include <cassert>
#include <limits>

namespace gem5 {
namespace replacement_policy {

LinRP::LinRP(const Params &p)
    : Base(p),
      weight(0.0),
      bias(0.0)
{}

void
LinRP::invalidate(const std::shared_ptr<ReplacementData>& replacement_data)
{
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    data->lastTouchTick = Tick(0);
    data->lastReuse      = Tick(0);
}

void
LinRP::touch(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    Tick now       = curTick();
    Tick current   = now - data->lastTouchTick;
    Tick previous  = data->lastReuse;

    // Predict next reuse based on previous
    double pred = weight * double(previous) + bias;
    double err  = double(current) - pred;
    weight     += learningRate * err * double(previous);
    bias       += learningRate * err;

    // Update history
    data->lastReuse      = current;
    data->lastTouchTick  = now;
}

void
LinRP::reset(const std::shared_ptr<ReplacementData>& replacement_data) const
{
    auto data = std::static_pointer_cast<MyReplData>(replacement_data);
    Tick now = curTick();
    data->lastTouchTick = now;
    data->lastReuse     = Tick(0);
}

ReplaceableEntry*
LinRP::getVictim(const ReplacementCandidates& candidates) const
{
    assert(!candidates.empty());
    ReplaceableEntry* victim = candidates[0];
    double worstPred = -std::numeric_limits<double>::infinity();
    Tick now = curTick();

    for (auto &cand : candidates) {
        auto data = std::static_pointer_cast<MyReplData>(cand->replacementData);
        Tick dist = now - data->lastTouchTick;
        double pred = weight * double(data->lastReuse) + bias;
        if (pred > worstPred) {
            worstPred = pred;
            victim   = cand;
        }
    }
    return victim;
}

std::shared_ptr<ReplacementData>
LinRP::instantiateEntry()
{
    return std::make_shared<MyReplData>();
}

} // namespace replacement_policy
} // namespace gem5

