import asyncio
from server.foundry.engine.router import AgentRouter
from server.foundry.engine.generator import FoundryGenerator
from server.foundry.engine.sandbox import DeterministicSandbox

async def main():
    print("=== QuanuX Foundry Core Engine Smoke Test ===\n")

    # 1. Instantiate the Router
    router = AgentRouter(provider="gemini", model="gemini-3.5-flash")
    print("[1] Router Online.")

    # 2. Mock generating an Intermediate Representation (IR)
    ir_payload = await router.generate_ir("Create a simple moving average crossover", "indicator")
    print(f"[2] Received deterministic IR Payload: {ir_payload['id']}")

    # 3. Instantiate the Generator and create variants
    generator = FoundryGenerator(router)
    py_code = await generator.generate_code(ir_payload, lang="python", version="3.14")
    cpp_code = await generator.generate_code(ir_payload, lang="cpp", version="20")
    print("[3] Generated Python and C++ variants from IR.")

    # 4. Run the variants through the Deterministic Sandbox
    sandbox = DeterministicSandbox()
    test_suite = {
        "python_3.14": py_code,
        "cpp_20": cpp_code
    }
    
    success = sandbox.run_equivalence_test(test_suite)
    if success:
         print("\n=== SMOKE TEST PASSED ===")
    else:
         print("\n=== SMOKE TEST FAILED ===")

if __name__ == "__main__":
    asyncio.run(main())
