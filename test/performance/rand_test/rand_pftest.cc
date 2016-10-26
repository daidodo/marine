#include <random>
#include "../pf_inc.h"

double g;

PFTEST(Rand, old)
{
    double r = double(rand() % 10000) / 10000;
    if(r < 0.5)
        g += r;
}

std::default_random_engine generator;
std::uniform_real_distribution<double> distribution(0.0, 1.0);

PFTEST(Rand, new)
{
    double r = distribution(generator);
    if(r < 0.5)
        g += r;
}
