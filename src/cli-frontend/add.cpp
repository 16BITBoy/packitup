#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>

#include "../piu/piu.hpp"

using namespace PIU;
namespace poptions = boost::program_options;

void printFileInfo(FileInfo file) {
    std::cout << file.filePath << " : " << file.fileSize << std::endl;
}

int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{
//        if(argc < 3) {
//            std::cerr << "Missing parameters" << std::endl;
//            return EXIT_FAILURE;
//        }

        poptions::options_description desc("Program options");
        desc.add_options()
                ("help",                                                         "Prints help information")
                ("package,p",     poptions::value< std::string >()             , "Package file")
                ("input-files,f", poptions::value< std::vector<std::string> >(), "Files to add to the package");

        poptions::positional_options_description p;
        p.add("input-files", -1);

        poptions::variables_map vm;
        poptions::store(poptions::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        poptions::notify(vm);

        if(vm.count("help")) {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        std::string package;
        std::vector<std::string> files;

        if(vm.count("package")) {
            package = vm["package"].as<std::string>();
        } else {
            std::cerr << "Package file not specified. Use --package to specify it." << std::endl;
            return EXIT_FAILURE;
        }

        if(vm.count("input-files")) {
            files = vm["input-files"].as< std::vector<std::string> >();
        } else {
            std::cerr << "No input files specified." << std::endl;
            return EXIT_FAILURE;
        }

        ar = new PIUArchive(package);

        std::vector<std::string>::iterator fileIt = files.begin();

        for(; fileIt != files.end(); fileIt++) {
            std::string file = *fileIt;
            ar->add(file);
        }

        std::vector<FileInfo> filesInside = ar->listFiles();
        std::cout << "There are : " << filesInside.size() << " files." << std::endl;
        for_each(filesInside.begin(), filesInside.end(), printFileInfo);

        ar->write();
        return EXIT_SUCCESS;
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }



}
