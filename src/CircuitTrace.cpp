#include "CircuitTrace.h"
#include <algorithm>


struct CCircuitTrace::SImplementation{

    // storeing trace.
    std::vector<SCircuitLocation> DLocations;

    std::vector<SCircuitTraceEdge> DEdge; // in between 2 locations

    // name of circuit
    TCircuitName DName = ""; 

    
    TCircuitName Name() const{
        return DName;
    }

    TCircuitName Name(const TCircuitName &name){ // a little bit of hard coding
        if (name == "")
        {
            return DName;
        }
        DName = name;
        return DName;
    }

    std::size_t LocationCount() const{
        return DLocations.size();
    }

    bool Location(std::size_t index, SCircuitLocation &location) const{ // gets the location using index and stores in location???
        if(index >= LocationCount()){
            return false;
        }
        location = DLocations[index];
        return true;
    }

    std::size_t EdgeCount() const{
        return DEdge.size();
    }

    bool Edge(std::size_t index, SCircuitTraceEdge &edge) const{
        if(index >= EdgeCount()){
            return false;
        }
        edge = DEdge[index];
        return true;
        
    }

    bool SourceDestinationAsEdge(SCircuitTraceEdge &edge) const{ // gives back source and destination of an edge
        if(DEdge.empty()){
            return false;
        }
        edge.DSource = DEdge.front().DSource;
        edge.DDestination = DEdge.back().DDestination;
        return true;
    }
    
    bool AddEdge(const SCircuitTraceEdge &edge){
        if(DLocations.empty()){ // checks if empty
            DLocations.push_back(edge.DSource);
        }

        SCircuitTraceEdge Reverse{edge.DDestination, edge.DSource}; // reverse of a edge 

        for(int i = 0; i < EdgeCount(); i++){ //going through to see if there are duplicate edges
            if(find(DEdge.begin(),DEdge.end(),edge ) != DEdge.end()){
                return false;
            }
            if(find(DEdge.begin(),DEdge.end(),Reverse ) != DEdge.end()){ // possible reverse edge dif
                return false;
            }
        }

        DEdge.push_back(edge);

        for(const auto &Loc : {edge.DSource, edge.DDestination}){ // Checks if the location has been added
            if(find(DLocations.begin(), DLocations.end(), Loc) == DLocations.end()){
                DLocations.push_back(Loc);
            }
        }

        return true;
    }

    bool ConcatenateTrace(const CCircuitTrace &trace){ // add trace to the end of Prev trace
        const std::size_t Count = trace.EdgeCount();
        if(Count == 0){ //check if trace is empty
            return false;
        }

        SCircuitTraceEdge Span;
        trace.SourceDestinationAsEdge(Span);

        bool Flip = false; 
        if(!DEdge.empty()){ // checking if the source of new trace matches with the end of old trace
            const SCircuitLocation &End = DEdge.back().DDestination;
            Flip = !(Span.DSource == End) && (Span.DDestination == End);
        }

        SCircuitTraceEdge Current;
        if(Flip){
            for(std::size_t I = Count; I > 0; I--){ // 
                trace.Edge(I, Current);
                AddEdge(SCircuitTraceEdge{Current.DDestination, Current.DSource});
            }
        }
        else{
            for(std::size_t I = 0; I < Count; I++){
                trace.Edge(I, Current);
                AddEdge(Current);
            }
        }
        return true;
    }

    bool IsBroken() const{
        const auto &Edge = DEdge; // idea for broken
        for(std::size_t Index = 0; Index + 1 < Edge.size(); Index++){
            if(!(Edge[Index].DDestination == Edge[Index + 1].DSource)){
                return true;
            }
        }
        return false;
    }

