Genesis-VIII
============

A simulation package to generate quantitative risk models for time series data using genetic programming.

## Summary

Genesis-VIII is a software tool that I wrote in 2008 (hence the VIII) for my MSc thesis in statistics & machine learning, which was on the subject of genetic programming.

The software provides generic classes for performing genetic programming in C++. It can be interactively controlled using an embedded Lua interpreter.

The base classes *Population*, *Program*, *Tree*, *Node*, *Leaf* and *Branch* can be easily subclassed to implement new types of genetic programs and evolution schemes.

## Specific genetic programs

Currently, the package contains genetic programs for modeling the following quantities:

* The Value at Risk (VaR) of a discrete time series.
* The Expected Shortfall (ES) of a discrete time series.
* The copula model of a bivariate sample of discrete time series data.

To get you started, example scripts and stock market data is included.

## Example

As an example, the following code will evolve a model for the bivariate copula of the returns of the Dow Jones Industrial Index (DOW) and the Deutsche Aktienindex (DAX), using 50 rounds of evolution with tournament selection and a given mutation, crossover and shrink rate:

```Lua
p=pop.new();

dir="data/copula_test_new";

pop.output_dir(p,dir);
logger.reopen(dir);

print("Mutation rate:",pop.mutation_rate(p,0.05),"\n");
print("Shrink rate:",pop.shrink_rate(p,0.025),"\n");
print("Crossover size:",pop.crossover_size(p,0.2),"\n");
print("Tournament size:",pop.tournament_size(p,6),"\n");

pop.create(p,1000,10,program.copula,"data/stocks/dow.csv","data/stocks/dax.csv",6,",");
pop.run(p,50);

```

## Terms of Usage

The code (except the numerical recipes snippets that were used and the Lua interface library code) can be freely reused. Feel free to include a reference to this code if you reuse it in your own project.

## Further Reading

For more information on this tool, check out my MSc thesis in statistics at http://www.andreas-dewes.de/en/publications . If you have any questions or comments about this code, feel free to drop me a line (contact details can be found on my web site).
