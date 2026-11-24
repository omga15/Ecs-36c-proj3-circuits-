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

const std::string GBaseHTML =   "<!DOCTYPE html>\n"
                                "<html lang=\"en\">\n"
                                "<head>\n"
                                "<meta charset=\"UTF-8\">\n"
                                "<title>{{TITLE}}</title>\n"
                                "\n"
                                "<style>\n"
                                "    body {\n"
                                "        font-family: sans-serif;\n"
                                "        margin: 20px;\n"
                                "    }\n"
                                "\n"
                                "    .layout {\n"
                                "        display: flex;\n"
                                "        gap: 20px;\n"
                                "        align-items: flex-start;\n"
                                "    }\n"
                                "\n"
                                "    #controls {\n"
                                "        width: 180px;\n"
                                "        flex-shrink: 0;\n"
                                "        display: flex;\n"
                                "        flex-direction: column;\n"
                                "        gap: 12px;\n"
                                "    }\n"
                                "    .sub-layers {\n"
                                "        display: flex;\n"
                                "        flex-direction: column;\n"
                                "        margin-left: 24px;\n"
                                "        padding-left: 8px;\n"
                                "        border-left: 2px solid #ccc;\n"
                                "    }\n"
                                "\n"
                                "    .sub-layers label {\n"
                                "        font-size: 0.9em;\n"
                                "    }\n"
                                "\n"
                                "    #viewer {\n"
                                "        flex: 1;\n"
                                "        min-width: 0;\n"
                                "        border: 1px solid #ccc;\n"
                                "    }\n"
                                "\n"
                                "    #viewer svg {\n"
                                "        display: block;\n"
                                "        width: 100%;\n"
                                "        height: auto;\n"
                                "    }\n"
                                "    .layer-header {\n"
                                "        display: flex;\n"
                                "        align-items: center;\n"
                                "        justify-content: space-between;\n"
                                "        width: 100%;\n"
                                "    }\n"
                                "\n"
                                "    .layer-header label {\n"
                                "        cursor: pointer;\n"
                                "    }\n"
                                "\n"
                                "    .expand-button {\n"
                                "        border: none;\n"
                                "        background: transparent;\n"
                                "        cursor: pointer;\n"
                                "        font-size: 12px;\n"
                                "        padding: 4px 8px;\n"
                                "    }\n"
                                "\n"
                                "    .expand-button:hover {\n"
                                "        color: #007bff;\n"
                                "    }\n"
                                ".disabled {\n"
                                "    opacity: 0.4;\n"
                                "    pointer-events: none;\n"
                                "    cursor: not-allowed;\n"
                                "}\n"
                                "</style>\n"
                                "</head>\n"
                                "\n"
                                "<body>\n"
                                "\n"
                                "<h2>{{TITLE}}</h2>\n"
                                "\n"
                                "<div class=\"layout\">\n"
                                "\n"
                                "    <!-- Left sidebar -->\n"
                                "    <div id=\"controls\">\n"
                                "        <strong>Layers</strong>\n"
                                "        {{LAYER_NAMES}}\n"
                                "    </div>\n"
                                "\n"
                                "    <!-- SVG viewer -->\n"
                                "    <div id=\"viewer\">\n"
                                "        <svg viewBox=\"0 0 {{WIDTH}} {{HEIGHT}}\" xmlns=\"http://www.w3.org/2000/svg\">\n"
                                "            <defs>\n"
                                "              <mask id=\"pinholes\">\n"
                                "                <!-- White = visible -->\n"
                                "                <rect width=\"{{WIDTH}}\" height=\"{{HEIGHT}}\" fill=\"white\"/>\n"
                                "\n"
                                "                <!-- Black = transparent holes -->\n"
                                "                {{PINHOLES}}\n"
                                "              </mask>\n"
                                "            </defs>\n"
                                "            <g>\n"
                                "                <rect width=\"{{WIDTH}}\" height=\"{{HEIGHT}}\" fill=\"#fff\"/>\n"
                                "            </g>\n"
                                "            {{LAYERS}}\n"
                                "        </svg>\n"
                                "    </div>\n"
                                "\n"
                                "</div>\n"
                                "\n"
                                "<script>\n"
                                "function toggleLayer(id, visible) {\n"
                                "    document.getElementById(id).style.display = visible ? \"inline\" : \"none\";\n"
                                "    const sublayer = document.getElementById(id + '-sublayer')\n"
                                "    if(sublayer){"
                                "       if(visible){\n"
                                "           sublayer.classList.remove('disabled');    // Disable\n"
                                "       }\n"
                                "       else{\n"
                                "           sublayer.classList.add('disabled');    // Enable\n"
                                "       }\n"
                                "    }\n"
                                "}\n"
                                "function toggleSublayer(id, button) {\n"
                                "    const sublayer = document.getElementById(id);\n"
                                "    const isExpanded = sublayer.style.display !== 'none';\n"
                                "    sublayer.style.display = isExpanded ? 'none' : 'flex';\n"
                                "    button.innerHTML = isExpanded ? '&#9660;' : '&#9650;';\n"
                                "    button.setAttribute('aria-expanded', !isExpanded);\n"
                                "}\n"
                                "</script>\n"
                                "\n"
                                "</body>\n"
                                "</html>\n";

