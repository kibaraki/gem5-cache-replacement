#ifndef __MEM_CACHE_REPLACEMENT_POLICIES_LIN_RP_HH__
#define __MEM_CACHE_REPLACEMENT_POLICIES_LIN_RP_HH__

#include "mem/cache/replacement_policies/base.hh"

namespace gem5 {
namespace replacement_policy {

struct LinRPParams;     
typedef LinRPParams Params;

class LinRP : public Base
{
  protected:
    struct MyReplData : public ReplacementData {
        Tick lastTouchTick;
        Tick lastReuse;
        MyReplData() : lastTouchTick(Tick(0)), lastReuse(Tick(0)) {}
    };

  public:
    explicit LinRP(const Params &p);
    ~LinRP() override = default;

    void invalidate(
        const std::shared_ptr<ReplacementData>& replacement_data) override;
    void touch(
        const std::shared_ptr<ReplacementData>& replacement_data) const override;
    void reset(
        const std::shared_ptr<ReplacementData>& replacement_data) const override;
    ReplaceableEntry* getVictim(
        const ReplacementCandidates& candidates) const override;
    std::shared_ptr<ReplacementData> instantiateEntry() override;

  private:
    mutable double weight;
    mutable double bias;
    static constexpr double learningRate = 0.01;
};

} // namespace replacement_policy
} // namespace gem5

#endif // __MEM_CACHE_REPLACEMENT_POLICIES_LIN_RP_HH__


