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

void Controls::Init(DaisySeed& hw, Engine& engine) {
  params_.Init(hw.AudioSampleRate() / hw.AudioBlockSize());
  // CVRegistry doesn't need Init() - it works at audio rate without smoothing
  // del_sw_.Init(static_cast<Pin>(kDelaySwitchPin), 1000.0f,
  // Switch::TYPE_TOGGLE,
  //              Switch::POLARITY_INVERTED, GPIO::Pull::PULLUP);
  initADCs(hw);
  registerParams(engine);
  registerCVs(engine);
}

void Controls::UpdateParameter(DaisySeed& hw) {
  params_.UpdateNormalized(Parameter::Structure, hw.adc.GetFloat(0));
  params_.UpdateNormalized(Parameter::Brightness, hw.adc.GetFloat(1));
  params_.UpdateNormalized(Parameter::Damping, hw.adc.GetFloat(2));
  params_.UpdateNormalized(Parameter::Position, hw.adc.GetFloat(3));
}

void Controls::UpdateCv(DaisySeed& hw) {
  cv_.UpdateNormalized(CV::OutputVolume, hw.adc.GetFloat(4));
  cv_.UpdateNormalized(CV::Note, hw.adc.GetFloat(5));
  cv_.UpdateNormalized(CV::Strum, hw.adc.GetFloat(6));
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
      std::bind(&Engine::SetStructure, &engine, _1), 0.05f);

  params_.Register(Parameter::Brightness, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetBrightness, &engine, _1), 0.05f);

  params_.Register(Parameter::Damping, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetDamping, &engine, _1), 0.05f);

  params_.Register(Parameter::Position, 0.5f, 0.0f, 1.0f,
      std::bind(&Engine::SetPosition, &engine, _1), 0.05f);
}

void Controls::registerCVs(Engine& engine) {
  using namespace std::placeholders;

  // Output volume: audio-rate, exponential scaling
  cv_.Register(CV::OutputVolume, 0.5f, 0.0f, 1.0f,
               std::bind(&Engine::SetOutputLevel, &engine, _1),
               daisysp::Mapping::EXP);

  // Note CV: V/Oct input, 0V = MIDI 24 (C1), 5V = MIDI 108 (C8)
  cv_.Register(CV::Note, 60.0f, 24.0f, 108.0f,
               std::bind(&Engine::SetNote, &engine, _1),
               daisysp::Mapping::LINEAR);

  // Strum: Gate/trigger input, >2.5V triggers new note
  cv_.Register(CV::Strum, 0.0f, 0.0f, 1.0f,
               [&engine](float val) {
                 engine.SetStrum(val > 0.5f);  // 2.5V threshold
               },
               daisysp::Mapping::LINEAR);
}