    bool Fix(){

        if(DEdge.size() > 1) {

            if(DEdge[0].DDestination != DEdge[1].DSource && (DEdge[0].DSource == DEdge[1].DSource || DEdge[0].DSource == DEdge[1].DDestination)) {

                std::swap(DEdge[0].DSource, DEdge[0].DDestination);

            }

        }

        for(std::size_t index = 0; index + 1 < DEdge.size(); index++) {

            if(DEdge[index].DDestination == DEdge[index + 1].DSource) continue;

            if(DEdge[index].DDestination == DEdge[index + 1].DDestination) std::swap(DEdge[index + 1].DSource, DEdge[index + 1].DDestination);

            else {

                SCircuitLocation p1 = DEdge[index].DDestination;
                SCircuitLocation p2 = DEdge[index + 1].DSource;

                long dx = static_cast<long>(p2.DX) - static_cast<long>(p1.DX);
                long dy = static_cast<long>(p2.DY) - static_cast<long>(p1.DY);
                
                long adx = std::abs(dx);
                long ady = std::abs(dy);

                if(adx == 0 || ady == 0 || adx == ady) {

                    DEdge.insert(DEdge.begin() + index + 1, SCircuitTraceEdge{p1, p2});

                    index++;

                }
                else {

                    long diag = std::min(adx, ady);

                    long sx = (dx > 0) ? 1 : -1;

                    long sy = (dy > 0) ? 1 : -1;

                    SCircuitLocation mid{
                        static_cast<TCircuitCoordinate>(static_cast<long>(p1.DX) + sx * diag),
                        static_cast<TCircuitCoordinate>(static_cast<long>(p1.DY) + sy * diag)
                    };

                    DEdge.insert(DEdge.begin() + index + 1, SCircuitTraceEdge{p1, mid});

                    DEdge.insert(DEdge.begin() + index + 2, SCircuitTraceEdge{mid, p2});

                    index += 2;

                }

            }

        }

        DLocations.clear();
        if(!DEdge.empty()) {
            
            DLocations.push_back(DEdge[0].DSource);
        
            for(const auto& edge : DEdge) {

                DLocations.push_back(edge.DDestination);

            }

        }

        return !IsBroken();

    }

    CCircuitTrace Simplify() const{
        return CCircuitTrace();
    }

    CCircuitTrace Elaborate() const{
        return CCircuitTrace();
    }
};

CCircuitTrace::CCircuitTrace() : DImplementation(std::make_unique<SImplementation>()){
}

CCircuitTrace::CCircuitTrace(const CCircuitTrace &trace) : DImplementation(std::make_unique<SImplementation>(*trace.DImplementation)){
}

CCircuitTrace::CCircuitTrace(CCircuitTrace &&trace) noexcept = default;

CCircuitTrace::~CCircuitTrace() = default;

CCircuitTrace &CCircuitTrace::operator=(const CCircuitTrace &trace){
    if(this != &trace){
        *DImplementation = *trace.DImplementation;
    }
    return *this;
}

CCircuitTrace &CCircuitTrace::operator=(CCircuitTrace &&trace) noexcept = default;

// Returns the name of the circuit associated with this trace.
TCircuitName CCircuitTrace::Name() const{
    return DImplementation->Name();
}

// Sets the name of the circuit associated with this trace.
TCircuitName CCircuitTrace::Name(const TCircuitName &name){
    return DImplementation->Name(name);
}

// Returns the total number of locations in the trace.
std::size_t CCircuitTrace::LocationCount() const{
    return DImplementation->LocationCount();
}

// Gets the location at the specified index.
bool CCircuitTrace::Location(std::size_t index, SCircuitLocation &location) const{
    return DImplementation->Location(index,location);
}

// Returns the total number of edges in the trace.
std::size_t CCircuitTrace::EdgeCount() const{
    return DImplementation->EdgeCount();
}

// Gets the edge at the specified index.
bool CCircuitTrace::Edge(std::size_t index, SCircuitTraceEdge &edge) const{
    return DImplementation->Edge(index,edge);
}

// Gets the edge connecting the first and last locations of the trace.
bool CCircuitTrace::SourceDestinationAsEdge(SCircuitTraceEdge &edge) const{
    return DImplementation->SourceDestinationAsEdge(edge);
}

// Adds an edge to the trace.
bool CCircuitTrace::AddEdge(const SCircuitTraceEdge &edge){
    return DImplementation->AddEdge(edge);
}

// Appends another circuit trace to this trace.
bool CCircuitTrace::ConcatenateTrace(const CCircuitTrace &trace){
    return DImplementation->ConcatenateTrace(trace);
}

// Checks whether the trace contains broken or disconnected connections.
bool CCircuitTrace::IsBroken() const{
    return DImplementation->IsBroken();
}

// Attempts to repair broken connections in the trace.
bool CCircuitTrace::Fix(){
    return DImplementation->Fix();
}

// Returns a simplified version of the trace.
CCircuitTrace CCircuitTrace::Simplify() const{
    return DImplementation->Simplify();
}

// Returns a more detailed version of the trace.
CCircuitTrace CCircuitTrace::Elaborate() const{
    return DImplementation->Elaborate();
}