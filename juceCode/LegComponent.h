#pragma once
#include <JuceHeader.h>

//==============================================================================
// One tentacle / parameter leg.
//
// The component resizes itself dynamically as its tip is dragged, always
// keeping both the start point and the tip within its bounds.
//
// getValue() maps the tip's vertical position to [0, 1]:
//   tip at rest position  → ~0.5
//   tip pulled down       → toward 1.0
//   tip pushed up         → toward 0.0
//
// Implement Listener and call setListener() to receive value changes.
//==============================================================================
class LegComponent : public juce::Component
{
public:
    //==========================================================================
    struct Listener
    {
        virtual ~Listener() = default;
        virtual void legValueChanged (int legIndex, float newValue) = 0;
    };

    //==========================================================================
    LegComponent (int index, juce::Point<float> startInParent)
        : legIndex (index), startPt (startInParent)
    {
        const float pct = index / 5.0f;
        tipPt = { startInParent.x + juce::jmap (pct, 0.0f, 1.0f, -16.0f, 16.0f),
                  startInParent.y + 72.0f + std::abs (pct - 0.5f) * 46.0f };
        updateBounds();
    }

    //==========================================================================
    void setParamName (const juce::String& name) { paramName = name; repaint(); }
    void setListener  (Listener* l)               { listener = l; }

    float getValue() const
    {
        return juce::jlimit (0.0f, 1.0f,
            juce::jmap (tipPt.y,
                        startPt.y - 60.0f, startPt.y + 90.0f,
                        0.0f, 1.0f));
    }

    juce::Point<float> getTipInParent()   const { return tipPt;   }
    juce::Point<float> getStartInParent() const { return startPt; }

    //==========================================================================
    void paint (juce::Graphics& g) override
    {
        const auto ls = toLocal (startPt);
        const auto lt = toLocal (tipPt);

        // Control points — first pulls straight down from start,
        // second arrives straight from above into the tip
        const juce::Point<float> c1 { ls.x, ls.y + 58.0f };
        const juce::Point<float> c2 { lt.x, lt.y - 22.0f };

        const bool active = isDragging || isHovered;
        const juce::Colour legCol = (legIndex % 2 == 0) ? dark : body;

        // ── Tapered bezier ───────────────────────────────────────────────────
        for (int seg = 0; seg < 12; ++seg)
        {
            const float a  = seg        / 12.0f;
            const float b  = (seg + 1)  / 12.0f;
            const float sw = juce::jmap ((float) seg, 0.0f, 11.0f, 10.0f, 1.2f);

            const auto p1 = cubicPoint (ls, c1, c2, lt, a);
            const auto p2 = cubicPoint (ls, c1, c2, lt, b);

            juce::Path segment;
            segment.startNewSubPath (p1);
            segment.lineTo (p2);
            g.setColour (legCol.withAlpha (active ? 1.0f : 0.82f));
            g.strokePath (segment, juce::PathStrokeType (
                sw,
                juce::PathStrokeType::curved,
                juce::PathStrokeType::rounded));
        }

        // ── Tip dot ──────────────────────────────────────────────────────────
        g.setColour (dark);
        g.fillEllipse (lt.x - 3.5f, lt.y - 3.5f, 7.0f, 7.0f);

        // ── Drag handle ring ─────────────────────────────────────────────────
        g.setColour (active ? lite.withAlpha (0.9f) : dark.withAlpha (0.22f));
        g.drawEllipse (lt.x - 9.0f, lt.y - 9.0f, 18.0f, 18.0f, 1.5f);

        // ── Param label + value ──────────────────────────────────────────────
        const float labelAlpha = active ? 1.0f : 0.55f;
        g.setFont (juce::Font (juce::FontOptions ("Courier New", 9.0f, juce::Font::plain)));

        g.setColour (dark.withAlpha (labelAlpha));
        g.drawText (paramName,
                    (int) lt.x - 20, (int) lt.y + 8, 40, 12,
                    juce::Justification::centred);

        g.setColour (body.withAlpha (labelAlpha));
        g.drawText (juce::String (getValue(), 2),
                    (int) lt.x - 20, (int) lt.y + 20, 40, 12,
                    juce::Justification::centred);
    }

    //==========================================================================
    void mouseEnter (const juce::MouseEvent&) override { isHovered = true;  repaint(); }
    void mouseExit  (const juce::MouseEvent&) override { isHovered = false; repaint(); }

    void mouseDown (const juce::MouseEvent& e) override
    {
        if (e.position.getDistanceFrom (toLocal (tipPt)) < 18.0f)
        {
            isDragging = true;
            setMouseCursor (juce::MouseCursor::DraggingHandCursor);
            repaint();
        }
    }

    void mouseDrag (const juce::MouseEvent& e) override
    {
        if (! isDragging) return;

        tipPt = toParent (e.position);
        updateBounds();   // resize component to keep tip in bounds
        repaint();

        if (listener != nullptr)
            listener->legValueChanged (legIndex, getValue());
    }

    void mouseUp (const juce::MouseEvent&) override
    {
        isDragging = false;
        setMouseCursor (juce::MouseCursor::NormalCursor);
        repaint();
    }

    // Only intercept clicks near the draggable tip
    bool hitTest (int x, int y) override
    {
        return juce::Point<float> ((float) x, (float) y)
                   .getDistanceFrom (toLocal (tipPt)) < 18.0f;
    }

private:
    //==========================================================================
    void updateBounds()
    {
        constexpr float pad = 32.0f;

        const float x1 = std::min (startPt.x, tipPt.x) - pad;
        const float y1 = std::min (startPt.y, tipPt.y) - pad;
        const float x2 = std::max (startPt.x, tipPt.x) + pad;
        const float y2 = std::max (startPt.y, tipPt.y) + pad + 36.0f; // label space

        setBounds ((int) x1, (int) y1,
                   (int) (x2 - x1), (int) (y2 - y1));
    }

    // Parent ↔ local coordinate helpers
    juce::Point<float> toLocal  (juce::Point<float> p) const { return p - getBounds().getTopLeft().toFloat(); }
    juce::Point<float> toParent (juce::Point<float> p) const { return p + getBounds().getTopLeft().toFloat(); }

    static juce::Point<float> cubicPoint (
        juce::Point<float> p0, juce::Point<float> p1,
        juce::Point<float> p2, juce::Point<float> p3, float t) noexcept
    {
        const float u = 1.0f - t;
        return p0 * (u * u * u)
             + p1 * (3.0f * u * u * t)
             + p2 * (3.0f * u * t * t)
             + p3 * (t * t * t);
    }

    //==========================================================================
    const int legIndex;
    juce::String paramName = "PARAM";

    juce::Point<float> startPt;   // fixed, in parent coordinates
    juce::Point<float> tipPt;     // draggable, in parent coordinates

    bool isDragging = false;
    bool isHovered  = false;

    Listener* listener = nullptr;

    const juce::Colour body { 82,  73, 178 };
    const juce::Colour dark { 44,  36, 118 };
    const juce::Colour lite { 165, 160, 225 };

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (LegComponent)
};
