# Overview 

This implementation creates the concept of a "Placeholder" node, which allows nodes to be subsituted as necessary. This has application for node extensionality in child state machines.

# Issues

Pros:
- Increased extensionality possibilities.

Cons:
- Invalidates some of the execution model; Especially when nodes get substituted multiple times in a tree.
- Requires extensive API additions to handle both substitution and flattening.
