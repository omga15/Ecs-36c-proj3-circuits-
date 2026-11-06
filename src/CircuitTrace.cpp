#include <memory>
#include <vector>
#include "CircuitBoardDefinitions.h"


struct CCircuitTree{
    CCircuitTrace(const CCircuitTrace &trace);
        CCircuitTrace(CCircuitTrace &&trace) noexcept = default;
        ~CCircuitTrace();
        
        CCircuitTrace &operator=(const CCircuitTrace &trace);
        CCircuitTrace &operator=(CCircuitTrace &&trace) noexcept = default;

        TCircuitName Name() const;
        TCircuitName Name(const TCircuitName &name);

        std::size_t LocationCount() const;
        SCircuitLocation Location(std::size_t index) const;

        std::size_t EdgeCount() const; // this part should return the total matrix/mapping
        SCircuitTraceEdge Edge(std::size_t index) const;
        bool AddEdge(const SCircuitTraceEdge &edge);
        
        bool IsBroken() const;
        bool Fix() const;

        CCircuitTrace Simplify() const;
        CCircuitTrace Elaborate() const;
    
}
CCircuitTree CCircuitTrace(){}

CCircuitTrace(const CCircuitTrace &trace){
    Dimplmentation->CCircuitTree(tree);
}
CCircuitTrace(CCircuitTrace &&trace) noexcept = default;
~CCircuitTrace();

CCircuitTrace &operator=(const CCircuitTrace &trace);
CCircuitTrace &operator=(CCircuitTrace &&trace) noexcept = default;

TCircuitName Name() const;
TCircuitName Name(const TCircuitName &name);

std::size_t LocationCount() const;
SCircuitLocation Location(std::size_t index) const;

std::size_t EdgeCount() const; // this part should return the total matrix/mapping
SCircuitTraceEdge Edge(std::size_t index) const;
bool AddEdge(const SCircuitTraceEdge &edge);

bool IsBroken() const;
bool Fix() const;

CCircuitTrace Simplify() const;
CCircuitTrace Elaborate() const;