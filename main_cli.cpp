#include "MapReduceController.h"
#include "P3_Logger.h"

#include <iostream>
#include <string>
#include <thread>

int main(int argc, char** argv)
{
    // --------- Parse CLI arguments ----------
    // arg1: input directory  (defaults to "sample_input")
    // arg2: output file path (defaults to "output/word_counts_cli.txt")
    // arg3: number of worker threads (optional, defaults to hardware_concurrency or 4)
    std::string inputDir   = (argc > 1) ? argv[1] : "sample_input";
    std::string outputFile = (argc > 2) ? argv[2] : "output/word_counts_cli.txt";

    unsigned int defaultWorkers =
        std::max(4u, std::thread::hardware_concurrency());  // at least 4

    unsigned int workers = defaultWorkers;
    if (argc > 3) {
        try {
            unsigned long parsed = std::stoul(argv[3]);
            if (parsed >= 3) {
                workers = static_cast<unsigned int>(parsed);
            }
        } catch (...) {
            // keep default if parsing fails
        }
    }

    Logger logger;
    logger.log("CLI MapReduce starting...");
    logger.log("Input directory: " + inputDir);
    logger.log("Output file: " + outputFile);
    logger.log("Worker threads: " + std::to_string(workers));

    try {
        MapReduceController controller(inputDir, outputFile, workers);
        bool ok = controller.run(logger);

        if (!ok) {
            std::cerr << "No .txt files found in: " << inputDir << std::endl;
            return 1;
        }

        logger.log("CLI MapReduce finished successfully.");
        std::cout << "MapReduce completed. Results written to: "
                  << outputFile << std::endl;
    }
    catch (const std::exception& ex) {
        logger.log(std::string("Unhandled exception: ") + ex.what());
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }
    catch (...) {
        logger.log("Unknown exception in CLI.");
        std::cerr << "Unknown error occurred." << std::endl;
        return 1;
    }

    return 0;
}
