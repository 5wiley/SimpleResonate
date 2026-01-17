#pragma once
#ifndef INFS_CV_REGISTRY_H
#define INFS_CV_REGISTRY_H

#include <functional>
#include <unordered_map>
#include <daisysp.h>

namespace SimpleRack {

/**
 * @brief
 * CV input registry for audio-rate processing without smoothing.
 * Designed for fast-changing CV signals processed once per audio callback.
 *
 * @tparam CVId Template parameter specifying the CV input ID type
 */
template <typename CVId>
class CVRegistry {
public:
  using Handler = std::function<void(const float)>;

  CVRegistry() {};
  ~CVRegistry() {};

  /**
   * @brief Register a CV input with scaling and curve mapping
   *
   * @param id Unique identifier for this CV input
   * @param initial_value Initial value (within min-max range)
   * @param min Minimum output value (when CV is 0.0)
   * @param max Maximum output value (when CV is 1.0)
   * @param handler Function to call with the processed CV value
   * @param mapping Curve mapping (LINEAR, EXPONENTIAL, LOGARITHMIC)
   */
  void Register(const CVId id,
                const float initial_value,
                const float min,
                const float max,
                Handler handler,
                const daisysp::Mapping mapping = daisysp::Mapping::LINEAR) {
    CVState state(initial_value, min, max, mapping, handler);
    cv_states_.insert({id, state});
  }

  /**
   * @brief Update a CV input with a new value in the actual range
   * No smoothing is applied - value is used directly on next Process()
   *
   * @param id CV input identifier
   * @param value Value in the actual min-max range
   */
  void Update(const CVId id, const float value) {
    auto it = cv_states_.find(id);
    if (it != cv_states_.end()) {
      auto& state = it->second;
      state.value = daisysp::fclamp(value, state.min, state.max);
    }
  }

  /**
   * @brief Update a CV input with a normalized value (0.0 to 1.0)
   * Applies mapping curve and scales to output range
   *
   * @param id CV input identifier
   * @param normValue Normalized input value (0.0 to 1.0)
   */
  void UpdateNormalized(const CVId id, const float normValue) {
    auto it = cv_states_.find(id);
    if (it != cv_states_.end()) {
      auto& state = it->second;
      state.value = daisysp::fmap(daisysp::fclamp(normValue, 0.0f, 1.0f),
                                  state.min, state.max, state.mapping);
    }
  }

  /**
   * @brief Process all CV inputs and call their handlers
   * Should be called once per audio callback
   */
  void Process() {
    for (auto& cv : cv_states_) {
      auto& state = cv.second;
      state.handler(state.value);
    }
  }

  /**
   * @brief Get the current value for a CV input
   *
   * @param id CV input identifier
   * @return float Current value in min-max range
   */
  float GetValue(const CVId id) const {
    auto it = cv_states_.find(id);
    if (it != cv_states_.end()) {
      return it->second.value;
    }
    return 0.0f;
  }

private:
  struct CVState {
    float value;  // Current value in the min-max range

    const float min;
    const float max;
    const daisysp::Mapping mapping;
    const Handler handler;

    CVState() = delete;
    CVState(const float initial,
            const float min,
            const float max,
            const daisysp::Mapping mapping,
            const Handler handler)
        : value(initial),
          min(min),
          max(max),
          mapping(mapping),
          handler(handler) {}
  };

  // TODO: Consider using libDaisy objects and avoiding STL containers
  // for better real-time performance
  using CVStates = std::unordered_map<CVId, CVState>;

  CVStates cv_states_;
};

}  // namespace SimpleRack

#endif
