
## Performance Principle

For LeanUAE, performance is not limited to raw emulation throughput.

Performance also includes:

- host-side efficiency
- responsiveness
- startup cost
- dependency weight
- memory footprint
- binary size
- clarity of execution paths
- overall fit to Leopard as the target system

LeanUAE therefore treats deep Leopard integration as a performance strategy in its own right.

The project does not measure success only by how fast the emulated machine runs.  
It also measures success by how little host overhead is required to run it well.

A faster-feeling host with lower overhead is a legitimate performance gain, even if raw emulation speed remains unchanged.

This leads to several engineering consequences:

- native Leopard services take priority over foreign abstraction layers
- unnecessary host-side indirection should be removed
- dependency weight should be reduced where Leopard already provides the required capability
- smaller and clearer host-side code is preferred over generic portability shells
- hot paths must remain explicit and easy to reason about

Reduced binary size is a desired consequence of this approach, but not the sole objective.

The real objective is a lighter, more responsive, better integrated LeanUAE that fits Leopard as naturally as possible.