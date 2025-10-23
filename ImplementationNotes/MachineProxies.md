# Overview 

This implementation note is about the concept of "Machine Proxies"  or MPs. MPs have the application
of preventing events from bubbling to a node's owning machine and redirect it in particular ways. This
has applications for children nodes implementing functionality and we may want to wait for child events
before allowing a transition or other effect.

# Issues

Pros:
- Relatively simple for people to implement: Would create an object to be stored in the node that holds child nodes.

Cons:
- The proxying node would need to know what events it's waiting on. This is fine for non-extendable, however when utilizing placeholders, we don't have a NodeAPI implementation. Even if we did implement a NodeAPI, we would need to remap emitted events from nodes in the child makine extension more complex.
- Requires the making of a proxy each time you make some custom waiting functionality. Creates more class bloat.
- Implementation requires extra steps in initialization.
- Complex implementation in extended classes to handle communicating to child nodes what is being waited on.
- Imcomplete interactions with placeholder nodes.
