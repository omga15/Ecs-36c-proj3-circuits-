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

std::string StringStrip(const std::string &str) noexcept;

CCircuitNetlist LoadNetlist(const std::string &filename);
void SaveCircuitBoard(const CCircuitBoard &board, const std::string &filename, const std::string &title);

int main(int argc, char *argv[]){
    CLI::App CLIApp{"Route Netlist"};
    std::string InFilename, OutFilename = "out.circ";
    bool Reroute = false;
    argv = CLIApp.ensure_utf8(argv);
    CLIApp.add_option("-i,--input", InFilename, "Netlist input file.")->required();
    CLIApp.add_option("-o,--output", OutFilename, "Output circuit file.");
    CLIApp.add_flag("-r,--reroute", Reroute, "Reroute and optimize layers.");
    CLI11_PARSE(CLIApp, argc, argv);

    CCircuitNetlist Netlist;
    if(!CCircuitUtils::LoadNetlist(InFilename,Netlist)){
        std::cerr<<"Could not load netlist '"<<InFilename<<"'!"<<std::endl;
        return EXIT_FAILURE;
    }
    auto Start = std::chrono::steady_clock::now();
    auto Board = CCircuitBoard::CreateBoardFromNetlist(Netlist);
    auto CreateNetlist = std::chrono::steady_clock::now();
    Board.CreateLayerNets();
    auto CreateLayerNets = std::chrono::steady_clock::now();
    Board.MinimizeLayers();
    auto MinimizeLayers = std::chrono::steady_clock::now();
    if(Reroute){
        Reroute = Board.RerouteAndOptimizeLayers();
    }
    auto RerouteEnd = std::chrono::steady_clock::now();
    
    ;
    auto LastSlash = InFilename.rfind("/");
    LastSlash = LastSlash == std::string::npos ? 0 : LastSlash + 1;
    CCircuitUtils::SaveBoard(OutFilename, Board);

    auto CreateNetlistDuration =  std::chrono::duration_cast<std::chrono::nanoseconds>(CreateNetlist-Start);
    auto CreateLayerNetsDuration =  std::chrono::duration_cast<std::chrono::nanoseconds>(CreateLayerNets-CreateNetlist);
    auto MinimizeLayersDuration =  std::chrono::duration_cast<std::chrono::nanoseconds>(MinimizeLayers-CreateLayerNets);
    auto RerouteDuration  =  std::chrono::duration_cast<std::chrono::nanoseconds>(RerouteEnd-MinimizeLayers);
    auto TotalDuration =  std::chrono::duration_cast<std::chrono::nanoseconds>(RerouteEnd-Start);

    std::string SeparatorLine(InFilename.length()+OutFilename.length() + 37,'-');
    std::cerr.imbue(std::locale(std::cerr.getloc(), new CommaSeparator));
    std::cerr<<"Processed input file '"<<InFilename<<"' -> output '"<<OutFilename<<"'"<<std::endl;
    std::cerr<<SeparatorLine<<std::endl;
    std::cerr<<"Net Count         : "<<Netlist.CircuitCount()<<std::endl;
    std::cerr<<"Layer Count       : "<<Board.LayerCount()<<std::endl;
    std::cerr<<SeparatorLine<<std::endl;
    std::cerr<<"Board From Netlist: "<<CreateNetlistDuration.count()<<"ns"<<std::endl;
    std::cerr<<"Route Layers      : "<<CreateLayerNetsDuration.count()<<"ns"<<std::endl;
    std::cerr<<"Minimize Layers   : "<<MinimizeLayersDuration.count()<<"ns"<<std::endl;
    if(Reroute){
        std::cerr<<"Reroute & Optimize: "<<RerouteDuration.count()<<"ns"<<std::endl;
    }
    std::cerr<<SeparatorLine<<std::endl;
    std::cerr<<"Total Time        : "<<TotalDuration.count()<<"ns"<<std::endl;
    return EXIT_SUCCESS;
}
