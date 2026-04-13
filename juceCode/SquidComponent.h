#pragma once
#include <JuceHeader.h>
#include "BodyComponent.h"
#include "EyeComponent.h"
#include "LegComponent.h"

//==============================================================================
// Top-level squid UI. Drop this into your AudioProcessorEditor.
//
// Layout (all in parent / plugin coordinates):
//
//          ┌─────────────────────────────┐
//          │         BodyComponent       │  ← purely visual
//          │       ┌───┐     ┌───┐       │
//          │       │Eye│     │Eye│       │  ← EyeComponent x2
//          │       └───┘     └───┘       │
//          └─────────────────────────────┘
//        ╱   ╲   ╱   ╲   ╱   ╲   ╱   ╲
//       LegComponent x6  ← draggable bezier tentacles
//
// Z-order (back to front): legs → body → eyes → hint label
//
// Wire up parameters by implementing legValueChanged() and setting
// onStateChanged on each eye — both are stubbed with comments below.
//==============================================================================
class SquidComponent : public juce::Component,
                       private LegComponent::Listener
{
public:
    static constexpr int kWidth  = 500;
    static constexpr int kHeight = 570;

    //==========================================================================
    SquidComponent()
    {
        setSize (kWidth, kHeight);

        addLegs();    // added first → rendered behind body
        addBody();
        addEyes();    // added last → rendered on top of body
        addHint();
    }

    //==========================================================================
    void paint (juce::Graphics& g) override
    {
        g.fillAll (juce::Colour (241, 238, 230));
    }

    //==========================================================================
    // Accessors — useful when wiring to APVTS in the editor
    LegComponent& getLeg (int i)  { return *legs[i]; }
    EyeComponent& getEye (int i)  { return *eyes[i]; }

private:
    //==========================================================================
    // Squid geometry constants (all in SquidComponent's coordinate space)
    static constexpr float SX = kWidth  * 0.5f;   // horizontal centre
    static constexpr float SY = 200.0f;            // vertical centre of body
    static constexpr float BW = 108.0f;            // body half-width reference
    static constexpr float BH = 152.0f;            // body half-height reference

    static constexpr float kLegStartY  = SY + BH * 0.47f;
    static constexpr float kEyeOffsetY = -10.0f;

    inline static const juce::StringArray kLegNames  { "FREQ","RES","DRIVE","MIX","ATK","REL" };
    inline static const juce::StringArray kEyeLabels { "ENV", "REV" };
    inline static const float             kEyeX[2]   { SX - 31.0f, SX + 31.0f };

    //==========================================================================
    void addLegs()
    {
        for (int i = 0; i < 6; ++i)
        {
            const float pct = i / 5.0f;
            const float sx  = SX + juce::jmap (pct, 0.0f, 1.0f, -BW * 0.47f, BW * 0.47f);

            auto* leg = legs.add (new LegComponent (i, { sx, kLegStartY }));
            leg->setParamName (kLegNames[i]);
            leg->setListener  (this);
            addAndMakeVisible (leg);
        }
    }

    void addBody()
    {
        // Give the body component enough room for the mantle tip and both fins
        const int bx = (int) (SX - BW * 0.95f);
        const int by = (int) (SY - BH * 0.62f);
        const int bw = (int) (BW * 1.90f);
        const int bh = (int) (BH * 1.22f);

        body.setBounds (bx, by, bw, bh);
        body.setBodyCenter (SX - (float) bx,    // convert parent origin → local
                            SY - (float) by);
        addAndMakeVisible (body);
    }

    void addEyes()
    {
        for (int i = 0; i < 2; ++i)
        {
            auto* eye = eyes.add (new EyeComponent());
            eye->setLabel (kEyeLabels[i]);

            // Centre the eye component on its logical position
            const float ex = kEyeX[i];
            const float ey = SY + kEyeOffsetY;
            eye->setBounds ((int) (ex - 30), (int) (ey - 24),
                            eye->getWidth(), eye->getHeight());

            // Wire to your APVTS here, e.g.:
            //   eye->onStateChanged = [i, this](bool on) {
            //       apvts.getParameter("eye" + juce::String(i))
            //            ->setValueNotifyingHost(on ? 1.0f : 0.0f);
            //   };
            eye->onStateChanged = [i] (bool on)
            {
                juce::ignoreUnused (i, on); // replace with APVTS call
            };

            addAndMakeVisible (eye);
        }
    }

    void addHint()
    {
        hint.setText ("drag legs  ·  click eyes", juce::dontSendNotification);
        hint.setFont  (juce::Font (juce::FontOptions (10.0f)));
        hint.setColour (juce::Label::textColourId, juce::Colour (185, 180, 212));
        hint.setJustificationType (juce::Justification::centred);
        hint.setBounds (0, kHeight - 34, kWidth, 20);
        hint.setInterceptsMouseClicks (false, false);
        addAndMakeVisible (hint);
    }

    //==========================================================================
    // LegComponent::Listener
    void legValueChanged (int legIndex, float newValue) override
    {
        juce::ignoreUnused (legIndex, newValue);

        // Wire to your APVTS here, e.g.:
        //   if (auto* p = apvts.getParameter(kLegNames[legIndex]))
        //       p->setValueNotifyingHost(newValue);
    }

    //==========================================================================
    BodyComponent                  body;
    juce::OwnedArray<LegComponent> legs;
    juce::OwnedArray<EyeComponent> eyes;
    juce::Label                    hint;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SquidComponent)
};
