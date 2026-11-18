#include "PlateModel.h"
#include <cassert>
#include <cmath>
#include <iostream>

int main()
{
    metaphysical::PlateModel plate;
    plate.prepare(48000.0);
    metaphysical::PlateParameters p;
    plate.setParameters(p);
    const auto normalFundamental = plate.getFundamentalHz();
    assert(normalFundamental > 0.0);
    assert(plate.getNumModes() > 0);
    assert(plate.isPlausible());

    p.widthMetres = 4.0;
    p.heightMetres = 2.0;
    plate.setParameters(p);
    const auto largerFundamental = plate.getFundamentalHz();
    assert(largerFundamental < normalFundamental);

    // Confirm that the strike excites the normal-size plate and remains finite.
    plate.strike();
    double energy = 0.0;
    for (int i = 0; i < 48000; ++i)
    {
        const auto sample = plate.processSample();
        assert(std::isfinite(sample));
        energy += std::abs(sample);
    }
    assert(energy > 0.0);

    // The current low-order modal basis cannot yet represent the audible high-order
    // modes of a 400 m plate. This explicit zero-mode result is a known v0.1 limit.
    p.widthMetres = 400.0;
    p.heightMetres = 250.0;
    plate.setParameters(p);
    const auto hugeFundamental = plate.getFundamentalHz();
    assert(hugeFundamental == 0.0);

    p.widthMetres = 0.8;
    p.heightMetres = 0.5;
    p.thicknessMetres = 0.2;
    plate.setParameters(p);
    assert(!plate.isPlausible());
    std::cout << "PlateModel tests passed. Default f(1,1): " << normalFundamental
              << " Hz; 4m x 2m: " << largerFundamental
              << " Hz; 400m x 250m: outside v0.1 modal range\n";
}
