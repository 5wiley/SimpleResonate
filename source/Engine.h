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

  // Main audio processing method
  void ProcessAudio(const float* in, float* out_l, float* out_r, size_t size);

  void ProcessCv(uint16_t& out0, uint16_t& out1);

private:
  // Part instance (original Rings DSP code)
  resonate::Part part_;

  // DSP state structures
  resonate::Patch patch_;
  resonate::PerformanceState perf_state_;

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
