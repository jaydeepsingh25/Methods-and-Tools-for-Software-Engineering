# Methods-and-Tools-for-Software-Engineering
## The repository includes the assignment and project work for ECE 650 (Methods-and-Tools-for-Software-Engineering). The assignments and project aim to solve the vertex cover problem. The idea is for the police to able to minimize the number of cameras they need to install, and still be as effective as possible eith their monitoring.
## The project performs the analysis of Different Algorithms for solving Minimum Vertex Cover Problem. The algorithm compared in the analysis are:
* CNF-SAT-VC  
In this approach, we do a polynomial time reduction of the given vertex-cover problem to CNFSAT, which is further solver by using minisat SAT solver.
* APPROX-VC-1  
Pick a vertex of highest degree (most incident edges). Add it to your vertex cover and throw away all edges incident on that vertex. Repeat till no edges remain.
* APPROX-VC-2  
Pick an edge <u,v> and add both u and v to your vertex cover. Throw away all edges attached to u and v. Repeat till no edges remain.