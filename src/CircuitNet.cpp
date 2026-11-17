#include "CircuitNet.h"
#include <algorithm>
#include <vector>
#include <utility>


struct CCircuitNet::SImplementation{

    // Pin storage related 
    TCircuitName DName;
    std::vector<SCircuitLocation> DPins;
    std::vector<CCircuitTrace> DTraces;

    TCircuitName Name() const{
        return DName;
    }

    TCircuitName Name(const TCircuitName &name){
        if (name == "")
        {
            return DName;
        }
        DName = name;
        return DName;
    }

    std::size_t PinCount() const{
        return DPins.size();
    }

    bool PinLocation(std::size_t index, SCircuitLocation &location) const{
        if(index >= DPins.size()){
            return false;
        }       
        location = DPins[index];
        return true;
    }

    bool AddPinLocation(const SCircuitLocation &location){ // amount of Pins
        const auto &Pins = DPins;
        for(size_t i = 0; i < PinCount(); i++){ //going through to see if there are duplicate pins
            SCircuitLocation Existing;
            if(PinLocation(i,Existing) && Existing == location){
                return false;
            }
        }
        DPins.push_back(location);
        return true;
    }

    std::size_t TraceCount() const{  // amount of traces
        return DTraces.size();
    }

    bool Trace(std::size_t index, CCircuitTrace &trace) const{ // find a trace index
        if(index >= TraceCount()){
            return false;
        }
        trace = DTraces[index]; 
        return true;
    }

    bool AddTrace(const CCircuitTrace &trace){ // adds trace

        if(DName != trace.Name()){
            return false;
        }
        
        for(const auto &CurrentTraces : DTraces)
        {
            
            if(CurrentTraces.Name() == trace.Name() && CurrentTraces.EdgeCount() == trace.EdgeCount()){
                SCircuitTraceEdge DEdge; //edge holder 
                SCircuitTraceEdge CEdge; // current Edge holder
                bool same = true;
                for(size_t i = 0; i < trace.EdgeCount(); i++){
                    CurrentTraces.Edge(i,DEdge);
                    trace.Edge(i,CEdge);
                    if(DEdge != CEdge){
                        same = false;
                        break;
                    }
                }
                if(same){return false;}
            }
            
        }
        
        DTraces.push_back(trace); 
        return true;
    }

    bool IsBroken() const{
        if(DTraces.empty())
        {
            return true;
        }
        for(const auto &trace : DTraces){
            if(trace.IsBroken()){
                return true;
            }
        }
        
        std::size_t TotalEdges = 0; // N pins should have N-1 edges
        for(const auto &Current : DTraces){
            TotalEdges += Current.EdgeCount();
        }
        if(!DPins.empty() && TotalEdges < DPins.size() - 1){
            return true;
        }
        return false;
    }

    bool Fix(){
        for(auto &trace : DTraces){
            if(trace.IsBroken()){
                return trace.Fix();
            }
        }
        return false;
    }

    bool SimplifyTraces(){

        if(DTraces.empty()) return false;

        std::vector<SCircuitTraceEdge> seenEdges;

        for(const auto &trace : DTraces) {

            CCircuitTrace elaborated = trace.Elaborate();

            for(std::size_t index = 0; index < elaborated.EdgeCount(); index++) {

                SCircuitTraceEdge edge;

                elaborated.Edge(index, edge);

                if(edge.DSource == edge.DDestination) continue;

                bool found = false;

                for(const auto &prevEdge : seenEdges) {

                    if((prevEdge.DSource == edge.DSource && prevEdge.DDestination == edge.DDestination) 
                    || (prevEdge.DDestination == edge.DSource && prevEdge.DSource == edge.DDestination)) {

                        found = true;

                        break;
                    }

                }

                if(!found) seenEdges.push_back(edge);
            }
        }

        using LCPair = std::pair<SCircuitLocation, int>;

        std::vector<LCPair> locDegree;

        for(const auto &edge : seenEdges) {

            for(const auto &endpoint : {edge.DSource, edge.DDestination}) {

                bool found = false;

                for(auto &pair : locDegree) {

                    if(pair.first == endpoint) {

                        pair.second++;

                        found = true;

                        break;

                    }
                }

                if(!found) locDegree.push_back({endpoint, 1});
            }
        }

        std::vector<bool> visited(seenEdges.size(), false);
        std::vector<CCircuitTrace> newTraces;

        for(const auto &pair : locDegree) {

            if(DegreeTwoAndNotInPins(pair.first, locDegree)) continue;

            for(std::size_t index = 0; index < seenEdges.size(); index++) {
                if(visited[index] || (pair.first != seenEdges[index].DSource && pair.first != seenEdges[index].DDestination)) {
                    continue;
                }
                CCircuitTrace newTrace;

                newTrace.Name(DName);

                SCircuitLocation current = pair.first;

                StepNextEdge(current, seenEdges, visited, newTrace);

                while(DegreeTwoAndNotInPins(current, locDegree) && StepNextEdge(current, seenEdges, visited, newTrace)) {}

                newTraces.push_back(newTrace.Simplify());

            }
        }

        for(std::size_t index = 0; index < seenEdges.size(); index++) {

            if(visited[index]) continue;

            CCircuitTrace stragglerTrace;
            stragglerTrace.Name(DName);

            SCircuitLocation start = seenEdges[index].DSource;
            SCircuitLocation current = start;

            StepNextEdge(current, seenEdges, visited, stragglerTrace);

            while(start != current && StepNextEdge(current, seenEdges, visited, stragglerTrace)) {}

            newTraces.push_back(stragglerTrace.Simplify());

        }

        DTraces = newTraces;

        return true;

    }

