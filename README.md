# Project 3 README

Student 1: Patrick Wang (924885084)

Student 2: Lucas Henson (920780125)

## Project Status
The current status of the project is that its fully completed and passes all the tests.

## Known Issues
There are no known issues at this time, all tests pass.

## Code References
* [Dijkstra's algorithm](https://graphisual.app/algorithm/dijkstra)
* [Exceptions/throw reference](https://en.cppreference.com/cpp/language/throw)
* [Bit Packing Reference that also covers bitwise operations](https://www.cs.cornell.edu/courses/cs3410/2024fa/notes/bitpack.html)

## Generative AI Use
Google Gemini was used to generate high-level pseudocode for several methods in the project.

### Prompt 1
Generate pseudocode (no actual implementation) for this method: bool SetVertexConnectionMask(TVertexID id, TGridVertexConnectionMask mask) noexcept

### Response 1
Here is the high-level pseudocode for SetVertexConnectionMask:
text
FUNCTION SetVertexConnectionMask(id, mask)
    // 1. Validate that the vertex ID exists in the grid
    IF id is out of bounds (e.g., id >= total number of vertices) THEN
        RETURN false
    END IF

    // 2. Store the new connection bitmask for this vertex
    Set the vertex's connection mask at index `id` to `mask`

    // 3. Report success
    RETURN true
END FUNCTION
Use code with caution.
(Note: Depending on whether your grid stores connections symmetrically on neighboring vertices or uses mask purely as a per-vertex property mask, step 2 either directly assigns the vertex's stored mask or updates only the directions allowed by mask.)
Send over the next method whenever you're ready, or let me know if SetVertexConnectionMask also needs to enforce grid boundary limits (like blocking outward edges on border vertices)!


### Changes/Comments 1
This was the initial prompt that I used to get a general idea of how I would go about implementing this type of method. I realized that I needed to account for the boundaries of the graph, and got a more specific example using prompt 2.

### Prompt 2
With boundary clipping

### Response 2
Here is the high-level pseudocode for SetVertexConnectionMask with boundary clipping:
text
FUNCTION SetVertexConnectionMask(id, mask)
    // 1. Validate vertex ID
    IF id is out of bounds (id >= width * height) THEN
        RETURN false
    END IF

    // 2. Convert linear vertex ID to 2D grid coordinates (x, y)
    x = id MOD width
    y = id DIV width

    // 3. Build a valid-direction mask by clipping out-of-bounds edges
    validMask = ALL_DIRECTIONS
    IF y == 0 THEN
        Remove NORTH (and NORTH_EAST, NORTH_WEST) from validMask
    END IF
    IF y == height - 1 THEN
        Remove SOUTH (and SOUTH_EAST, SOUTH_WEST) from validMask
    END IF
    IF x == 0 THEN
        Remove WEST (and NORTH_WEST, SOUTH_WEST) from validMask
    END IF
    IF x == width - 1 THEN
        Remove EAST (and NORTH_EAST, SOUTH_EAST) from validMask
    END IF

    // 4. Apply boundary clipping to the requested mask and store it
    clippedMask = mask BITWISE_AND validMask
    Store clippedMask as the connection mask for vertex `id`

    RETURN true
END FUNCTION
Use code with caution.
Send over the next method whenever you're ready!


### Changes/Comments 2
Again, acted as a sort of guide for the general direction the method should go in. Created arrays to act as the direction values as I had seen done in one of the test suites for more convenience.

### Prompt 3
generate pseudocode (again, no impl) for this method: bool SetGridConnectionPattern(const SGridConnectionPattern &pattern) noexcept {

### Response 3
Here is the high-level pseudocode for SetGridConnectionPattern:
text
FUNCTION SetGridConnectionPattern(pattern)
    // 1. Validate the incoming pattern dimensions or configuration
    IF pattern has invalid or empty dimensions (e.g., width == 0 or height == 0) THEN
        RETURN false
    END IF

    // 2. Save the new grid connection pattern
    Store `pattern` as the grid's active connection pattern

    // 3. Recompute each vertex's connection mask across the entire grid
    FOR y FROM 0 TO grid_height - 1 DO
        FOR x FROM 0 TO grid_width - 1 DO
            // Determine which tile/cell in `pattern` applies to coordinate (x, y)
            pattern_x = x MOD pattern.width
            pattern_y = y MOD pattern.height
            rawMask = pattern.GetMask(pattern_x, pattern_y)

            // Clip out-of-bounds directions for vertices on the grid edges
            validMask = ComputeBoundaryValidMask(x, y, grid_width, grid_height)
            clippedMask = rawMask BITWISE_AND validMask

            // Update the vertex's stored connection mask
            id = y * grid_width + x
            Store clippedMask for vertex `id`
        END FOR
    END FOR

    // 4. Report success
    RETURN true
END FUNCTION
Use code with caution.
(Note: If SGridConnectionPattern stores a fixed set of direction masks rather than a tiled 2D sub-grid—for example, separate masks for even/odd rows, even/odd columns, or a global uniform mask—Step 3 selects rawMask from pattern according to (x, y)'s parity or position before applying boundary clipping.)
Send over the next method whenever you're ready, or let me know how SGridConnectionPattern is structured if you'd like Step 3 tailored to its exact fields!

### Changes/Comments 3
Same story, I used it to get a general sense of direction for the implementation. Biggest change here would be that I used bit packing to store a bit to indicate if the mask had been set manually or not. 

### Prompt 4
generate high level pseudo code for a function that fixes a broken trace stored as locations and edges

### Response 4
Here is the high-level pseudocode for fixing a broken trace stored as a sequence of locations and edges:
text
FUNCTION FixTrace(locations, edges)
    // 1. Validate that there are enough points to form a trace
    IF locations has fewer than 2 points THEN
        RETURN false
    END IF

    // 2. Rebuild a clean, continuous list of valid edges from the stored locations
    newEdges = empty list

    FOR i FROM 0 TO locations.size - 2 DO
        start = locations[i]
        end = locations[i + 1]

        // Skip duplicate consecutive points
        IF start == end THEN
            CONTINUE
        END IF

        // Check if a direct valid step (orthogonal or 45-degree diagonal) exists
        IF IsValidStep(start, end) THEN
            Append Edge(start, end) to newEdges
        ELSE
            // Bridge the gap by inserting an intermediate waypoint
            // (e.g., move diagonally until aligned horizontally/vertically, then straight)
            waypoint = ComputeIntermediateWaypoint(start, end)
            Append Edge(start, waypoint) to newEdges
            Append Edge(waypoint, end) to newEdges
        END IF
    END FOR

    // 3. Synchronize the stored locations list so it matches the repaired edge chain
    newLocations = empty list
    IF newEdges is NOT empty THEN
        Append newEdges[0].source to newLocations
        FOR EACH edge IN newEdges DO
            Append edge.destination to newLocations
        END FOR
    END IF

    // 4. Replace the trace's old locations and edges with the repaired versions
    locations = newLocations
    edges = newEdges

    RETURN true
END FUNCTION
Use code with caution.
(Note: If the broken trace is defined primarily by its disconnected edges list rather than locations, Step 1 first extracts/orders the endpoints from edges into the sequence of locations before bridging any gaps or invalid angles in Step 2.)
Send over the next method or function whenever you're ready, or let me know if your trace rules require a specific routing constraint (such as strictly orthogonal vs. 45-degree diagonal steps)!

### Changes/Comments 4
Used as a way to get my bearings, as with the others. 

### Prompt 5
generate pseudo for a function that simplifies all of the traces in a circuit net

### Response 5
Here is the high-level pseudocode for simplifying all traces in a circuit net:
text
FUNCTION SimplifyTraces(netPins, netTraces)
    // 1. Validate that the net has traces to simplify
    IF netTraces is empty THEN
        RETURN false
    END IF

    // 2. Break all traces down into unit grid segments and deduplicate overlaps
    uniqueUnitEdges = empty list
    FOR EACH trace IN netTraces DO
        unitTrace = ElaborateIntoUnitEdges(trace)
        FOR EACH edge IN unitTrace.edges DO
            IF edge.source == edge.destination THEN
                CONTINUE
            END IF
            IF neither (edge.source -> edge.destination) nor (edge.destination -> edge.source) is in uniqueUnitEdges THEN
                Append edge to uniqueUnitEdges
            END IF
        END FOR
    END FOR

    // 3. Tally the connection degree of every grid location in the deduplicated graph
    locationDegree = empty map/list of (Location -> Count)
    FOR EACH edge IN uniqueUnitEdges DO
        Increment locationDegree[edge.source]
        Increment locationDegree[edge.destination]
    END FOR

    // Helper rule: A "stop node" is any location that is a pin OR has degree != 2
    // (i.e., dead-ends with degree 1, or T/X junctions with degree >= 3)

    // 4. Walk chains of unvisited unit edges between stop nodes
    visited = boolean array of size uniqueUnitEdges.size, initialized to false
    simplifiedTraces = empty list

    FOR EACH (location, degree) IN locationDegree DO
        IF location is NOT a stop node THEN
            CONTINUE
        END IF

        FOR i FROM 0 TO uniqueUnitEdges.size - 1 DO
            IF visited[i] is true OR uniqueUnitEdges[i] does not touch location THEN
                CONTINUE
            END IF

            newTrace = new empty Trace
            current = location

            // Take the first step outward from the stop node and keep walking until hitting another stop node
            StepAlongNextUnvisitedEdge(current, uniqueUnitEdges, visited, newTrace)
            WHILE current is NOT a stop node AND StepAlongNextUnvisitedEdge(current, uniqueUnitEdges, visited, newTrace) DO
                // Continue advancing along the degree-2 chain
            END WHILE

            // Merge collinear segments along the walked chain and store it
            Append SimplifySingleTrace(newTrace) to simplifiedTraces
        END FOR
    END FOR

    // 5. Fallback sweep for any isolated closed rings (all degree-2 nodes, no pins)
    FOR i FROM 0 TO uniqueUnitEdges.size - 1 DO
        IF visited[i] is true THEN
            CONTINUE
        END IF

        ringTrace = new empty Trace
        start = uniqueUnitEdges[i].source
        current = start

        StepAlongNextUnvisitedEdge(current, uniqueUnitEdges, visited, ringTrace)
        WHILE current != start AND StepAlongNextUnvisitedEdge(current, uniqueUnitEdges, visited, ringTrace) DO
            // Continue walking around the closed ring
        END WHILE

        Append SimplifySingleTrace(ringTrace) to simplifiedTraces
    END FOR

    // 6. Replace the net's traces with the newly simplified set
    netTraces = simplifiedTraces
    RETURN true
END FUNCTION
Use code with caution.
Send over the next method or function whenever you're ready!

### Changes/Comments 5
Could be optimized further with a map, I used a vector of pairs to store the location/count. Also ended up creating some helper functions for this method as the implementation ended up repeating a bit.