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
  // 1. Update control-rate parameters once per block
  controls.UpdateParameter(hw);
  controls.Process();  // Apply smoothing

  // 2. Update audio-rate CV once per block (not per-sample!)
  controls.UpdateCv(hw);

  // 3. Process audio through Engine (handles Part internally)
  engine.ProcessAudio(IN_L, OUT_L, OUT_R, size);
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
    hw.Print(",");

    FixedCapStr<16> str1("ADC1:");
    str1.AppendFloat(hw.adc.GetFloat(1));
    hw.Print(str1);
    hw.Print(",");

    FixedCapStr<16> str2("ADC2:");
    str2.AppendFloat(hw.adc.GetFloat(2));
    hw.Print(str2);
    hw.Print(",");

    FixedCapStr<16> str3("ADC3:");
    str3.AppendFloat(hw.adc.GetFloat(3));
    hw.Print(str3);
    hw.Print(",");

    FixedCapStr<16> str4("ADC4:");
    str4.AppendFloat(hw.adc.GetFloat(4));
    hw.Print(str4);
    hw.Print(",");

    FixedCapStr<16> str5("ADC5:");
    str5.AppendFloat(hw.adc.GetFloat(5));
    hw.Print(str5);
    hw.Print(",");

    FixedCapStr<16> str6("ADC6:");
    str6.AppendFloat(hw.adc.GetFloat(6));
    hw.Print(str6);
    // hw.Print(",");

    hw.PrintLine("");  // Ends control sequence
    System::Delay(5);
  }
}
//
