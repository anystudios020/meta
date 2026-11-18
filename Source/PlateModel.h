#pragma once
#include <array>
#include <cstddef>

namespace metaphysical
{
struct PlateParameters
{
    double widthMetres = 1.20;
    double heightMetres = 0.80;
    double thicknessMetres = 0.012;
    double youngsModulusPa = 70.0e9; // representative soda-lime glass
    double densityKgM3 = 2500.0;
    double poissonsRatio = 0.22;
    double modalLoss = 0.0018;       // dimensionless loss factor
    double impactX = 0.50;           // normalized [0, 1]
    double impactY = 0.50;
    double rodVelocityMps = 2.0;
    double rodMassKg = 0.18;
};

struct ModeInfo
{
    int m = 1;
    int n = 1;
    double frequencyHz = 0.0;
};

class PlateModel
{
public:
    static constexpr std::size_t maxModes = 64;

    void prepare(double sampleRate);
    void setParameters(const PlateParameters& parameters);
    void strike();
    float processSample();
    void reset();

    std::size_t getNumModes() const noexcept { return numModes; }
    ModeInfo getModeInfo(std::size_t index) const noexcept;
    double getFundamentalHz() const noexcept;
    bool isPlausible() const noexcept;

private:
    struct Mode
    {
        double frequency = 0.0;
        double amplitude = 0.0;
        double velocity = 0.0;
        double decay = 0.0;
        double cosOmega = 1.0;
        double sinOmega = 0.0;
        int m = 1;
        int n = 1;
    };

    void rebuildModes();
    PlateParameters params;
    std::array<Mode, maxModes> modes {};
    std::size_t numModes = 0;
    double sampleRate = 44100.0;
    double outputScale = 0.00002;
};
}
