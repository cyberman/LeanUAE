
# LeanUAE Target Scope

## Purpose

LeanUAE is intentionally narrow in scope.

This project does not aim for broad compatibility across many host systems,
many Mac variants, or many AmigaOS generations.

LeanUAE is built around a fixed, truthful, and verifiable target.

That narrow scope is not a limitation to apologize for.
It is a deliberate engineering choice.

---

## Target Truth

LeanUAE targets:

- **AmigaOS 3.2+ only**
- **PowerPC only**
- **Mac OS X 10.5.8 Leopard only**

These targets are treated as **code truth**.

They are not provisional guesses.
They are not broad compatibility promises.
They are not placeholders for “maybe later”.

They define the project boundary.

---

## Amiga Target

LeanUAE is designed for **AmigaOS 3.2 and newer only**.

Earlier AmigaOS generations are out of scope.

This means:

- no design pressure from AmigaOS 3.1 or older
- no backward-compatibility burden for pre-3.2 assumptions
- no effort spent preserving older operating system edge cases
- no requirement to shape the project around historical lowest-common-denominator behavior

LeanUAE is allowed to treat **AmigaOS 3.2+** as the meaningful Amiga-side baseline.

---

## Host Target

LeanUAE is designed for **PowerPC on Mac OS X 10.5.8 Leopard only**.

Other host targets are out of scope unless explicitly revisited later.

This means:

- no Intel Leopard support claim
- no requirement to maintain dual PowerPC/Intel host truth
- no obligation to keep code paths alive that cannot be verified
- no host abstraction work merely to preserve hypothetical portability

The host truth is intentionally fixed to the last Leopard release on PowerPC.

---

## Verification Rule

LeanUAE does not claim support for targets that cannot be verified.

A target is considered supported only if it can be validated through:

- real project hardware
- or an equally trusted validation setup explicitly accepted by the project

This rule exists to prevent false compatibility claims.

LeanUAE prefers a smaller truthful support statement over a larger unverified one.

---

## Engineering Consequences

Because the target scope is fixed, LeanUAE may:

- cut code to the chosen host and guest truth
- remove compatibility branches that only exist for unsupported targets
- avoid conditional complexity that serves no verified project target
- optimize interfaces around known constraints instead of speculative portability
- prefer simpler code over broader code

This is a core project strength.

LeanUAE is not trying to become universally portable.
It is trying to become correct within a tightly defined target.

---

## Non-Goals

The following are explicitly not goals of LeanUAE:

- support for AmigaOS versions earlier than 3.2
- support for Intel Leopard without later verification
- support for broader macOS generations
- support for generalized cross-platform host reuse at the expense of target clarity
- compatibility theater

LeanUAE does not widen scope by default.

---

## Scope Expansion Rule

If the project ever considers a wider target set, that expansion must be treated as a new decision,
not as an assumption hidden in the code.

Any future support for:

- Intel Leopard
- other macOS releases
- earlier AmigaOS versions
- other host environments

must be explicitly justified, documented, and verifiable.

Until that happens, they remain out of scope.

---

## Design Principle

LeanUAE prefers:

- truthful support over theoretical support
- verified scope over guessed scope
- narrow target clarity over broad compatibility claims
- fewer code paths over more untested code paths

This principle applies to architecture, implementation, documentation, and release claims.

---

## Closing Statement

LeanUAE is intentionally strict about what it supports.

It targets **AmigaOS 3.2+ only** and **PowerPC on Mac OS X 10.5.8 only**.

These limits are not temporary excuses.
They are part of the project’s engineering discipline.

LeanUAE does not optimize for maximum reach.

It optimizes for a small, honest, verifiable target that can be built and understood without unnecessary additional fronts.
