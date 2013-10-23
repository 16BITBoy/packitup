#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <boost/program_options.hpp>

#include "../piu/piu.hpp"

using namespace PIU;
namespace poptions = boost::program_options;

// Shows all the information about the PIU file on screen
void showPIUFileInformation(std::string file) throw (UndefinedException) {
    PIUArchive *ar = NULL;
    ar = new PIUArchive(file);
    std::vector<FileInfo> fileList = ar->listFiles();
    int i;
    std::cout << "File information read from PIU archive:" << std::endl;
    std::cout << "File list size: " << ar->getFileListSize() << std::endl;
    for(i = 0; i < fileList.size(); i++){
        std::cout << fileList[i].filePath << " : "
                  << fileList[i].fileSize << std::endl;
    }

}

int main(int argc, char **argv) {
    try {
        poptions::options_description desc("Program options");
        desc.add_options()
                ("help",                                      "Prints help information")
                ("package,p", poptions::value<std::string>(), "Package file to view");

        poptions::positional_options_description p;
        p.add("package", -1);

        poptions::variables_map vm;
        poptions::store(poptions::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        poptions::notify(vm);

        if(vm.count("help")) {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        if(vm.count("package")) {
            showPIUFileInformation(vm["package"].as<std::string>());
            return EXIT_SUCCESS;
        } else {
            std::cerr << "No package file specified." << std::endl;
            return EXIT_FAILURE;
        }

    }
    catch(UndefinedException &e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}

