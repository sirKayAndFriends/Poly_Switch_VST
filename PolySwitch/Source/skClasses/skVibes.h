#pragma once
#include "JuceHeader.h"

struct visualParams
{
    int lineStartX;
    int visualWidth;
    int lineEndX;
    float visualCurveWidth;
    int controlMin;
    int controlMax;
    float controlWidth;
    int lineY;
    int maxY;
    int humpHeight;

    juce::Path curve;
    juce::Point<float> curveStart, controlDot, curveEnd, lineStart, lineEnd;
};

struct panelParams
{
    juce::Colour outerColour;
    juce::Colour innerColour;
    juce::Point<float> position;
    double width, height;
};

class SKVibes : public juce::LookAndFeel_V4
{
public:

    enum skColours {
        skLavendar = 0xffaa46d8,
        skLightBlue = 0xff00c3ff,
        skPaleCyan = 0xff00eaff,
        skDaisyYellow = 0xfffff700,
        skPaleYellow = 0xffffff4d,
        skCream = 0xffffff90,
        skMauve = 0xffbb00c0,
        skPalePurple = 0xffbb56ff,
        skPeach = 0xffffbaae,
        skBerryPink = 0xffd800a5,
        skPrincessPink = 0xffff38c3,
        skStrawbYog = 0xffffcfff,
        skRaspberry = 0xffff0064,
        skTomato = 0xffff0000,
        skMintGreen = 0xff5cfff9,
        skBarleyGreen = 0xff3ee8bb,
        skForestGreen = 0xff00e500,
        skDuckEgg = 0xff3ee1ff,
        skCocoa = 0xff550030,
        skClear = 0x00ffffff,
        skWhite = 0xffffffff,
        skBlack = 0xff000000
    };

    struct diodeArray
    {
        juce::Button* button;
        juce::Point<float> position;
        juce::Rectangle<float> bounds;
        juce::Point<float> midPoint;

        std::vector<juce::Point<float>> triangle;
        std::vector<juce::Point<float>> wire;
        std::vector<juce::Point<float>> cathode;

        int softColour;
        int hardColour;
        bool direction;
        bool state;
        int unit;
        double thickness;
    } diode1, diode2, diode3, diode4;

    std::vector<diodeArray*> diodes
    {
        &diode1,
        &diode2,
        &diode3,
        &diode4
    };

    static const juce::Font& getPluginFont()
    {
        static juce::Font quicksand(juce::Font(juce::Typeface::createSystemTypefaceFor(BinaryData::QuicksandRegular_otf,
            BinaryData::QuicksandRegular_otfSize)));

        return quicksand;
    };

    //create a grid of squares
    void createDivisions(int width, int height, int numX, int numY, int margin);

    void createSubDivisions(int numsubDivisions);

    //return centre points of grid
    void getGridCentres();

    //return grid
    std::vector<juce::Rectangle<float>>& getGrid();

    //return subGrid
    std::vector<juce::Rectangle<float>>& getSubGrid();

    //return a single corner point in row/column (0 index)
    juce::Point<float>& getGridRectPoint(int row, int col);
   
    //return single centre point in row/column (0 index)
    juce::Point<float>& getGridCentrePoint(int row, int col);

    //return a single rectangle from grid
    juce::Rectangle<float>& getGridRect(int row, int col);

    //return XY value of bottom left corner of selected rectangle
    juce::Point<float> getGridRectBottomLeft(int row, int col);

    //return XY value of bottom right corner of selected rectangle
    juce::Point<float> getGridRectBottomRight(int row, int col);

    //return XY value of top left of selected rectangle
    juce::Point<float> getGridRectTopLeft(int row, int col);

    //return XY value of top right of selected rectangle
    juce::Point<float> getGridRectTopRight(int row, int col);

    //returns size of grid squares
    std::vector<float> getUnitSize();

    //centre component in grid square (draw from top-left)
    juce::Point<float> drawFromCentre(int row, int col, float width, float height);

    //centre component on grid square boundaries
    juce::Point<float> drawFromEdge(int row, int col, float width, float height);

    //align component to left of selected square
    //similar to changing draw point from top-left to bottom-right
    juce::Point<float>& convertToBottomRight(int row, int col, float width, float height);
    
    void drawRotarySlider(juce::Graphics& g, int x, int y, int width, int height, float sliderPos, const float rotaryStartAngle, const float rotaryEndAngle, juce::Slider& slider) override;

    struct panelParams setupPanels(float x, float y, double width, double height, int outerCol, int innerCol);

    void drawPanel(juce::Graphics& g, double x, double y, double width, double height, int bgCol);
    
    void drawDoublePanel(juce::Graphics& g, panelParams* panel);

    void drawLabels(juce::Slider* knob, juce::String& text);
    
    struct visualParams setVisualParams(int x, int y, int width, int height);

    void drawMidVisual(juce::Graphics& g, visualParams* params, float& peakX, float& peakY);

    void diodeInit(juce::Button* button, float x, float y, int size, int colour, bool direction, bool state, int index);

    void drawDiode(juce::Graphics& g, int index, bool state);

    void diodeSort(juce::Button* button, bool state);

private:

    std::vector<juce::Rectangle<float>> gridSquare;
    std::vector<juce::Rectangle<float>> gridSubSquares;
    std::vector<juce::Point<float>> gridCentres;

    int numGridX;
    int numGridY;

    int numSubGrid;

    float unitWidth;
    float unitHeight;
};