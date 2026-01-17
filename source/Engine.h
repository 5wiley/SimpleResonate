#pragma once
#ifndef IFS_FEEDBACK_SYNTH_ENGINE_H
#define IFS_FEEDBACK_SYNTH_ENGINE_H

#include <memory>
#include <daisysp.h>
// #include "BiquadFilters.h"
// #include "EchoDelay.h"
// #include "KarplusString.h"

// === INTEGRATION CODE - NOT FROM ORIGINAL STRINGS ===
#include "dsp/part.h"
#include "dsp/patch.h"
#include "dsp/performance_state.h"
// ===================================================

#ifdef __arm__
#include <dev/sdram.h>
#endif

namespace SimpleRack {

class Engine {
public:
  Engine() = default;
  ~Engine() = default;

  void Init(const float sample_rate);

  // void SetStringPitch(const float nn);

  // void SetFeedbackGain(const float gain_dbfs);

  // void SetFeedbackDelay(const float delay_s);
  // void SetFeedbackLPFCutoff(const float cutoff_hz);
  // void SetFeedbackHPFCutoff(const float cutoff_hz);

  // void SetEchoDelayTime(const float echo_time);
  // void SetEchoDelayFeedback(const float echo_fb);
  // void SetEchoDelaySendAmount(const float echo_send);

  // // Both range 0-1
  // void SetReverbMix(const float mix);
  // void SetReverbFeedback(const float time);

  void SetOutputLevel(const float level);

  // Strings DSP parameter setters
  void SetStructure(float structure);
  void SetBrightness(float brightness);
  void SetDamping(float damping);
  void SetPosition(float position);
  void SetNote(float note_midi);
  void SetTonic(float tonic_midi);
  void SetChord(int32_t chord);
  void SetStrum(bool strum);

  // Part instance (public for direct access in main.cpp)
  resonate::Part part_;

  // State getters (for AudioCallback)
  const resonate::Patch& GetPatch() const { return patch_; }
  const resonate::PerformanceState& GetPerformanceState() const { return perf_state_; }
  float GetOutputLevel() const { return output_level_; }

  void ProcessCv(uint16_t& out0, uint16_t& out1);

private:
  // DSP state structures
  resonate::Patch patch_;
  resonate::PerformanceState perf_state_;

  // Reverb buffer (allocated in SDRAM)
  uint16_t* reverb_buffer_ = nullptr;

  // Trigger edge detection
  bool prev_trigger_state_ = false;

  float output_level_ = 0.5f;

  int testCounter = 0;
  int testRamp = 0;

  Engine(const Engine& other) = delete;
  Engine(Engine&& other) = delete;
  Engine& operator=(const Engine& other) = delete;
  Engine& operator=(Engine&& other) = delete;
};

}  // namespace SimpleRack

#endif