    bool ElaborateTraces(){

        if(DTraces.empty()) return false;

        for(auto& trace : DTraces) {

            trace = trace.Elaborate();

        }

        return true;
    }

    bool DegreeTwoAndNotInPins(SCircuitLocation loc, std::vector<std::pair<SCircuitLocation, int> >& locDegree) const {

        auto it = std::find(DPins.begin(), DPins.end(), loc);

        int deg = 0;

        for(const auto& pair : locDegree) {

            if(pair.first == loc) deg = pair.second;

        }

        return it == DPins.end() && deg == 2;

    }

    bool StepNextEdge(SCircuitLocation &current, const std::vector<SCircuitTraceEdge> &seenEdges, std::vector<bool> &visited, CCircuitTrace &outTrace) const {

        for(std::size_t j = 0; j < seenEdges.size(); j++) {

            if(visited[j] || (current != seenEdges[j].DSource && current != seenEdges[j].DDestination)) {

                continue;

            }

            visited[j] = true;

            SCircuitLocation next = (current == seenEdges[j].DSource) ? seenEdges[j].DDestination : seenEdges[j].DSource;

            outTrace.AddEdge({current, next});

            current = next;

            return true;
            
        }
        return false;
    }

};

CCircuitNet::CCircuitNet(){
    DImplementation = std::make_unique<SImplementation>();
}

CCircuitNet::CCircuitNet(const CCircuitNet &net){
    DImplementation = std::make_unique<SImplementation>(*net.DImplementation);
}

CCircuitNet::CCircuitNet(CCircuitNet &&net) noexcept = default;

CCircuitNet::~CCircuitNet(){}

CCircuitNet &CCircuitNet::operator=(const CCircuitNet &net){
    if(this != &net){
        *DImplementation = *net.DImplementation;
    }
    return *this;
}

CCircuitNet &CCircuitNet::operator=(CCircuitNet &&net) noexcept = default;

// Returns the name of the circuit associated with this net.
TCircuitName CCircuitNet::Name() const{
    return DImplementation->Name();
}

// Sets the name of the circuit associated with this net.
TCircuitName CCircuitNet::Name(const TCircuitName &name){
    return DImplementation->Name(name);
}

// Returns the total number of pin locations in the net.
std::size_t CCircuitNet::PinCount() const{
    return DImplementation->PinCount();
}

// Gets the pin location at the specified index.
bool CCircuitNet::PinLocation(std::size_t index, SCircuitLocation &location) const{
    return DImplementation->PinLocation(index,location);
}

// Adds a pin location to the net.
bool CCircuitNet::AddPinLocation(const SCircuitLocation &location){
    return DImplementation->AddPinLocation(location);
}

// Returns the total number of traces in the net.
std::size_t CCircuitNet::TraceCount() const{
    return DImplementation->TraceCount();
}

// Gets the trace at the specified index.
bool CCircuitNet::Trace(std::size_t index, CCircuitTrace &trace) const{
    return DImplementation->Trace(index,trace);
}

// Adds a trace to the net.
bool CCircuitNet::AddTrace(const CCircuitTrace &trace){
    return DImplementation->AddTrace(trace);
}

// Checks whether the net contains broken or disconnected connections.
bool CCircuitNet::IsBroken() const{
    return DImplementation->IsBroken();
}

// Attempts to repair broken connections in the net.
bool CCircuitNet::Fix(){
    return DImplementation->Fix();
}

// Simplifies all traces contained in the net.
bool CCircuitNet::SimplifyTraces(){
    return DImplementation->SimplifyTraces();
}

// Expands all traces contained in the net into more detailed representations.
bool CCircuitNet::ElaborateTraces(){
    return DImplementation->ElaborateTraces();
}