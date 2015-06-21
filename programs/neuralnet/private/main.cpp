#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <random>
#include <map>
#include <boost/dynamic_bitset.hpp>

std::default_random_engine generator(std::random_device{}());

enum class Type
{
    NUMBER,
    OPERATOR
};

enum class Operator
{
    PLUS = 0,
    MINUS,
    MULTIPLY,
    DIVIDE
};

struct Gene
{
    Type m_type;
    size_t m_number;
    Operator m_operator;
};

typedef std::vector<size_t> Sample;
typedef std::vector<Gene> Genotype;

Genotype parseGenotype(const std::vector<size_t>& genotype)
{
    std::vector<Gene> parsedGenotype;

    // Parse genotype
    // For each gene (4 bits)
    for (auto& gene : genotype)
    {
        // If this gene is a valid number (0-9)
        if (gene < 10)
        {
            // If there are no values or the last value is an operator, push back the number
            if (parsedGenotype.size() == 0 || parsedGenotype.back().m_type == Type::OPERATOR)
            {
                Gene parsedGene;
                parsedGene.m_type = Type::NUMBER;
                parsedGene.m_number = gene;

                parsedGenotype.push_back(parsedGene);
            }
        }
        // If this is an operator (1010 - 1101)
        else if (gene < 14)
        {
            // If there are values and the last value is a number, push the operator
            if (parsedGenotype.size() != 0 && parsedGenotype.back().m_type == Type::NUMBER)
            {
                Gene parsedGene;
                parsedGene.m_type = Type::OPERATOR;
                parsedGene.m_operator = static_cast<Operator>(gene - 10);

                parsedGenotype.push_back(parsedGene);
            }
        }
    }

    return parsedGenotype;
};

double objective(const Genotype& parsedGenotype)
{
    // Evaluate function for genotype
    double evaluatedGenotype = 0.0;
    if (parsedGenotype.size() > 0)
    {
        evaluatedGenotype = static_cast<double>(parsedGenotype.front().m_number);
        Operator lastOper;

        for (int i = 1; i < parsedGenotype.size(); ++i)
        {
            auto& gene = parsedGenotype[i];

            if (gene.m_type == Type::OPERATOR)
            {
                lastOper = gene.m_operator;
            }
            else
            {
                switch (lastOper)
                {

                case Operator::PLUS:
                    evaluatedGenotype += gene.m_number;
                    break;

                case Operator::MINUS:
                    evaluatedGenotype -= gene.m_number;
                    break;

                case Operator::MULTIPLY:
                    evaluatedGenotype *= gene.m_number;
                    break;

                case Operator::DIVIDE:
                    evaluatedGenotype /= gene.m_number;
                    break;

                }
            }
        }
    }

    return evaluatedGenotype;
}

double fitness(double objectiveValue, double target)
{
    double fitness;

    // If objectiveValue is 0 or indeterminate (n != n)
    if (objectiveValue == 0.0 || objectiveValue != objectiveValue)
        fitness = 0.0;
    else
        fitness = 1.0 / fabs(target - objectiveValue);

    return fitness;
};

// https://en.wikipedia.org/wiki/Selection_(genetic_algorithm)
std::vector<Sample> select(const std::multimap<double, Sample>& map, int count)
{
    if (count < 0)
        throw;

    // Copy map
    std::multimap<double, Sample> normalisedFitness;
    std::multimap<double, Sample> accumulatedFitness;

    // Normalise fitness values
    // Sum fitness values
    double fitnessTotal = 0.0;
    for (auto& genotype : map)
    {
        fitnessTotal += genotype.first;
    }

    // Divide by total fitness
    for (auto& genotype : map)
    {
        auto value = std::pair<double, Sample>(genotype.first / fitnessTotal, genotype.second);
        normalisedFitness.insert(value);
    }

    // Compute accumulated fitness
    double lastFitness = 0.0;
    for (auto& genotype : normalisedFitness)
    {
        auto fitness = genotype.first + lastFitness;
        auto value = std::pair<double, Sample>(fitness, genotype.second);
        accumulatedFitness.insert(value);
        lastFitness = fitness;
    }

    // Select random samples
    std::vector<Sample> data;

    for (int i = 0; i < count; ++i)
    {
        const std::uniform_real_distribution<double> distribution(0.0, 1.0);

        // Generate random number
        double n = distribution(generator);

        // Pick first sample with accumulated fitness >= n
        auto greater = [n](std::pair<const double, Sample>& sample)
        {
            return sample.first >= n;
        };

        auto it = std::find_if(accumulatedFitness.begin(), accumulatedFitness.end(), greater);
        data.push_back(it->second);
    }

    return data;
}

int main(int argc, char** argv)
{
    // Target number
    const int TARGET_NUMBER = 42;

    // Population size
    const int POPULATION_SIZE = 100;

    // Sample genotypes
    std::vector<Sample> samples;

    // Generate samples
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        const std::uniform_int_distribution<int> geneValueDistribution(0, 15);

        // Genotype length - 10 genes
        const int length = 10;
        std::vector<size_t> genotype;

        // Generate samples
        for (int j = 0; j < length; ++j)
        {
            auto rng = geneValueDistribution(generator);

            genotype.push_back(rng & 0xF);
        }

        samples.push_back(genotype);
    }

    for (int i = 0; i < 10; ++i)
    {
        // Calculate scores
        std::multimap<double, Sample> scores;

        for (auto& genotype : samples)
        {
            // Parse genotype
            auto parsedGenotype = parseGenotype(genotype);

            // Evaluate objective function
            auto objectiveValue = objective(parsedGenotype);

            // Evaluate fitness
            auto fitnessValue = fitness(objectiveValue, TARGET_NUMBER);

            // Add to score map
            scores.insert(std::pair<double, Sample>(fitnessValue, genotype));
        }

        // Check for solution
        const auto& last = scores.rbegin();
        if (last->first == std::numeric_limits<double>::infinity())
        {
            printf("Found solution %f\n", objective(parseGenotype(last->second)));
        }
        else
        {
            printf("Best value: %f\n", objective(parseGenotype(last->second)));

            // Select members for next iteration
            samples = select(scores, 2);

            int t = 0;
        }
    }

    system("pause");
}