const std::string GBaseLabel = "<label><input type=\"checkbox\" checked onchange=\"toggleLayer('{{LAYER_ID}}', this.checked)\">{{LAYER_NAME}}</label>";
const std::string GBaseGroup = "<g id=\"{{GROUP_ID}}\">\n{{GROUP_BLOCK}}</g>";
const std::string GBaseCircle = "<circle cx=\"{{CX}}\" cy=\"{{CY}}\" r=\"{{R}}\" fill=\"{{FILL}}\"/>";
const std::string GBaseRectangle = "<rect x=\"{{X}}\" y=\"{{Y}}\" width=\"{{WIDTH}}\" height=\"{{HEIGHT}}\" fill=\"{{FILL}}\"/>";
const std::string GBaseRectangleMask = "<rect x=\"{{X}}\" y=\"{{Y}}\" width=\"{{WIDTH}}\" height=\"{{HEIGHT}}\" fill=\"{{FILL}}\" mask=\"{{MASK}}\"/>";
const std::string GBasePolyline = "<polyline points=\"{{POINTS}}\" fill=\"{{FILL}}\" stroke=\"{{STROKE}}\" stroke-width=\"{{WIDTH}}\" stroke-linejoin=\"round\" />";
const std::string GBaseLayerTitle = "Layer {{LAYER_INDEX}}";
const std::string GBaseDiv = "<div class=\"{{CLASS}}\">\n{{GROUP_BLOCK}}</div>";
const std::string GBaseDivWithID = "<div id=\"{{GROUP_ID}}\" class=\"{{CLASS}}\"{{STYLE}}>\n{{GROUP_BLOCK}}</div>";
const std::string GBaseButton = "<button class=\"expand-button\" onclick=\"toggleSublayer('{{GROUP_ID}}-sublayer', this)\" aria-expanded=\"false\">&#9660;</button>";
const std::string GCLASS = "{{CLASS}}";
const std::string GTITLE = "{{TITLE}}";
const std::string GGROUP_ID = "{{GROUP_ID}}";
const std::string GLAYERS = "{{LAYERS}}";
const std::string GLAYER_ID = "{{LAYER_ID}}";
const std::string GLAYER_NAME = "{{LAYER_NAME}}";
const std::string GLAYER_NAMES = "{{LAYER_NAMES}}";
const std::string GGROUP_BLOCK = "{{GROUP_BLOCK}}";
const std::string GSTYLE = "{{STYLE}}";
const std::string GWIDTH = "{{WIDTH}}";
const std::string GHEIGHT = "{{HEIGHT}}";
const std::string GPOINTS = "{{POINTS}}";
const std::string GCX = "{{CX}}";
const std::string GCY = "{{CY}}";
const std::string GR = "{{R}}";
const std::string GX = "{{X}}";
const std::string GY = "{{Y}}";
const std::string GFILL = "{{FILL}}";
const std::string GSTROKE = "{{STROKE}}";
const std::string GPINHOLES = "{{PINHOLES}}";
const std::string GMASK = "{{MASK}}";
const std::string GLAYER_INDEX = "{{LAYER_INDEX}}";
const std::string GColorBlack = "black";
const std::string GColorWhite = "white";
const std::string GColorRed = "#FF2C2C";
const std::string GColorOrange= "#E69F00";
const std::string GColorSkyBlue = "#56B4E9";
const std::string GColorBluishGreen = "#009E73";
const std::string GColorVermillion = "#D55E00";
const std::string GColorReddishPurple = "#CC79A7";
const std::string GColorBlue = "#0072B2";
const std::string GColorYellow = "#F0E442";
const std::string GColorPurple = "#9B72CF";
const std::string GColorDarkCharcoal = "#202124";
const std::string GColorNone = "none";
const std::string GMaskPinholes = "url(#pinholes)";
const std::string GLayerGroup = "layer-group";
const std::string GSubLayers = "sub-layers";

