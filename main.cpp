#include "PerformanceProfiler.h"

int main(int argc, char *argv[])
{
    std::string mark = argv[1];
    if (mark == "performance")
    {
        Converter *converter = new Converter(argv[2], argv[3]);
        converter->writeSummary();
    }
    else if (mark == "show")
    {
        Converter *converter = new Converter(argv[2], atoi(argv[3]));
        converter->showSpecificdata(std::atoi(argv[3]));
    }
    else
    {
        std::cout << "the number of the arguments is not correct." << std::endl;
    }
}
