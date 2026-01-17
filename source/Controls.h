#pragma once
#ifndef INFS_FEEDBACKSYNTHCONTROLS_H
#define INFS_FEEDBACKSYNTHCONTROLS_H

#include <daisy.h>
#include <daisy_seed.h>
#include "Engine.h"
#include "ParameterRegistry.h"
#include "CVRegistry.h"

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
    cv_.Process();
  }

private:
  // Identifies a parameter of the synth engine
  // The order here is the same order as the ADC pin configs in the cpp file
  static const size_t kNumAdcChannels = 1;
  enum class Parameter : uint8_t {
    // OutputVolume moved to CV
  };

  // CV inputs (audio-rate, no smoothing)
  enum class CV : uint8_t {
    OutputVolume  // 1
  };

  using Parameters = ParameterRegistry<Parameter>;
  using CVs = CVRegistry<CV>;

  Parameters params_;
  CVs cv_;

  // daisy::Switch del_sw_;

  void initADCs(daisy::DaisySeed& hw);
  void registerParams(Engine& engine);
  void registerCVs(Engine& engine);
};

}  // namespace SimpleRack

#endif
