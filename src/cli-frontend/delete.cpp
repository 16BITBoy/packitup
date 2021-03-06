#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>

#include "../piu/piu.hpp"

using namespace PIU;
namespace poptions = boost::program_options;

void printFileInfo(FileInfo file) {
    std::cout << file.filePath << " : " << file.fileSize << std::endl;
}

int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{

        poptions::options_description desc("Program options");
        desc.add_options()
                ("help",                                                          "Prints help information")
                ("package,p",      poptions::value< std::string >()             , "Package file")
                ("delete-files,d", poptions::value< std::vector<std::string> >(), "Files to delete from the package");

        poptions::positional_options_description p;
        p.add("delete-files", -1);

        poptions::variables_map vm;
        poptions::store(poptions::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        poptions::notify(vm);

        if(vm.count("help")) {
            std::cout << "Description: Deletes one or more files from the specified PackItUp package file." << std::endl;
            std::cout << "Usage: "+std::string(argv[0])+" -p PACKAGE_FILE FILES..." << std::endl;
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

        if(vm.count("delete-files")) {
            files = vm["delete-files"].as< std::vector<std::string> >();
        } else {
            std::cerr << "No input files specified." << std::endl;
            return EXIT_FAILURE;
        }

        ar = new PIUArchive(package);

        std::vector<std::string>::iterator fileIt = files.begin();

        for(; fileIt != files.end(); fileIt++) {
            std::string file = *fileIt;
            boost::filesystem::path filePath(file);
            if(boost::filesystem::exists(filePath)) {
                ar->deleteFile(file);
            } else {
                std::cerr << "[Warning] " << file << " doesn't exist in this package." << std::endl;
            }
        }

        std::vector<FileInfo> filesInside = ar->listFiles();
        std::cout << "There are : " << filesInside.size() << " files." << std::endl;
        for_each(filesInside.begin(), filesInside.end(), printFileInfo);

        ar->write();
        return EXIT_SUCCESS;
    }
    catch(std::runtime_error &e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }



}

