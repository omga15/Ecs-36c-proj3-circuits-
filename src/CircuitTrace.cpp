#include "CircuitTrace.h"


struct CCircuitTrace::SImplementation{

    TCircuitName Name() const{
        return TCircuitName();
    }

    TCircuitName Name(const TCircuitName &name){
        return TCircuitName();
    }

    std::size_t LocationCount() const{
        return 0;
    }

    bool Location(std::size_t index, SCircuitLocation &location) const{
        return false;
    }

    std::size_t EdgeCount() const{
        return 0;
    }

    bool Edge(std::size_t index, SCircuitTraceEdge &edge) const{
        return false;
    }

    bool SourceDestinationAsEdge(SCircuitTraceEdge &edge) const{
        return false;
    }

    bool AddEdge(const SCircuitTraceEdge &edge){
        return false;
    }

    bool ConcatenateTrace(const CCircuitTrace &trace){
        return false;
    }

    bool IsBroken() const{
        return false;
    }

    bool Fix(){
        return false;
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

CCircuitTrace::CCircuitTrace(const CCircuitTrace &trace)
    : DImplementation(std::make_unique<SImplementation>(*trace.DImplementation)){
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