#include "Controls.h"
#include <functional>

using namespace SimpleRack;
using namespace daisy;

////////////// SIMPLE X DAISY PINOUT CHEATSHEET ///////////////

// 3v3           29  |       |   20    AGND
// D15 / A0      30  |       |   19    OUT 01
// D16 / A1      31  |       |   18    OUT 00
// D17 / A2      32  |       |   17    IN 01
// D18 / A3      33  |       |   16    IN 00
// D19 / A4      34  |       |   15    D14
// D20 / A5      35  |       |   14    D13
// D21 / A6      36  |       |   13    D12
// D22 / A7 DAC1 37  |       |   12    D11
// D23 / A8 DAC2 38  |       |   11    D10
// D24 / A9      39  |       |   10    D9
// D25 / A10     40  |       |   09    D8
// D26           41  |       |   08    D7
// D27           42  |       |   07    D6
// D28 / A11     43  |       |   06    D5
// D29           44  |       |   05    D4
// D30           45  |       |   04    D3
// 3v3 Digital   46  |       |   03    D2
// VIN           47  |       |   02    D1
// DGND          48  |       |   01    D0

// TODO: Add footprint numbers to these

static constexpr daisy::Pin kStructureAdcPin = daisy::seed::A0;     // 30
static constexpr daisy::Pin kBrightnessAdcPin = daisy::seed::A1;    // 31
static constexpr daisy::Pin kDampingAdcPin = daisy::seed::A2;       // 32
static constexpr daisy::Pin kPositionAdcPin = daisy::seed::A3;      // 33
static constexpr daisy::Pin kOutputVolumeAdcPin = daisy::seed::A4;  // 34
static constexpr daisy::Pin kNoteCvPin = daisy::seed::A5;           // 35
static constexpr daisy::Pin kStrumCvPin = daisy::seed::A6;          // 36
static constexpr daisy::Pin kStrumButtonPin = daisy::seed::D7;      // 06

void Controls::Init(DaisySeed& hw, Engine& engine) {
  params_.Init(hw.AudioSampleRate() / hw.AudioBlockSize());

  strum_button_.Init(
      kStrumButtonPin,
      1000.0f,
      Switch::TYPE_MOMENTARY,
      Switch::POLARITY_INVERTED
  );

  initADCs(hw);
  registerParams(engine);
}

void Controls::UpdateParameter(DaisySeed& hw) {
  params_.UpdateNormalized(Parameter::Structure, hw.adc.GetFloat(0));
  params_.UpdateNormalized(Parameter::Brightness, hw.adc.GetFloat(1));
  params_.UpdateNormalized(Parameter::Damping, hw.adc.GetFloat(2));
  params_.UpdateNormalized(Parameter::Position, hw.adc.GetFloat(3));
  params_.UpdateNormalized(Parameter::OutputVolume, hw.adc.GetFloat(4));
  params_.UpdateNormalized(Parameter::Note, hw.adc.GetFloat(5));

  strum_button_.Debounce();

  bool button_trigger = strum_button_.RisingEdge();

  // Gate-to-trigger conversion for CV input (detect rising edge)
  bool strum_cv_gate = hw.adc.GetFloat(6) > 0.5f;  // 2.5V threshold for 0-5V input
  bool strum_cv_trigger = strum_cv_gate && !prev_strum_cv_gate_;
  prev_strum_cv_gate_ = strum_cv_gate;

  bool combined_trigger = strum_cv_trigger || button_trigger;
  params_.UpdateNormalized(Parameter::Strum, combined_trigger ? 1.0f : 0.0f);
}

void Controls::UpdateCv(DaisySeed& hw) {
}

void Controls::initADCs(DaisySeed& hw) {
  AdcChannelConfig config[kNumAdcChannels];
  config[0].InitSingle(kStructureAdcPin);
  config[1].InitSingle(kBrightnessAdcPin);
  config[2].InitSingle(kDampingAdcPin);
  config[3].InitSingle(kPositionAdcPin);
  config[4].InitSingle(kOutputVolumeAdcPin);
  config[5].InitSingle(kNoteCvPin);
  config[6].InitSingle(kStrumCvPin);

  hw.adc.Init(config, kNumAdcChannels);
  hw.adc.Start();
}

void Controls::registerParams(Engine& engine) {
  using namespace std::placeholders;

  // Patch parameters: control-rate with 50ms smoothing
  params_.Register(Parameter::Structure, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetStructure, &engine, _1), 0.05f, daisysp::Mapping::LINEAR);

  params_.Register(Parameter::Brightness, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetBrightness, &engine, _1), 0.05f, daisysp::Mapping::LINEAR);

  params_.Register(Parameter::Damping, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetDamping, &engine, _1), 0.05f, daisysp::Mapping::LINEAR);

  params_.Register(Parameter::Position, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetPosition, &engine, _1), 0.05f, daisysp::Mapping::LINEAR);

  params_.Register(Parameter::OutputVolume, 0.5f, 0.0f, 1.0f,
               std::bind(&Engine::SetOutputLevel, &engine, _1), 0.0f,
               daisysp::Mapping::EXP);

  // Note CV: V/Oct input, 0V = MIDI 24 (C1), 5V = MIDI 108 (C8)
  params_.Register(Parameter::Note, 60.0f, 24.0f, 108.0f,
               std::bind(&Engine::SetNote, &engine, _1), 0.0f,
               daisysp::Mapping::LINEAR);

  // Strum: Gate/trigger input, >2.5V triggers new note
  params_.Register(Parameter::Strum, 0.0f, 0.0f, 1.0f,
              std::bind(&Engine::SetStrum, &engine, _1), 0.0f,
               daisysp::Mapping::LINEAR);
}