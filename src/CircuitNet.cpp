#include <memory>
#include <vector>
#include "CircuitNet.h"

struct CircuitNet::SImplementation{
    std::vector<>
    mutable std::vector<SCircuitLocation> Location;

    TCircuitName Name() const{

    }
    TCircuitName Name(const TCircuitName &name){

    }

    std::size_t PinCount() const{

    }
    SCircuitLocation PinLocation(std::size_t index) const{

    }
    bool AddPinLocation(const SCircuitLocation &location){

    }

    std::size_t TraceCount() const;
    CCircuitTrace Trace(std::size_t index) const;
        
};

CCircuitNet::CCircuitNet(){
    Dimplmentation = std::make_unique<SImplementation>();
}

CCircuitNet::~CCircuitNet(){

}


TCircuitName CCircuitNet CCircuitNet Name() const{
    return Dimplmentation->Name();
}

std::size_t CCircuitNet PinCount() noexcept{
    return Dimplmentation->PinCount();
}

SCircuitLocation CCircuitNet PinLocation(std::size_t index){
    return Dimplmentation->Pinlocation(index);
}

bool CCircuitNet AddPinLocation(const SCircuitLocation &location){
    return Dimplementation->AddPinLocation(location);
}

std::size_t CCircuitNet TraceCount(){
   return Dimplementation->TraceCount(); 
}

CCircuitTrace CCircuitNet Trace(std::size_t index) const{
    return Dimplementation->Trace(index);
}
