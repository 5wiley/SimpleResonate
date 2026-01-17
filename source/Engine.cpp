#include "Engine.h"
#include "DSPUtils.h"
#include "memory/sdram_alloc.h"

using namespace SimpleRack;
using namespace daisysp;

void Engine::Init(const float sample_rate) {
  // using ED = EchoDelay<kMaxEchoDelaySamp>;

  // echo_delay_[0] = EchoDelayPtr(SDRAM::allocate<ED>());
  // echo_delay_[1] = EchoDelayPtr(SDRAM::allocate<ED>());
  // verb_ = VerbPtr(SDRAM::allocate<ReverbSc>());

  // sample_rate_ = sample_rate;
  // fb_delay_smooth_coef_ = onepole_coef(0.2f, sample_rate);

  noise_.Init();
  noise_.SetAmp(1);  // VERY LOUD

  // for (unsigned int i = 0; i < 2; i++) {

  //   strings_[i].Init(sample_rate);
  //   strings_[i].SetBrightness(0.98f);
  //   strings_[i].SetFreq(mtof(40.0f));
  //   strings_[i].SetDamping(0.4f);

  //   fb_delayline_[i].Init();

  //   echo_delay_[i]->Init(sample_rate);
  //   echo_delay_[i]->SetDelayTime(5.0f, true);
  //   echo_delay_[i]->SetFeedback(0.5f);
  //   echo_delay_[i]->SetLagTime(0.5f);

  //   overdrive_[i].Init();
  //   overdrive_[i].SetDrive(0.4);
  // }

  // verb_->Init(sample_rate);
  // verb_->SetFeedback(0.85f);
  // verb_->SetLpFreq(12000.0f);

  // fb_lpf_.Init(sample_rate);
  // fb_lpf_.SetQ(0.9f);
  // fb_lpf_.SetCutoff(18000.0f);

  // fb_hpf_.Init(sample_rate);
  // fb_hpf_.SetQ(0.9f);
  // fb_hpf_.SetCutoff(60.f);
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

void Engine::ProcessAudio(float& outL, float& outR) {
  // // --- Update audio-rate-smoothed control params ---

  // fonepole(fb_delay_samp_, fb_delay_samp_target_, fb_delay_smooth_coef_);

  // // --- Process Samples ---

  float /* inL, inR, */ sampL, sampR /* , echoL, echoR, verbL, verbR */;

  sampL = noise_.Process();
  sampR = noise_.Process();

  // ---> Output
  outL = sampL * output_level_;
  outR = sampR * output_level_;
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
