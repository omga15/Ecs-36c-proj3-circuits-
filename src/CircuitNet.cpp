#include "CircuitNet.h"


struct CCircuitNet::SImplementation{
    TCircuitName DName; 
    
    struct Pins{
        SCircuitLocation DDestination;
    };

    std::vector<Pins> DPin;
    std::vector<bool> DDelete;

    TCircuitName Name() const{
        return DName;
    }

    TCircuitName Name(const TCircuitName &name){
        return DName = name;
    }

    std::size_t PinCount() const{
        return DPin.size();
    }

    bool PinLocation(std::size_t index, SCircuitLocation &location) const{
        if(index >= DPin.size()){
            location = DPin[index].DDestination;
        }       
        return false;
    }

    bool AddPinLocation(const SCircuitLocation &location){
        
        return false;
    }

    std::size_t TraceCount() const{
        return 0;
    }

    bool Trace(std::size_t index, CCircuitTrace &trace) const{
        return false;
    }

    bool AddTrace(const CCircuitTrace &trace){
        return false;
    }

    bool IsBroken() const{
        return false;
    }

    bool Fix(){
        return false;
    }

    bool SimplifyTraces(){
        return false;
    }

    bool ElaborateTraces(){
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