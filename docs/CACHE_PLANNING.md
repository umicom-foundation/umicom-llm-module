![Umicom](../../../framework/resources/brand/umicom-logo.svg)

# Plan a dense KV-cache budget

Umicom LLM has an optional native console for the shared Framework dense cache planner. It calculates token capacity from supplied geometry. It does not load a model, change a model's context setting or reserve memory.

1. Configure Umicom Applications with **UMICOM_LLM_BUILD_CACHE_PLANNER=ON** and a Framework revision containing the memory-command API.
2. Build target **umicom-llm-cache-plan**.
3. Run the ten-argument example below.
4. Check both the exit code and the report. A fitting cache is only one component of model memory.

~~~powershell
cmake --build build/windows-ucrt64-all-debug --target umicom-llm-cache-plan --parallel 2
& "./build/windows-ucrt64-all-debug/bin/umicom-llm-cache-plan.exe" 1919 10 2 3 2 2 4 4 2 2
ctest --test-dir build/windows-ucrt64-all-debug -R "^llm.cache_plan." --output-on-failure --no-tests=error
~~~

Run these from the configured Applications repository. Stop after a failed command. If the configured runtime output directory differs, use that directory. The example should report nine affordable tokens, 192 bytes per token, 1,728 cache bytes and 191 unused bytes.

The numbers mean cache bytes, requested tokens, sequences, layers, key heads, value heads, key dimensions, value dimensions, key element bytes and value element bytes. Use **--help** alone for usage.

The product only decodes native command-line arguments and calls **UmiModelDenseKvCommand**. Parsing, overflow checks, geometry and reporting stay in Framework. The optional target uses the existing Umicom application-branding helper; it adds no graphical layout.

Dense payload arithmetic excludes weights, workspace, runtime, allocator alignment and device allocations. The backend must really use the supplied dense layout. A model can still fail to load when the cache calculation succeeds. Keep missing costs and current memory observations explicit.

The registered product test exercises this console with a fitting budget, a smaller budget, malformed decimal text and zero-token geometry. Its existence is not evidence of an executed application test.
