#pragma once
#ifndef INFS_FEEDBACKSYNTHCONTROLS_H
#define INFS_FEEDBACKSYNTHCONTROLS_H

#include <daisy.h>
#include <daisy_seed.h>
#include "Engine.h"
#include "ParameterRegistry.h"

namespace SimpleRack {

class Controls {
public:
  Controls() = default;
  ~Controls() = default;

  void Init(daisy::DaisySeed& hw, Engine& engine);

  void UpdateParameter(daisy::DaisySeed& hw);
  void UpdateCv(daisy::DaisySeed& hw);

  void Process() {
    params_.Process();
  }

private:
  // Identifies a parameter of the synth engine
  // The order here is the same order as the ADC pin configs in the cpp file
  static const size_t kNumAdcChannels = 7;
  enum class Parameter : uint8_t {
    Structure,      // 0
    Brightness,     // 1
    Damping,        // 2
    Position,       // 3
    OutputVolume,
    Note,
    Strum
  };

  using Parameters = ParameterRegistry<Parameter>;

  Parameters params_;

  daisy::Switch strum_button_;
  bool prev_strum_cv_gate_ = false;

  void initADCs(daisy::DaisySeed& hw);
  void registerParams(Engine& engine);
};

}  // namespace SimpleRack

#endif
