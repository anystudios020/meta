#include "PlateModel.h"
#include <algorithm>
#include <cmath>

namespace metaphysical
{
namespace
{
constexpr double pi = 3.14159265358979323846;
constexpr int maxOrder = 12;
}

void PlateModel::prepare(double newSampleRate)
{
    sampleRate = std::max(8000.0, newSampleRate);
    rebuildModes();
}

void PlateModel::setParameters(const PlateParameters& p)
{
    const bool unchanged = params.widthMetres == p.widthMetres
        && params.heightMetres == p.heightMetres
        && params.thicknessMetres == p.thicknessMetres
        && params.youngsModulusPa == p.youngsModulusPa
        && params.densityKgM3 == p.densityKgM3
        && params.poissonsRatio == p.poissonsRatio
        && params.modalLoss == p.modalLoss
        && params.impactX == p.impactX
        && params.impactY == p.impactY
        && params.rodVelocityMps == p.rodVelocityMps
        && params.rodMassKg == p.rodMassKg;
    if (unchanged)
        return;
    params = p;
    params.widthMetres = std::clamp(params.widthMetres, 0.05, 400.0);
    params.heightMetres = std::clamp(params.heightMetres, 0.05, 400.0);
    params.thicknessMetres = std::clamp(params.thicknessMetres, 0.0005, 0.5);
    params.densityKgM3 = std::clamp(params.densityKgM3, 100.0, 30000.0);
    params.youngsModulusPa = std::clamp(params.youngsModulusPa, 1.0e6, 2.0e12);
    params.poissonsRatio = std::clamp(params.poissonsRatio, 0.0, 0.49);
    params.modalLoss = std::clamp(params.modalLoss, 0.00005, 0.08);
    params.impactX = std::clamp(params.impactX, 0.001, 0.999);
    params.impactY = std::clamp(params.impactY, 0.001, 0.999);
    params.rodVelocityMps = std::clamp(params.rodVelocityMps, 0.01, 30.0);
    params.rodMassKg = std::clamp(params.rodMassKg, 0.005, 10.0);
    rebuildModes();
}

void PlateModel::rebuildModes()
{
    numModes = 0;
    const double a = params.widthMetres;
    const double b = params.heightMetres;
    const double h = params.thicknessMetres;
    const double nu = params.poissonsRatio;
    const double rigidity = params.youngsModulusPa * h * h * h / (12.0 * (1.0 - nu * nu));
    const double arealMass = params.densityKgM3 * h;
    const double coefficient = (pi / 2.0) * std::sqrt(rigidity / arealMass);

    // Kirchhoff-Love thin-plate frequencies for a simply supported rectangle.
    // This is a first-order boundary-condition model, not a general FEM solver.
    for (int m = 1; m <= maxOrder; ++m)
    {
        for (int n = 1; n <= maxOrder; ++n)
        {
            const double invLengths = (static_cast<double>(m * m) / (a * a))
                                    + (static_cast<double>(n * n) / (b * b));
            const double frequency = coefficient * invLengths;
            if (frequency < 8.0 || frequency > sampleRate * 0.45)
                continue;
            if (numModes >= maxModes)
                return;

            auto& mode = modes[numModes++];
            mode = {};
            mode.frequency = frequency;
            mode.m = m;
            mode.n = n;
            const double omega = 2.0 * pi * frequency / sampleRate;
            mode.cosOmega = std::cos(omega);
            mode.sinOmega = std::sin(omega);
            // Loss-factor approximation: amplitude envelope exp(-eta*pi*f*t).
            mode.decay = std::exp(-params.modalLoss * pi * frequency / sampleRate);
        }
    }
}

void PlateModel::strike()
{
    // A short, velocity-dependent contact impulse. The spatial mode shape determines
    // how strongly the impact excites each mode. Rod/plate contact is simplified here.
    const double plateMass = params.densityKgM3 * params.thicknessMetres
                           * params.widthMetres * params.heightMetres;
    const double effectiveMass = (params.rodMassKg * plateMass) / (params.rodMassKg + plateMass);
    const double impulse = effectiveMass * params.rodVelocityMps;
    const double modalMass = std::max(1.0e-6, plateMass * 0.25);

    for (std::size_t i = 0; i < numModes; ++i)
    {
        auto& mode = modes[i];
        const double shape = std::sin(mode.m * pi * params.impactX)
                           * std::sin(mode.n * pi * params.impactY);
        // Generalized velocity kick. Modal normalization and radiation are represented
        // by a conservative scale; later calibration will use measured plate responses.
        mode.velocity += (impulse / modalMass) * shape;
    }
}

float PlateModel::processSample()
{
    double sum = 0.0;
    for (std::size_t i = 0; i < numModes; ++i)
    {
        auto& mode = modes[i];
        const double next = mode.decay * (mode.amplitude * mode.cosOmega
                         + mode.velocity * mode.sinOmega / (2.0 * pi * mode.frequency / sampleRate));
        const double nextVelocity = mode.decay * (mode.velocity * mode.cosOmega
                                 - mode.amplitude * (2.0 * pi * mode.frequency / sampleRate)
                                 * mode.sinOmega);
        mode.amplitude = next;
        mode.velocity = nextVelocity;
        sum += mode.velocity;
    }
    return static_cast<float>(std::clamp(sum * outputScale, -1.0, 1.0));
}

void PlateModel::reset()
{
    for (auto& mode : modes)
    {
        mode.amplitude = 0.0;
        mode.velocity = 0.0;
    }
}

ModeInfo PlateModel::getModeInfo(std::size_t index) const noexcept
{
    if (index >= numModes)
        return {};
    return { modes[index].m, modes[index].n, modes[index].frequency };
}

double PlateModel::getFundamentalHz() const noexcept
{
    double lowest = 0.0;
    for (std::size_t i = 0; i < numModes; ++i)
        if (lowest == 0.0 || modes[i].frequency < lowest)
            lowest = modes[i].frequency;
    return lowest;
}

bool PlateModel::isPlausible() const noexcept
{
    // The current analytical model assumes a thin, homogeneous, simply-supported plate.
    // A thickness-to-short-span ratio above 1/20 is flagged as outside that assumption.
    const double shortSpan = std::min(params.widthMetres, params.heightMetres);
    return params.thicknessMetres / shortSpan <= 0.05;
}
}