const int GIndentSpaces = 4;
const int GCircuitScaling = 5;
const int GPinKeepout = 4;
const int GPinExterior = 2;
const int GPinInterior = 1;
const int GTraceWidth = 2;
const int GTraceCap = 2;

struct CommaSeparator : std::numpunct<char> {
    char do_thousands_sep() const override { return ','; }
    std::string do_grouping() const override { return "\3"; }
};

void SaveCircuitBoard(const CCircuitBoard &board, const std::string &filename, const std::string &title);

int main(int argc, char *argv[]){
    CLI::App CLIApp{"Render Board"};
    std::string InFilename, OutFilename = "out.html";
    argv = CLIApp.ensure_utf8(argv);
    CLIApp.add_option("-i,--input", InFilename, "Netlist input file.")->required();
    CLIApp.add_option("-o,--output", OutFilename, "Output HTML file.");
    CLI11_PARSE(CLIApp, argc, argv);

    CCircuitBoard Board;
    if(!CCircuitUtils::LoadBoard(InFilename,Board)){
        std::cerr<<"Could not load board '"<<InFilename<<"'!"<<std::endl;
        return EXIT_FAILURE;
    }
    auto LastSlash = InFilename.rfind("/");
    LastSlash = LastSlash == std::string::npos ? 0 : LastSlash + 1;
    SaveCircuitBoard(Board,OutFilename,InFilename.substr(LastSlash,InFilename.rfind(".")-LastSlash));
    std::cerr<<"Processed input file '"<<InFilename<<"' -> output '"<<OutFilename<<"'"<<std::endl;
    return EXIT_SUCCESS;
}

SCircuitLocation ScaleLocation(const SCircuitLocation &location){
    return {location.DX * GCircuitScaling,location.DY *GCircuitScaling};
}

std::string IndentString(int indent){
    return std::string(indent,' ');
}

std::string BlockIndent(const std::string &block, int indent){
    return StringReplace(block,"\n",std::string("\n") + IndentString(indent));
}

std::string IDFromName(const std::string &name){
    return StringReplace(StringLower(name)," ","");
}

int GetExpectedIndent(const std::string &base, const std::string &tag){
    auto TagIndex = base.find(tag);
    auto LastNewline = base.rfind("\n",TagIndex);
    return TagIndex - LastNewline;
}

