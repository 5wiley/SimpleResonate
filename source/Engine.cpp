#include "Engine.h"
#include "DSPUtils.h"
#include "memory/sdram_alloc.h"

using namespace SimpleRack;
using namespace daisysp;

void Engine::Init(const float sample_rate) {
  // Allocate reverb buffer: 32,768 uint16_t = 64KB
  reverb_buffer_ = SDRAM::allocate_buf<uint16_t>(32768);

  // Initialize Part (from Strings DSP)
  part_.Init(reverb_buffer_);
  part_.set_polyphony(4);
  part_.set_model(resonate::RESONATOR_MODEL_MODAL);

  // Initialize patch defaults (0.5 = middle of range)
  patch_.structure = 0.5f;
  patch_.brightness = 0.5f;
  patch_.damping = 0.5f;
  patch_.position = 0.5f;

  // Initialize performance state
  perf_state_.strum = false;
  perf_state_.internal_exciter = true;   // Use internal plucker
  perf_state_.internal_strum = false;
  perf_state_.internal_note = false;
  perf_state_.note = 60.0f;              // Middle C
  perf_state_.tonic = 48.0f;             // C3
  perf_state_.fm = 0.0f;
  perf_state_.chord = 0;

  prev_trigger_state_ = false;
}

// void Engine::SetStringPitch(const float nn) {
//   // const auto freq = mtof(nn);
//   // strings_[0].SetFreq(freq);
//   // strings_[1].SetFreq(freq);
// }

// void Engine::SetFeedbackGain(const float gain_db) {
//   // fb_gain_ = dbfs2lin(gain_db);
// }

// void Engine::SetFeedbackDelay(const float delay_s) {
//   // fb_delay_samp_target_ =
//   //     DSY_CLAMP(delay_s * sample_rate_, 1.0f,
//   //               static_cast<float>(kMaxFeedbackDelaySamp - 1));
// }

// void Engine::SetFeedbackLPFCutoff(const float cutoff_hz) {
//   // fb_lpf_.SetCutoff(cutoff_hz);
// }

// void Engine::SetFeedbackHPFCutoff(const float cutoff_hz) {
//   fb_hpf_.SetCutoff(cutoff_hz);
// }

// void Engine::SetEchoDelayTime(const float echo_time) {
//   echo_delay_[0]->SetDelayTime(echo_time);
//   echo_delay_[1]->SetDelayTime(echo_time);
// }

// void Engine::SetEchoDelayFeedback(const float echo_fb) {
//   echo_delay_[0]->SetFeedback(echo_fb);
//   echo_delay_[1]->SetFeedback(echo_fb);
// }

// void Engine::SetEchoDelaySendAmount(const float echo_send) {
//   echo_send_ = echo_send;
// }

// void Engine::SetReverbMix(const float mix) {
//   verb_mix_ = fclamp(mix, 0.0f, 1.0f);
// }

// void Engine::SetReverbFeedback(const float time) { verb_->SetFeedback(time);
// }

void Engine::SetOutputLevel(const float level) {
  output_level_ = level;
}

void Engine::SetStructure(float structure) {
  patch_.structure = fclamp(structure, 0.0f, 1.0f);
}

void Engine::SetBrightness(float brightness) {
  patch_.brightness = fclamp(brightness, 0.0f, 1.0f);
}

void Engine::SetDamping(float damping) {
  patch_.damping = fclamp(damping, 0.0f, 1.0f);
}

void Engine::SetPosition(float position) {
  patch_.position = fclamp(position, 0.0f, 1.0f);
}

void Engine::SetNote(float note_midi) {
  perf_state_.note = note_midi;
}

void Engine::SetTonic(float tonic_midi) {
  perf_state_.tonic = tonic_midi;
}

void Engine::SetChord(int32_t chord) {
  perf_state_.chord = fclamp(chord, 0, 10);
}

void Engine::SetStrum(bool strum) {
  // Detect rising edge for trigger
  if (strum && !prev_trigger_state_) {
    perf_state_.strum = true;
  } else if (!strum) {
    perf_state_.strum = false;
  }
  prev_trigger_state_ = strum;
}

void Engine::ProcessCv(uint16_t& out0, uint16_t& out1) {
  // Example: Write values to the DAC outputs
  // For DAC output, values typically range from 0-4095 for 12-bit DAC

  ++testCounter;
  if (testCounter > 20) {
    testCounter = 0;
    ++testRamp;
  }
  if (testRamp > 4095) {
    testRamp = 0;
  }
  out0 = testRamp;
  out1 = testRamp;

  // out0 = (noise_.Process() * 0.5 + 1) * 4095;
  // out1 = (noise_.Process() * 0.5 + 1) * 4095;

  // out0 = 4000;
  // out1 = 4000;
}
