#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <exception>
#include "CLI11.hpp"
#include "CircuitBoard.h"
#include "CircuitUtils.h"
#include <unordered_map>
#include <iomanip>
#include <locale>

struct CommaSeparator : std::numpunct<char> {
    char do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};


int main(int argc, char *argv[]){
    CLI::App CLIApp{"Verify Board"};
    std::string InFilename, OutFilename = "out.html";
    argv = CLIApp.ensure_utf8(argv);
    CLIApp.add_option("-i,--input", InFilename, "Netlist input file.")->required();
    CLI11_PARSE(CLIApp, argc, argv);

    CCircuitBoard Board;
    if(!CCircuitUtils::LoadBoard(InFilename,Board)){
        std::cerr<<"Could not load board '"<<InFilename<<"'!"<<std::endl;
        return EXIT_FAILURE;
    }
    std::vector<std::vector<std::pair<TCircuitName,TCircuitName>>> LayerShortedNets;
    std::vector<std::vector<TCircuitName>> LayerBrokenNets;
    bool Errors = false;
    for(std::size_t LayerIndex = 0; LayerIndex < Board.LayerCount(); LayerIndex++){
        CCircuitLayer Layer;
        if(!Board.Layer(LayerIndex,Layer)){
            std::cerr<<"Could get layer "<<LayerIndex<<"!"<<std::endl;
            return EXIT_FAILURE;
        }
        std::vector<std::pair<TCircuitName,TCircuitName>> ShortedNets;
        std::vector<TCircuitName> BrokenNets;
        Layer.GetShortedNets(ShortedNets);
        Errors = Errors || !ShortedNets.empty();
        LayerShortedNets.push_back(ShortedNets);
        Layer.GetBrokenNets(BrokenNets);
        Errors = Errors || !BrokenNets.empty();
        LayerBrokenNets.push_back(BrokenNets);
    }
    if(Errors){
        std::cout<<"Board '"<<InFilename<<"' has the following errors!"<<std::endl;
        std::string Line(InFilename.length()+34,'-');
        for(std::size_t LayerIndex = 0; LayerIndex < Board.LayerCount(); LayerIndex++){
            if(!LayerShortedNets[LayerIndex].empty() || !LayerBrokenNets[LayerIndex].empty()){
                std::cout<<Line<<std::endl;
                std::cout<<"Layer "<<LayerIndex<<":"<<std::endl;
                if(!LayerBrokenNets[LayerIndex].empty()){
                    std::cout<<"  Broken Nets :";
                    for(auto &Circuit : LayerBrokenNets[LayerIndex]){
                        std::cout<<" '"<<Circuit<<"'";
                    }
                    std::cout<<std::endl;
                }
                if(!LayerShortedNets[LayerIndex].empty()){
                    std::cout<<"  Shorted Nets:"<<std::endl;
                    for(auto &ShortedPair : LayerShortedNets[LayerIndex]){
                        std::cout<<"    "<<std::get<0>(ShortedPair)<<" - "<<std::get<1>(ShortedPair)<<std::endl;
                    }
                }
            }
        }
    }
    else{
        std::cout<<"Board '"<<InFilename<<"' has no broken or shorted nets!"<<std::endl;
    }

    return EXIT_SUCCESS;
}
