#include "daisy_seed.h"
#include "Engine.h"
#include "Controls.h"

using namespace SimpleRack;
using namespace daisy;
using namespace daisysp;

static const auto kSampleRate = SaiHandle::Config::SampleRate::SAI_48KHZ;
static const size_t kBlockSize = 4;

static DaisySeed hw;
static Engine engine;
static Controls controls;

void AudioCallback(AudioHandle::InputBuffer in,
                   AudioHandle::OutputBuffer out,
                   size_t size) {
  controls.UpdateParameter(hw);
  controls.Process();
  for (size_t i = 0; i < size; i++) {
    controls.UpdateCv(hw);
    engine.ProcessAudio(OUT_L[i], OUT_R[i]);
  }
  // limiter[0].ProcessBlock(OUT_L, size, 0.7f);
  // limiter[1].ProcessBlock(OUT_R, size, 0.7f);
}

void DacCallback(uint16_t** out, size_t size) {
  // controls.UpdateParameter(hw);
  // controls.Process();
  for (size_t i = 0; i < size; i++) {
    controls.UpdateCv(hw);
    engine.ProcessCv(out[0][i], out[1][i]);
  }
}

int main(void) {
  hw.Init();
  hw.SetAudioSampleRate(SaiHandle::Config::SampleRate::SAI_48KHZ);
  hw.SetAudioBlockSize(48);
  static const size_t size = 48;

  hw.StartLog();

  engine.Init(hw.AudioSampleRate());
  controls.Init(hw, engine);

  // DACs
  DacHandle::Config cfg;
  cfg.bitdepth = DacHandle::BitDepth::BITS_12;
  cfg.buff_state = DacHandle::BufferState::ENABLED;
  cfg.mode = DacHandle::Mode::DMA;
  // Only one of the following //
  cfg.chn = DacHandle::Channel::BOTH;  // A8 and A7
  // cfg.chn = DacHandle::Channel::ONE;   // A8
  // cfg.chn = DacHandle::Channel::TWO;   // A7
  hw.dac.Init(cfg);
  static uint16_t DMA_BUFFER_MEM_SECTION dacBuffer1[size], dacBuffer2[size];
  hw.dac.Start(dacBuffer1, dacBuffer2, size, DacCallback);

  hw.StartAudio(AudioCallback);

  while (1) {
    // hw.dac.WriteValue(DacHandle::Channel::BOTH, 4000);
    hw.Print(">");  // Begins control sequence

    FixedCapStr<16> str0("ADC0:");
    str0.AppendFloat(hw.adc.GetFloat(0));
    hw.Print(str0);
    // hw.Print(",");

    hw.PrintLine("");  // Ends control sequence
    System::Delay(1);
  }
}
//