std::string CreateDiv(const std::string &divclass, const std::string &block, const std::string &id = "", const std::string &style = ""){
    std::string Style = style.empty() ? style : std::string(" style=\"") + style + "\"";
    return StringReplace(id.empty() ? GBaseDiv : GBaseDivWithID,{{GCLASS,divclass},{GGROUP_BLOCK,BlockIndent(block,GIndentSpaces)},{GGROUP_ID,IDFromName(id)+"-sublayer"}, {GSTYLE,Style}} );
}

std::string CreateGroup(const std::string &id, const std::string &block){
    return StringReplace(GBaseGroup,{{GGROUP_ID,id,},{GGROUP_BLOCK,BlockIndent(block,GIndentSpaces)}});
}

std::string CreateLabel(const std::string &label){
    return StringReplace(GBaseLabel,{{GLAYER_ID,IDFromName(label)}, {GLAYER_NAME,label}}) + "\n";
}

std::string CreateButton(const std::string &id){
    return StringReplace(GBaseButton,{{GGROUP_ID,IDFromName(id)}}) + "\n";
}

std::string CreateLabels(const std::vector<std::vector<std::string>> &labelgroups){
    std::string ReturnString;
    for(auto &LabelGroup : labelgroups){
        if(LabelGroup.size() == 1){
            ReturnString += CreateLabel(LabelGroup[0]);
        }
        else{
            auto LabelGroupString = CreateLabel(LabelGroup[0]);
            std::string SubLabels;
            LabelGroupString += CreateButton(LabelGroup[0]);
            for(std::size_t Index = 1; Index < LabelGroup.size(); Index++){
                SubLabels += CreateLabel(LabelGroup[Index]);
            }
            LabelGroupString += CreateDiv(GSubLayers,SubLabels,LabelGroup[0],"display: none");
            ReturnString += CreateDiv(GLayerGroup,LabelGroupString);
        }
    }
    
    return ReturnString;
}

std::string CreateCircle(const SCircuitLocation &location, int radius, const std::string &color){

    std::unordered_map<std::string,std::string> Context = { {GCX,std::to_string(location.DX)},
                                                            {GCY,std::to_string(location.DY)},
                                                            {GR,std::to_string(radius)},
                                                            {GFILL,color}};
    return StringReplace(GBaseCircle,Context);
}

std::string CreateRectangle(const SCircuitLocation &location, int width, int height, const std::string &color, const std::string &mask){
    std::unordered_map<std::string,std::string> Context = { {GX,std::to_string(location.DX)},
                                                            {GY,std::to_string(location.DY)},
                                                            {GWIDTH,std::to_string(width)},
                                                            {GHEIGHT,std::to_string(height)},
                                                            {GFILL,color},
                                                            {GMASK,mask}};
    return StringReplace(mask.empty()? GBaseRectangle : GBaseRectangleMask,Context);
}


std::string CreatePolyline(const std::vector<SCircuitLocation> &locations, const std::string &fill, const std::string &stroke, int width){
    std::string Points;
    for(auto &Location: locations){
        if(!Points.empty()){
            Points += " ";
        }
        Points += std::to_string(Location.DX) + "," + std::to_string(Location.DY);
    }
    std::unordered_map<std::string,std::string> Context = { {GPOINTS,Points},
                                                            {GFILL,fill},
                                                            {GSTROKE,stroke},
                                                            {GWIDTH,std::to_string(width)}};
    return StringReplace(GBasePolyline,Context);
}

std::string CreateLine(const SCircuitLocation &start, const SCircuitLocation &end, const std::string &fill, const std::string &stroke, int width){
    return CreatePolyline({start,end},fill,stroke,width);
}

std::string CreatePin(const SCircuitLocation &location, bool fillconnect, const std::string &color){
    std::string ReturnString; 
    if(fillconnect){
        ReturnString += CreateLine({location.DX-GPinKeepout,location.DY},{location.DX+GPinKeepout,location.DY},GColorNone,color,GTraceWidth*2);
        ReturnString += CreateLine({location.DX,location.DY-GPinKeepout},{location.DX,location.DY+GPinKeepout},GColorNone,color,GTraceWidth*2);
    }
    ReturnString += CreateCircle(location,GPinExterior,color) + CreateCircle(location,GPinInterior,GColorWhite) + "\n";
    return ReturnString;
}

