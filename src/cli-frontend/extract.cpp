#include <iostream>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>
#include <boost/program_options.hpp>

#include "../piu/piu.hpp"

using namespace PIU;
namespace poptions = boost::program_options;

int main(int argc, char **argv){
    PIUArchive *ar = NULL;
    try{

        poptions::options_description desc("Program options");
        desc.add_options()
                ("help",                                                           "Prints help information")
                ("package,p",       poptions::value< std::string >()             , "Package file")
                ("extract-files,f", poptions::value< std::vector<std::string> >(), "Files to extract from the package")
                ("extract-all-files,a",                                            "Extracts all files inside the specified package");

        poptions::positional_options_description p;
        p.add("extract-files", -1);

        poptions::variables_map vm;
        poptions::store(poptions::command_line_parser(argc, argv).
                  options(desc).positional(p).run(), vm);
        poptions::notify(vm);

        std::string package;
        std::vector<std::string> files;

        if(vm.count("help")) {
            std::cout << desc << std::endl;
            return EXIT_SUCCESS;
        }

        if(vm.count("package")) {
            package = vm["package"].as<std::string>();
        } else {
            std::cerr << "Package not specified. Please specify one by adding --package option." << std::endl;
            return EXIT_FAILURE;
        }

        boost::filesystem::path packagePath(package.c_str());
        if(!boost::filesystem::exists(packagePath)) {
            std::cerr << "Package file \""+package+"\" doesn't exist." << std::endl;
            return EXIT_FAILURE;
        }

        ar = new PIUArchive(package);

        if(vm.count("extract-files")) {
            files = vm["extract-files"].as< std::vector<std::string> >();
        } else if(vm.count("extract-all-files")) {
            std::vector<FileInfo> filesInfo = ar->listFiles();
            for(std::vector<FileInfo>::iterator it = filesInfo.begin(); it != filesInfo.end(); it++) {
                FileInfo info = *it;
                files.push_back(info.filePath);
            }
        } else {
            std::cerr << "Files to extract not specified." << std::endl;
            return EXIT_FAILURE;
        }


        for(std::vector<std::string>::iterator fileIt = files.begin(); fileIt != files.end(); fileIt++) {
            std::string filePath = *fileIt;
            std::cout << "Extracting "+filePath << std::endl;
            ar->extractFile(filePath);
        }

        return EXIT_SUCCESS;
    }
    catch(UndefinedException e){
        std::cout << e.what() << std::endl;
        return EXIT_FAILURE;
    }



}

