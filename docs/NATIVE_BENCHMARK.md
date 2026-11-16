# Umicom LLM: Native Checkpoint Completion Benchmark

![Umicom](../../../framework/resources/brand/umicom-logo.svg)

**Author:** Sammy Hegab · **Organisation:** Umicom Foundation · **Licence:** MIT

The module is a **thin consumer** of Umicom Framework. A new opt-in console
can display a reviewed source-completion benchmark without creating a second
language model, sampling implementation or training system.

## Building

Enable `UMICOM_LLM_BUILD_NATIVE_BENCHMARK=ON` in the root Umicom Applications
CMake configuration. The default remains OFF. The benchmark uses canonical
`Umicom::AiTraining`, `Umicom::desktop_system` and `Umicom::platform` services.
The executable remains `umicom-llm-benchmark`; no GUI layout is added.

<!-- Engineering history: the former dependency description is retained.
Shared memory observations and UTF-8 handling replace the minimal link.
CMake configuration. The default remains OFF. The target links only the
canonical `Umicom::AiTraining` service and is built as
`umicom-llm-benchmark`. No existing application menu, console, layout or test
has been changed or removed.
-->

## Running

```powershell
Set-Location "C:\umicom\Umicom-Applications"

& ".\build\windows-ucrt64-all-debug\bin\umicom-llm-benchmark.exe" `
    "C:\path\to\approved-decoder-checkpoint.umi" `
    ".\framework\learning\native-llm\fixtures\benchmark-reviewed.txt"
```

Consult `framework/learning/native-llm/BENCHMARK.md` for the reviewed suite
format, resource limits and byte-level score meanings.

**This does not compile, execute or trust model output.** Matching short
reference text is not evidence that a C23 function is safe or correct. The
educational model lacks training capacity for dependable programming assistance.

The original LLM workspace and console remain unchanged. Normal building
should not depend on this opt-in binary. The earlier antivirus quarantines on
Windows require vendor review and cannot be resolved by renaming executables
or weakening security rules.

## Optional memory observations

Add `--memory-observations` to capture self-process memory before load, after
load, after generation and after cleanup. Reports go to standard error, keeping
`--evidence-json` score output on standard output. No observation occurs by default.

The [step-by-step memory lesson](../../../framework/docs/learning/native-memory-evidence.html)
explains configuration, commands, unavailable counters and failure recovery.
Windows arguments use Framework UTF-8 decoding before checkpoint access.
Sampling failures are reported separately and do not change completion scores.
These observations exclude external model servers and device memory; they do
not establish a leak or a speed/memory improvement.