std::string CreateTrace(const CCircuitTrace &trace, const std::string &color){
    std::vector<SCircuitLocation> Locations;
    for(std::size_t Index = 0; Index < trace.EdgeCount(); Index++){
        SCircuitTraceEdge Edge;
        trace.Edge(Index,Edge);
        if(Locations.empty()){
            Locations.push_back(ScaleLocation(Edge.DSource));
        }
        Locations.push_back(ScaleLocation(Edge.DDestination));
    }
    return CreatePolyline(Locations,GColorNone,color,GTraceWidth) + CreateCircle(Locations.front(),GTraceCap,color) + CreateCircle(Locations.back(),GTraceCap,color) + "\n";
}

std::string CreateLayer(const std::string &name, const CCircuitLayer &layer, const CCircuitNetlist &netlist, const std::string &color){
    std::string NetStrings;
    std::string PinStrings;
    for(std::size_t NetIndex = 0; NetIndex < layer.NetCount(); NetIndex++){
        CCircuitNet Net;
        if(layer.CircuitNet(NetIndex,Net)){
            std::string TraceStrings;
            for(std::size_t TraceIndex = 0; TraceIndex < Net.TraceCount(); TraceIndex++){
                CCircuitTrace Trace;
                if(Net.Trace(TraceIndex,Trace)){
                    TraceStrings += CreateTrace(Trace,color);
                }
            }
            NetStrings += CreateGroup(IDFromName(Net.Name()),TraceStrings);
            std::vector<SCircuitLocation> PinLocations;
            std::size_t CircuitIndex;
            if(netlist.CircuitNameToIndex(Net.Name(), CircuitIndex)){
                for(std::size_t PinIndex = 0; PinIndex < netlist.PinCount(CircuitIndex); PinIndex++){
                    SCircuitLocation Location;
                    if(netlist.PinLocation(CircuitIndex,PinIndex,Location)){
                        PinLocations.push_back(ScaleLocation(Location));
                    }
                }
            }
            for(auto &Location : PinLocations){
                PinStrings += CreatePin(Location,false,color) + "\n";
            }
        }

    }
    return CreateGroup(IDFromName(name),NetStrings + PinStrings);
}

std::string CreateLayer(const std::string &name, const SCircuitLocation &dims, const CCircuitNetlist &netlist, const std::string &color){
    // Fill layer
    std::string PinStrings;
    std::size_t CircuitIndex;
    
    if(netlist.CircuitNameToIndex(name, CircuitIndex)){
        for(std::size_t PinIndex = 0; PinIndex < netlist.PinCount(CircuitIndex); PinIndex++){
            SCircuitLocation Location;
            if(netlist.PinLocation(CircuitIndex,PinIndex,Location)){
                PinStrings += CreatePin(ScaleLocation(Location),true,color) + "\n";
            }
        }
    }
    return CreateGroup(IDFromName(name), CreateRectangle({0,0},dims.DX,dims.DY,color,GMaskPinholes) + PinStrings + "\n");
}

