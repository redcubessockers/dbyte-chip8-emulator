# ByteTemple Forge

ByteTemple Forge is a compact HolyC-inspired systems environment written in DByte v12. The current executable is a playable deterministic dungeon campaign rather than a print-only command demo. It models room progression, inventory, hit points, a trap, a typed puzzle gate, scoring, replay output, generics, closures, and lexical cleanup.

## Quick start

Run these commands from the project root:

```powershell
dbyte check src/main.dby
dbyte run src/main.dby
dbyte run --vm src/main.dby
dbyte check tests/forge_contract.dby
dbyte run tests/forge_contract.dby
```

The Hosted tree interpreter and bytecode VM should produce the same campaign transcript. The entry point is intentionally deterministic because the current compiler build does not expose stable `std.env`, `std.fs`, or local-module bindings in this standalone execution path.

## Campaign

The campaign is called **The Executable Seal**. The player starts in the Entry Vault with three hit points and no inventory. The fixed route is:

```text
LOOK -> TAKE -> MOVE -> TRAP -> ALIGN -> MOVE -> SOLVE
```

The route is implemented as typed state transitions. State is packed into a `u32` using room, inventory, hit points, and score fields. Every action returns `Result[u32, u32]`; `?` propagates a failed transition without hidden exceptions or unwinding.

The expected ending includes:

```text
[RELIC] executable seal accepted
[SCORE] base: 142
[SCORE] flawless-route bonus: 50
[SCORE] final: 192
[REPLAY] LOOK > TAKE > MOVE > TRAP > ALIGN > MOVE > SOLVE
[CAMPAIGN] completed with score: 192
```

The trap is intentional. It removes one hit point and subtracts three score points, so the campaign demonstrates state mutation through explicit returned state rather than through global hidden state.

## DByte feature matrix

| Feature | Where it appears |
|---|---|
| `Result[T, E]` | Every state transition and campaign report. |
| `?` propagation | `advance` and `play_campaign`. |
| Exhaustive `match` | `report` over `Result.Ok` and `Result.Err`. |
| Generics | `choose[T]` with explicit `choose[u32](...)`. |
| Explicit closure | The legacy puzzle library retains a borrow-capturing arithmetic closure. |
| Lexical `defer` | Campaign scope cleanup message. |
| Fixed-width types | State, inventory, hit points, score, and action codes use `u32`. |
| Deterministic replay | The campaign prints its canonical action sequence. |

## Project layout

| File | Purpose |
|---|---|
| `src/main.dby` | Stable standalone playable campaign executable. |
| `src/forge_puzzle.dby` | Reference puzzle library with payload enum and typed snapshot model. |
| `src/forge_binary.dby` | Optional filesystem/buffer/binary/hash/encoding adapter. |
| `src/forge_native.dby` | Native-only `@repr(C)` and `extern "C"` seam. |
| `src/parser.dby` | Legacy parser reference using a VM-safe numeric boundary. |
| `src/commands.dby` | Legacy command reference for future local-module integration. |
| `src/puzzle.dby` | Legacy typed puzzle reference. |
| `src/model.dby` | Legacy fixed-shape model reference. |
| `tests/forge_contract.dby` | Contract test for `Result` and explicit generic application. |
| `FORGE_DESIGN.md` | Architecture and protocol notes. |
| `VALIDATION.md` | Compiler diagnostics and validation status. |

## Why the executable is standalone

Earlier compiler diagnostics showed that this build does not create bindings for several local or standard-library aliases inside a standalone executable. The stable core therefore avoids imports, custom enum constructors, `Option` expression bindings, filesystem calls, and list literals at top level. The richer adapters remain in separate modules so they can be enabled when the compiler profile exposes those interfaces consistently.

## Native and C ABI direction

`src/forge_native.dby` isolates the i686 cdecl boundary. The intended interface uses scalar values no wider than 32 bits and C-compatible function pointers. Aggregates should cross the boundary through pointers to `@repr(C)` structs rather than by-value payload enums or structs.

```powershell
dbytec build src/forge_native.dby --target i686-dbyte-none --emit obj -o build/forge_native.o
```

## Verification

Run the executable through both hosted engines and compare the transcript:

```powershell
dbyte run src/main.dby > tree.txt
dbyte run --vm src/main.dby > vm.txt
fc.exe tree.txt vm.txt
```

Run the contract test separately:

```powershell
dbyte check tests/forge_contract.dby
dbyte run tests/forge_contract.dby
```

## References

[1]: https://dbytelang.site/ "DByte official site and v12 language materials"
[2]: https://dbytelang.site/docs/12.0/getting-started/first-program/ "DByte first program and Hosted execution"
[3]: https://dbytelang.site/docs/12.0/language/result/ "DByte Result and propagation"
[4]: https://dbytelang.site/docs/12.0/language/payload-enums/ "DByte payload enums"
[5]: https://dbytelang.site/docs/12.0/language/closures/ "DByte explicit-capture closures"
[6]: https://dbytelang.site/docs/12.0/language/defer/ "DByte lexical defer"

## Optional argv-driven CLI adapter

`src/forge_cli.dby` is the real command-line adapter. It is intentionally separate from the compiler-stable core because the current compiler profile has not consistently exposed `std.env` in standalone files. If the profile supports that module, use:

```powershell
dbyte check src/forge_cli.dby
dbyte run src/forge_cli.dby help
dbyte run src/forge_cli.dby status
dbyte run src/forge_cli.dby scan
dbyte run src/forge_cli.dby play
dbyte run src/forge_cli.dby solve 42
dbyte run src/forge_cli.dby replay
dbyte run --vm src/forge_cli.dby play
```

`play` executes the same state machine through a single command invocation. The stable `src/main.dby` remains the showcase fallback and always runs the canonical campaign without external bindings.