std::string CreateLayers(const CCircuitBoard &board){
    auto BeginningLayers = board.LayerCount() / 2;
    CCircuitLayer Layer;
    std::string AllLayers;
    std::size_t TempIndex;
    const std::vector<std::string> LayerColors = { GColorSkyBlue, GColorBluishGreen, GColorReddishPurple, GColorOrange, GColorBlue, GColorYellow, GColorPurple, GColorVermillion};

    for(TLayerCoordinate LayerIndex = 0; LayerIndex < BeginningLayers; LayerIndex++){
        if(board.Layer(LayerIndex,Layer)){
            AllLayers += CreateLayer(StringReplace(GBaseLayerTitle,GLAYER_INDEX,std::to_string(LayerIndex)),Layer,board.Netlist(), LayerColors[LayerIndex % LayerColors.size()]);
        }
        
    }
    if(board.Netlist().CircuitNameToIndex(CircuitNameGound,TempIndex)){
        AllLayers += CreateLayer(CircuitNameGound,ScaleLocation(board.Dimensions()),board.Netlist(),GColorDarkCharcoal);
    }
    if(board.Netlist().CircuitNameToIndex(CircuitNameVCC,TempIndex)){
        AllLayers += CreateLayer(CircuitNameVCC,ScaleLocation(board.Dimensions()),board.Netlist(),GColorRed);
    }
    for(TLayerCoordinate LayerIndex = BeginningLayers; LayerIndex < board.LayerCount(); LayerIndex++){
        
        if(board.Layer(LayerIndex,Layer)){
            AllLayers += CreateLayer(StringReplace(GBaseLayerTitle,GLAYER_INDEX,std::to_string(LayerIndex)),Layer,board.Netlist(), LayerColors[LayerIndex % LayerColors.size()]);
        }
    }
    return AllLayers;
}

std::string CreatePinholes(const CCircuitNetlist &netlist){
    std::string ReturnString;
    for(std::size_t CircuitIndex = 0; CircuitIndex < netlist.CircuitCount(); CircuitIndex++){
        for(std::size_t PinIndex = 0; PinIndex < netlist.PinCount(CircuitIndex); PinIndex++){
            SCircuitLocation Location;
            if(netlist.PinLocation(CircuitIndex,PinIndex,Location)){
                ReturnString += CreateCircle(ScaleLocation(Location),GPinKeepout,GColorBlack) + "\n";
            }
        }
    }
    return ReturnString;
}

void SaveCircuitBoard(const CCircuitBoard &board, const std::string &filename, const std::string &title){
    std::ofstream OutFile(filename);
    std::vector<std::vector<std::string>> LayerNameGroups;
    LayerNameGroups.push_back({CircuitNameGound});
    for(std::size_t LayerIndex = 0; LayerIndex < board.LayerCount(); LayerIndex++){
        LayerNameGroups.push_back({StringReplace(GBaseLayerTitle,GLAYER_INDEX,std::to_string(LayerIndex))});
        CCircuitLayer Layer;
        if(board.Layer(LayerIndex,Layer)){
            for(std::size_t NetIndex = 0; NetIndex < Layer.NetCount(); NetIndex++){
                CCircuitNet Net;
                if(Layer.CircuitNet(NetIndex,Net)){
                    LayerNameGroups.back().push_back(Net.Name());
                }
            }
        }
        std::sort(LayerNameGroups.back().begin()+1,LayerNameGroups.back().end());
    }
    LayerNameGroups.push_back({CircuitNameVCC});
    auto PinholeIndent = GetExpectedIndent(GBaseHTML,GPINHOLES);
    auto Pinholes = BlockIndent(CreatePinholes(board.Netlist()),PinholeIndent);
    auto ControlsIndent = GetExpectedIndent(GBaseHTML,GLAYER_NAMES);
    auto Controls = BlockIndent(CreateLabels(LayerNameGroups),ControlsIndent);
    auto LayerIndent = GetExpectedIndent(GBaseHTML,GLAYERS);
    auto Layers = BlockIndent(CreateLayers(board),LayerIndent);
    auto Dimensions = ScaleLocation(board.Dimensions());
    auto HTML = StringReplace(GBaseHTML,{   {GTITLE,title},
                                            {GWIDTH,std::to_string(Dimensions.DX)},
                                            {GHEIGHT,std::to_string(Dimensions.DY)},
                                            {GPINHOLES,Pinholes},
                                            {GLAYER_NAMES,Controls},
                                            {GLAYERS,Layers}});
    OutFile<<HTML;
}
